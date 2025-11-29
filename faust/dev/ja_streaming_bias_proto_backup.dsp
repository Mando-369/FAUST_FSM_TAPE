import("stdfaust.lib");
import("ja_lut_k60.lib");  // 2D LUT for K60 substeps 1..65

// Streaming JA hysteresis prototype with phase-locked bias oscillator.
// Uses precomputed LUT for bias sin() values (computed at init, not runtime).
// Matches C++ JAHysteresisScheduler "Normal" quality substep counts.
//
// K60_LUT mode: 1 real substep + 2D LUT lookup for remainder (massive CPU savings)

// ===== Physics parameters (fixed for prototype) =====
Ms              = 320.0;
a_density       = 720.0;
k_pinning       = 280.0;
c_reversibility = 0.18;
alpha_coupling  = 0.015;

// ===== User controls =====
input_gain  = hslider("Input Gain [dB]", -7.0, -24.0, 24.0, 0.1) : ba.db2linear : si.smoo;
output_gain = hslider("Output Gain [dB]", 40.0, -24.0, 48.0, 0.1) : ba.db2linear : si.smoo;
drive_db    = hslider("Drive [dB]", -13.0, -18.0, 18.0, 0.1);
drive_gain  = drive_db : si.smoo : ba.db2linear;

bias_level      = hslider("Bias Level", 0.62, 0.0, 1.0, 0.01) : si.smoo;
bias_scale      = hslider("Bias Scale", 11.0, 1.0, 100.0, 0.1) : si.smoo;
bias_resolution = nentry("Bias Resolution [style:menu{'K32':0;'K48':1;'K60':2;'K60_LUT':3}]", 3, 0, 3, 1);

mix = hslider("Mix [Dry->Wet]", 1.0, 0.0, 1.0, 0.01) : si.smoo;

// ===== Derived constants =====
Ms_safe    = ba.if(Ms > 1e-6, Ms, 1e-6);
alpha_norm = alpha_coupling;
a_norm     = a_density / Ms_safe;
k_norm     = k_pinning / Ms_safe;
c_norm     = c_reversibility;
bias_amp   = bias_level * bias_scale;

// ===== Precomputed bias lookup tables (init-time, not runtime) =====
// These replace runtime sin() calls with simple table lookups.
// Each table stores sin((i + 0.5) * dphi) for midpoint sampling.

// K32: 2 cycles = 4π total phase span, 36 substeps
tablesize_36 = 36;
dphi_36 = 4.0 * ma.PI / tablesize_36;
bias_gen_36(n) = sin((float(ba.period(n)) + 0.5) * dphi_36);
bias_lut_36(idx) = rdtable(tablesize_36, bias_gen_36(tablesize_36), int(idx));

// K48: 3 cycles = 6π total phase span, 54 substeps
tablesize_54 = 54;
dphi_54 = 6.0 * ma.PI / tablesize_54;
bias_gen_54(n) = sin((float(ba.period(n)) + 0.5) * dphi_54);
bias_lut_54(idx) = rdtable(tablesize_54, bias_gen_54(tablesize_54), int(idx));

// K60: 3 cycles = 6π total phase span, 66 substeps
tablesize_66 = 66;
dphi_66 = 6.0 * ma.PI / tablesize_66;
bias_gen_66(n) = sin((float(ba.period(n)) + 0.5) * dphi_66);
bias_lut_66(idx) = rdtable(tablesize_66, bias_gen_66(tablesize_66), int(idx));

sigma           = 1e-6;

// ===== Fast tanh approximation =====
fast_tanh(x) = t * (27.0 + x2) / (27.0 + 9.0 * x2)
with {
  t  = max(-3.0, min(3.0, x));
  x2 = t * t;
};

// ===== Precompute constants (matching C++ Normal quality) =====
// K32: 2 cycles × 18 points = 36 substeps
// K48: 3 cycles × 18 points = 54 substeps
// K60: 3 cycles × 22 points = 66 substeps
inv_36 = 1.0 / 36.0;
inv_54 = 1.0 / 54.0;
inv_66 = 1.0 / 66.0;
inv_a_norm = 1.0 / a_norm;

