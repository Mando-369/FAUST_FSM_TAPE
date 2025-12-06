// Variable Substep LUT Test
// Tests alternating between K120, K121, K122 LUTs to simulate
// the C++ scheduler's variable substep count behavior.
//
// For K121 mode (5.5 cycles per sample):
// - Fractional part = 0.5
// - Samples alternate: K121 on even, K122 on odd
// - This replicates C++ behavior where cursor accumulation
//   causes variable substep counts (121, 122, 121, 122...)

import("stdfaust.lib");
import("ja_lut_k120.lib");  // N-1: 120 substeps over 11π
import("ja_lut_k121.lib");  // N:   121 substeps over 11π (base)
import("ja_lut_k122.lib");  // N+1: 122 substeps over 11π

//==============================================================================
// tape_channel: Main processing function with variable substep LUT selection
//==============================================================================
tape_channel(input_gain_db, output_gain_db, drive_db, mix_val) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  // ===== Gains from function parameters =====
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = 1.0 / drive_gain;

  // ===== Physics parameters (fixed for prototype) =====
  Ms              = 320.0;
  a_density       = 720.0;
  k_pinning       = 280.0;
  c_reversibility = 0.18;
  alpha_coupling  = 0.015;

  // ===== Derived constants =====
  Ms_safe    = ba.if(Ms > 1e-6, Ms, 1e-6);
  alpha_norm = alpha_coupling;
  a_norm     = a_density / Ms_safe;
  k_norm     = k_pinning / Ms_safe;
  c_norm     = c_reversibility;
  bias_amp   = 0.41 * 11.0;

  // ===== Bias lookup table for substep 0 =====
  // K121: 5.5 cycles = 11π, 121 substeps
  tablesize_121 = 121;
  dphi_121 = 11.0 * ma.PI / tablesize_121;
  bias_gen_121(n) = sin((float(ba.period(n)) + 0.5) * dphi_121);
  bias_lut_121(idx) = rdtable(tablesize_121, bias_gen_121(tablesize_121), int(idx));

  // ===== Inverse substep counts for averaging =====
  sigma      = 1e-6;
  inv_120    = 1.0 / 120.0;
  inv_121    = 1.0 / 121.0;
  inv_122    = 1.0 / 122.0;
  inv_a_norm = 1.0 / a_norm;

  // ===== Real tanh =====
  fast_tanh(x) = ma.tanh(x);

  // ===== Substep 0 (computed in real-time) =====
  ja_substep0(bias_val, M_prev, H_prev, H_audio) = M1, H1
  with {
    H1 = H_audio + bias_amp * bias_val;
    dH = H1 - H_prev;
    He = H1 + alpha_norm * M_prev;

    x_man   = He * inv_a_norm;
    Man_e   = fast_tanh(x_man);
    Man_e2  = Man_e * Man_e;
    dMan_dH = (1.0 - Man_e2) * inv_a_norm;

    dir      = ba.if(dH >= 0.0, 1.0, -1.0);
    pin      = dir * k_norm - alpha_norm * (Man_e - M_prev);
    inv_pin  = 1.0 / (pin + sigma);

    denom     = 1.0 - c_norm * alpha_norm * dMan_dH;
    inv_denom = 1.0 / (denom + 1e-9);
    dMdH      = (c_norm * dMan_dH + (Man_e - M_prev) * inv_pin) * inv_denom;
    dM_step   = dMdH * dH;

    M_unclamped = M_prev + dM_step;
    M1          = max(-1.0, min(1.0, M_unclamped));
  };

  // ===== Variable Substep LUT Loop with 3-way Interpolation =====
  // Uses fractional cursor to smoothly interpolate across K120, K121, K122
  // cursor 0.0-0.5: interpolate K120 → K121
  // cursor 0.5-1.0: interpolate K121 → K122
  ja_loop_variable(M_prev, H_prev, H_audio, cursor) = M_end, H_end, Mavg
  with {
    // Compute substep 0 (same for all variants - same phase span)
    M1_H1 = ja_substep0(bias_lut_121(0), M_prev, H_prev, H_audio);
    M1 = ba.selector(0, 2, M1_H1);

    // Get M_end from all 3 LUTs
    M_end_120 = ja_lookup_m_end_k120(M1, H_audio);
    M_end_121 = ja_lookup_m_end_k121(M1, H_audio);
    M_end_122 = ja_lookup_m_end_k122(M1, H_audio);

    // Get sumM_rest from all 3 LUTs
    sumM_rest_120 = ja_lookup_sum_m_rest_k120(M1, H_audio);
    sumM_rest_121 = ja_lookup_sum_m_rest_k121(M1, H_audio);
    sumM_rest_122 = ja_lookup_sum_m_rest_k122(M1, H_audio);

    // Interpolation based on cursor position (0.0 to 1.0)
    // cursor 0.0-0.5: blend K120 → K121
    // cursor 0.5-1.0: blend K121 → K122
    // Using COSINE interpolation instead of linear for smoother transitions
    // and better preservation of harmonic detail
    in_lower_half = cursor < 0.5;
    t_linear_lower = cursor * 2.0;
    t_linear_upper = (cursor - 0.5) * 2.0;
    // Cosine interpolation: 0.5 * (1 - cos(π * t))
    t_lower = 0.5 * (1.0 - cos(ma.PI * t_linear_lower));
    t_upper = 0.5 * (1.0 - cos(ma.PI * t_linear_upper));

    // Interpolate M_end
    M_end_lower = M_end_120 * (1.0 - t_lower) + M_end_121 * t_lower;
    M_end_upper = M_end_121 * (1.0 - t_upper) + M_end_122 * t_upper;
    M_end = ba.if(in_lower_half, M_end_lower, M_end_upper);

    // Interpolate sumM_rest
    sumM_rest_lower = sumM_rest_120 * (1.0 - t_lower) + sumM_rest_121 * t_lower;
    sumM_rest_upper = sumM_rest_121 * (1.0 - t_upper) + sumM_rest_122 * t_upper;
    sumM_rest = ba.if(in_lower_half, sumM_rest_lower, sumM_rest_upper);

    // Interpolate inverse substep count for proper averaging
    inv_n_lower = inv_120 * (1.0 - t_lower) + inv_121 * t_lower;
    inv_n_upper = inv_121 * (1.0 - t_upper) + inv_122 * t_upper;
    inv_n = ba.if(in_lower_half, inv_n_lower, inv_n_upper);

    // Compute average magnetization
    Mavg = (M1 + sumM_rest) * inv_n;

    // H_end uses last bias value
    H_end = H_audio + bias_amp * bias_lut_121(120);
  };

  // ===== Streaming JA hysteresis with fractional cursor =====
  // Cursor accumulates fractional part (0.5 for K121 = 5.5 cycles)
  // Wraps at 1.0, giving smooth variation across all 3 LUTs
  ja_hysteresis(H_in) = (loop ~ (mem, mem, mem)) : ba.selector(2, 4)
  with {
    frac_increment = 0.5;  // 5.5 cycles → 0.5 fractional part

    loop(recM, recH, recCursor) = M_end, H_end, Mavg, nextCursor
    with {
      // Accumulate cursor, wrap at 1.0
      nextCursor = ma.frac(recCursor + frac_increment);

      // Run the variable LUT loop with current cursor
      result = ja_loop_variable(recM, recH, H_in, recCursor);
      M_end = ba.selector(0, 3, result);
      H_end = ba.selector(1, 3, result);
      Mavg = ba.selector(2, 3, result);
    };
  };

  // ===== Tape stage =====
  dc_blocker = fi.SVFTPT.HP2(10.0, 0.7071);

  tape_stage(x) =
    x * input_gain
    : *(drive_gain)
    : ja_hysteresis
    : dc_blocker
    : *(drive_comp);

  wet_gained = tape_stage : *(output_gain);
};

//==============================================================================
// tape_channel_ui: UI wrapper with sliders
//==============================================================================
tape_channel_ui =
  tape_channel(input_gain_db, output_gain_db, drive_db, mix)
with {
  input_gain_db  = hslider("Input Gain [dB]", 0.0, -24.0, 24.0, 0.1);
  output_gain_db = hslider("Output Gain [dB]", 15.9, -24.0, 48.0, 0.1);
  drive_db       = hslider("Drive [dB]", 0.0, -18.0, 18.0, 0.1);
  mix            = hslider("Mix [Dry->Wet]", 1.0, 0.0, 1.0, 0.01);
};

process = par(i, 2, tape_channel_ui);
