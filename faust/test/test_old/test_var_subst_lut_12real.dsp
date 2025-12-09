// Variable Substep LUT Test - 12 Real Substeps Version
//
// Computes 12 real JA physics substeps (10% of total) before LUT lookup.
// This provides more "alive" sound while keeping CPU low.
//
// Architecture:
// - Substeps 0..11: Real JA physics (computed every sample)
// - Substeps 12..N-1: LUT lookup (precomputed)
//
// LUTs generated with:
//   python generate_ja_lut.py --mode K121 --real-substeps 12 --use-rk4 --variants

import("stdfaust.lib");
import("ja_lut_k120.lib");  // N-1: 120 substeps, LUT covers 12..119
import("ja_lut_k121.lib");  // N:   121 substeps, LUT covers 12..120
import("ja_lut_k122.lib");  // N+1: 122 substeps, LUT covers 12..121

//==============================================================================
// Configuration
//==============================================================================
NUM_REAL_SUBSTEPS = 12;  // 10% of K121's 121 total substeps

//==============================================================================
// tape_channel: Main processing function with 12 real substeps + LUT
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
  sigma      = 1e-6;
  inv_a_norm = 1.0 / a_norm;

  // ===== Bias lookup table for real substeps =====
  // K121: 5.5 cycles = 11π, 121 substeps
  // We need bias values for substeps 0..11 (the real ones)
  tablesize_121 = 121;
  dphi_121 = 11.0 * ma.PI / tablesize_121;
  bias_gen_121(n) = sin((float(ba.period(n)) + 0.5) * dphi_121);
  bias_lut_121(idx) = rdtable(tablesize_121, bias_gen_121(tablesize_121), int(idx));

  // ===== Inverse substep counts for averaging =====
  inv_120 = 1.0 / 120.0;
  inv_121 = 1.0 / 121.0;
  inv_122 = 1.0 / 122.0;

  // ===== Real tanh =====
  fast_tanh(x) = ma.tanh(x);

  // ===== Single JA substep (computed in real-time) =====
  // Returns: M_new, H_new
  ja_substep(bias_val, M_prev, H_prev, H_audio) = M_new, H_new
  with {
    H_new = H_audio + bias_amp * bias_val;
    dH = H_new - H_prev;
    He = H_new + alpha_norm * M_prev;

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
    M_new       = max(-1.0, min(1.0, M_unclamped));
  };

  // ===== Chain of 12 real substeps =====
  // Input:  M_prev, H_prev, H_audio, sum_M_acc (accumulator)
  // Output: M_after_12, H_after_12, H_audio (passed through), sum_M_12
  //
  // Each substep:
  // - Computes JA physics with bias_lut_121(i)
  // - Adds M_new to accumulator
  // - Passes H_audio through unchanged

  ja_real_substep(i, M_prev, H_prev, H_audio, sum_M) = M_new, H_new, H_audio, sum_M_new
  with {
    substep_result = ja_substep(bias_lut_121(i), M_prev, H_prev, H_audio);
    M_new = ba.selector(0, 2, substep_result);
    H_new = ba.selector(1, 2, substep_result);
    sum_M_new = sum_M + M_new;
  };

  // Chain 12 substeps using seq
  // seq(i, N, f) chains f(0), f(1), ..., f(N-1)
  ja_12_real_substeps(M_prev, H_prev, H_audio) = M_12, H_12, H_audio_out, sum_M_12
  with {
    chain_result = M_prev, H_prev, H_audio, 0.0 : seq(i, NUM_REAL_SUBSTEPS, ja_real_substep(i));
    M_12 = ba.selector(0, 4, chain_result);
    H_12 = ba.selector(1, 4, chain_result);
    H_audio_out = ba.selector(2, 4, chain_result);
    sum_M_12 = ba.selector(3, 4, chain_result);
  };

  // ===== Variable Substep LUT Loop with 3-way Interpolation =====
  // Now uses M_12 (after 12 real substeps) for LUT lookup
  // cursor 0.0-0.5: interpolate K120 -> K121
  // cursor 0.5-1.0: interpolate K121 -> K122
  ja_loop_variable(M_prev, H_prev, H_audio, cursor) = M_end, H_end, Mavg
  with {
    // Compute 12 real substeps
    real_result = ja_12_real_substeps(M_prev, H_prev, H_audio);
    M_12 = ba.selector(0, 4, real_result);
    H_12 = ba.selector(1, 4, real_result);
    sum_M_real = ba.selector(3, 4, real_result);

    // Get M_end from all 3 LUTs (using M_12, not M_1)
    M_end_120 = ja_lookup_m_end_k120(M_12, H_audio);
    M_end_121 = ja_lookup_m_end_k121(M_12, H_audio);
    M_end_122 = ja_lookup_m_end_k122(M_12, H_audio);

    // Get sumM_rest from all 3 LUTs (substeps 12..N-1)
    sumM_rest_120 = ja_lookup_sum_m_rest_k120(M_12, H_audio);
    sumM_rest_121 = ja_lookup_sum_m_rest_k121(M_12, H_audio);
    sumM_rest_122 = ja_lookup_sum_m_rest_k122(M_12, H_audio);

    // Interpolation based on cursor position (0.0 to 1.0)
    // Using cosine interpolation for smooth transitions
    in_lower_half = cursor < 0.5;
    t_linear_lower = cursor * 2.0;
    t_linear_upper = (cursor - 0.5) * 2.0;
    t_lower = 0.5 * (1.0 - cos(ma.PI * t_linear_lower));
    t_upper = 0.5 * (1.0 - cos(ma.PI * t_linear_upper));

    // Interpolate M_end
    M_end_lower = M_end_120 * (1.0 - t_lower) + M_end_121 * t_lower;
    M_end_upper = M_end_121 * (1.0 - t_upper) + M_end_122 * t_upper;
    M_end = ba.if(in_lower_half, M_end_lower, M_end_upper);

    // Interpolate sumM_rest (the LUT portion)
    sumM_rest_lower = sumM_rest_120 * (1.0 - t_lower) + sumM_rest_121 * t_lower;
    sumM_rest_upper = sumM_rest_121 * (1.0 - t_upper) + sumM_rest_122 * t_upper;
    sumM_rest = ba.if(in_lower_half, sumM_rest_lower, sumM_rest_upper);

    // Interpolate inverse substep count for proper averaging
    inv_n_lower = inv_120 * (1.0 - t_lower) + inv_121 * t_lower;
    inv_n_upper = inv_121 * (1.0 - t_upper) + inv_122 * t_upper;
    inv_n = ba.if(in_lower_half, inv_n_lower, inv_n_upper);

    // Compute average magnetization
    // Total sum = sum from real substeps + sum from LUT
    Mavg = (sum_M_real + sumM_rest) * inv_n;

    // H_end uses last bias value
    H_end = H_audio + bias_amp * bias_lut_121(120);
  };

  // ===== Streaming JA hysteresis with fractional cursor =====
  ja_hysteresis(H_in) = (loop ~ (mem, mem, mem)) : ba.selector(2, 4)
  with {
    frac_increment = 0.5;  // 5.5 cycles -> 0.5 fractional part

    loop(recM, recH, recCursor) = M_end, H_end, Mavg, nextCursor
    with {
      nextCursor = ma.frac(recCursor + frac_increment);
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