// ===== Core JA step driven by current bias sample =====
ja_substep(bias_offset) = ja_step
with {
  ja_step(M_prev, H_prev, H_audio, M_sum_prev) = M_sum_new, M_new, H_new, H_audio
  with {
    H_new = H_audio + bias_amp * bias_offset;
    dH    = H_new - H_prev;
    He    = H_new + alpha_norm * M_prev;

    x_man    = He * inv_a_norm;
    Man_e    = fast_tanh(x_man);
    Man_e2   = Man_e * Man_e;
    dMan_dH  = (1.0 - Man_e2) * inv_a_norm;

    dir      = ba.if(dH >= 0.0, 1.0, -1.0);
    pin      = dir * k_norm - alpha_norm * (Man_e - M_prev);
    inv_pin  = 1.0 / (pin + sigma);

    denom     = 1.0 - c_norm * alpha_norm * dMan_dH;
    inv_denom = 1.0 / (denom + 1e-9);
    dMdH      = (c_norm * dMan_dH + (Man_e - M_prev) * inv_pin) * inv_denom;
    dM_step   = dMdH * dH;

    M_unclamped = M_prev + dM_step;
    M_new       = max(-1.0, min(1.0, M_unclamped));
    M_sum_new   = M_sum_prev + M_new;
  };
};

// ===== Substeps with LUT lookup (replaces runtime sin() calls) =====
// Input:  (M_prev, H_prev, H_audio, M_sum_prev, idx)
// Output: (M_new,  H_new,  H_audio, M_sum_new,  idx+1)

ja_substep_lut36(M_prev, H_prev, H_audio, M_sum_prev, idx) =
  M_new, H_new, H_audio, M_sum_new, idx + 1
with {
  bias_offset = bias_lut_36(idx);
  ja_out      = (M_prev, H_prev, H_audio, M_sum_prev) : ja_substep(bias_offset);
  M_sum_new   = ba.selector(0, 4, ja_out);
  M_new       = ba.selector(1, 4, ja_out);
  H_new       = ba.selector(2, 4, ja_out);
};

ja_substep_lut54(M_prev, H_prev, H_audio, M_sum_prev, idx) =
  M_new, H_new, H_audio, M_sum_new, idx + 1
with {
  bias_offset = bias_lut_54(idx);
  ja_out      = (M_prev, H_prev, H_audio, M_sum_prev) : ja_substep(bias_offset);
  M_sum_new   = ba.selector(0, 4, ja_out);
  M_new       = ba.selector(1, 4, ja_out);
  H_new       = ba.selector(2, 4, ja_out);
};

ja_substep_lut66(M_prev, H_prev, H_audio, M_sum_prev, idx) =
  M_new, H_new, H_audio, M_sum_new, idx + 1
with {
  bias_offset = bias_lut_66(idx);
  ja_out      = (M_prev, H_prev, H_audio, M_sum_prev) : ja_substep(bias_offset);
  M_sum_new   = ba.selector(0, 4, ja_out);
  M_new       = ba.selector(1, 4, ja_out);
  H_new       = ba.selector(2, 4, ja_out);
};

// ===== Loop helpers with LUT (matching C++ Normal quality substep counts) =====
// Now using precomputed lookup tables instead of runtime sin() calls.
// Input:  (M_prev, H_prev, H_audio)
// Output: (M_new, H_new, M_sum)

// K32: 36 substeps (2 cycles × 18 points/cycle)
ja_loop36(M_prev, H_prev, H_audio) =
  M_prev, H_prev, H_audio, 0.0, 0 : seq(i, 36, ja_substep_lut36)
  <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(3, 5);

// K48: 54 substeps (3 cycles × 18 points/cycle)
ja_loop54(M_prev, H_prev, H_audio) =
  M_prev, H_prev, H_audio, 0.0, 0 : seq(i, 54, ja_substep_lut54)
  <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(3, 5);

// K60: 66 substeps (3 cycles × 22 points/cycle)
ja_loop66(M_prev, H_prev, H_audio) =
  M_prev, H_prev, H_audio, 0.0, 0 : seq(i, 66, ja_substep_lut66)
  <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(3, 5);

// ===== K60 LUT-optimized loop: 1 real substep + 2D LUT lookup =====
// This replaces 66 sequential substeps with:
//   1. Compute substep 0 with full physics → get M1
//   2. LUT lookup: (M1, H_audio) → (M_end, sumM_rest)
//   3. Mavg = (M1 + sumM_rest) / 66
//   4. State update: M_prev = M_end, H_prev = H_audio + biasAmp * last_bias

// Single substep 0 with full JA physics (for cross-sample dependency)
ja_substep0_full(M_prev, H_prev, H_audio) = M1, H1
with {
  bias0 = bias_lut_66(0);
  H1 = H_audio + bias_amp * bias0;
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

// K60 LUT-optimized version: 1 substep + 2D LUT lookup
// Output: (M_end, H_end, Mavg)
ja_loop66_lut(M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
  // Substep 0: full physics (cross-sample dependency)
  M1_H1 = ja_substep0_full(M_prev, H_prev, H_audio);
  M1 = ba.selector(0, 2, M1_H1);

  // LUT lookup for substeps 1..65
  M_end = ja_lookup_m_end(M1, H_audio);
  sumM_rest = ja_lookup_sum_m_rest(M1, H_audio);

  // Average magnetization over all 66 substeps
  Mavg = (M1 + sumM_rest) * inv_66;

  // H_end = H_audio + biasAmp * last_bias_value
  H_end = H_audio + bias_amp * bias_lut_66(65);
};

// ===== Streaming JA hysteresis =====
// Output order from ja_loopXX is (M_new, H_new, M_sum) or (M_end, H_end, Mavg) for LUT
// Feedback via ~ takes first 2 outputs: M_new -> recM, H_new -> recH
// Final output is M_sum/Mavg at index 2, normalized by substep count (for non-LUT modes)
ja_hysteresis(H_in) =
  ba.if(bias_resolution < 0.5,
    loopK32(H_in),
    ba.if(bias_resolution < 1.5,
      loopK48(H_in),
      ba.if(bias_resolution < 2.5,
        loopK60(H_in),
        loopK60_LUT(H_in))))
with {
  loopK32(H) = (loop ~ (mem, mem))
    : ba.selector(2, 3)
    : *(inv_36)
  with {
    loop(recM, recH) = recM, recH, H : ja_loop36;
  };

  loopK48(H) = (loop ~ (mem, mem))
    : ba.selector(2, 3)
    : *(inv_54)
  with {
    loop(recM, recH) = recM, recH, H : ja_loop54;
  };

  loopK60(H) = (loop ~ (mem, mem))
    : ba.selector(2, 3)
    : *(inv_66)
  with {
    loop(recM, recH) = recM, recH, H : ja_loop66;
  };

  // K60_LUT: 1 substep + 2D LUT lookup (Mavg already computed in loop)
  loopK60_LUT(H) = (loop ~ (mem, mem))
    : ba.selector(2, 3)
  with {
    loop(recM, recH) = recM, recH, H : ja_loop66_lut;
  };
};

// ===== Prototype tape stage (no limiter/clipper) =====
// DC blocker: 2nd-order SVF TPT highpass at 10 Hz, Butterworth Q
dc_blocker = fi.SVFTPT.HP2(10.0, 0.7071);

tape_stage(x) =
  x * input_gain
  : *(drive_gain)
  : ja_hysteresis
  : dc_blocker;

wet_gained = tape_stage : *(output_gain);

// Dry/wet mix for quick listening tests.
tape_channel = ef.dryWetMixer(mix, wet_gained);

process = tape_channel, tape_channel;
