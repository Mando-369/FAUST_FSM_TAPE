// ja_tabulateNd.dsp - 3D LUT using ba.tabulateNd
// Runtime bias interpolation without precomputed waveforms
//
// Uses FAUST's built-in tabulateNd for 3D interpolation:
// - M axis: 33 points, [-1, 1]
// - H axis: 65 points, [-40, 40]
// - Bias axis: 9 points, [0.1, 0.9]
// - 4x cascaded lookups (K45 = 45 substeps per lookup, 180 total)

import("stdfaust.lib");

//==============================================================================
// JA Physics Core - K45 substeps for one table entry
//==============================================================================
// This function is evaluated at compile time to build the table

// Physics constants
Ms = 320.0;
a_density = 720.0;
k_pinning = 280.0;
c_rev = 0.18;
alpha = 0.015;
bias_scale = 11.0;

// Derived constants
Ms_safe = max(Ms, 1e-6);
alpha_norm = alpha;
a_norm = a_density / Ms_safe;
inv_a_norm = 1.0 / max(a_norm, 1e-9);
k_norm = k_pinning / Ms_safe;
c_norm = c_rev;
sigma = 1e-3;
diff_scale = 2.53;

two_pi = 2.0 * ma.PI;
substeps = 45;
substep_phase = two_pi / substeps;
inv_n = 1.0 / substeps;

// Single JA substep
ja_substep(bias_offset, M_prev, H_prev, H_audio, bias_amp) = M_new, H_new
with {
  H_new = H_audio + bias_amp * bias_offset;
  dH = H_new - H_prev;
  He = H_new + alpha_norm * M_prev;

  x_man = He * inv_a_norm;
  Man_e = ma.tanh(x_man);
  dMan_dH = (1.0 - Man_e * Man_e) * inv_a_norm;

  diff = Man_e - M_prev;
  diff_clamped = diff / (1.0 + abs(diff) * diff_scale);

  dir = ba.if(dH >= 0.0, 1.0, -1.0);
  pin = dir * k_norm - alpha_norm * diff_clamped;
  inv_pin = 1.0 / (pin + sigma);

  denom = 1.0 - c_norm * alpha_norm * dMan_dH;
  inv_denom = 1.0 / (denom + 1e-9);
  dMdH = (c_norm * dMan_dH + diff_clamped * inv_pin) * inv_denom;
  dM = dMdH * dH;

  M_unclamped = M_prev + dM;
  M_new = max(-1.0, min(1.0, M_unclamped));
};

// K45 substep sequence - returns M_end
ja_k45_seq(M_prev, H_prev, H_audio, M_sum, phase, bias_amp) =
  M_new, H_new, H_audio, M_sum_new, phase_new, bias_amp
with {
  midpoint = ma.frac((phase + substep_phase * 0.5) / two_pi) * two_pi;
  bias_offset = sin(midpoint);
  step = ja_substep(bias_offset, M_prev, H_prev, H_audio, bias_amp);
  M_new = ba.selector(0, 2, step);
  H_new = ba.selector(1, 2, step);
  M_sum_new = M_sum + M_new;
  phase_new = ma.frac((phase + substep_phase) / two_pi) * two_pi;
};

// Run K45 substeps and return M_end
ja_k45_m_end(M_prev, H_audio, bias_level) = M_end
with {
  bias_amp = bias_level * bias_scale;
  // seq returns: M, H, H_audio, M_sum, phase, bias_amp
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp : seq(i, 45, ja_k45_seq);
  M_end = ba.selector(0, 6, result);
};

// Run K45 substeps and return sum(M_rest) for averaging
ja_k45_sum_rest(M_prev, H_audio, bias_level) = sum_rest
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp : seq(i, 45, ja_k45_seq);
  M_sum = ba.selector(3, 6, result);
  M_end = ba.selector(0, 6, result);
  sum_rest = M_sum - M_end;  // sum of M[1..44], excludes M_end
};

//==============================================================================
// 3D Tabulated Lookups using ba.tabulateNd
//==============================================================================

// Grid sizes
M_SIZE = 33;
H_SIZE = 65;
B_SIZE = 9;

// Ranges
M_MIN = -1.0;
M_MAX = 1.0;
H_MIN = -40.0;
H_MAX = 40.0;
B_MIN = 0.1;
B_MAX = 0.9;

// Tabulated M_end lookup with tricubic interpolation
lut_m_end(M, H, bias) = ba.tabulateNd(1, ja_k45_m_end,
  (M_SIZE, H_SIZE, B_SIZE,
   M_MIN, H_MIN, B_MIN,
   M_MAX, H_MAX, B_MAX,
   M, H, bias)).cub;

// Tabulated sum_rest lookup with tricubic interpolation
lut_sum_rest(M, H, bias) = ba.tabulateNd(1, ja_k45_sum_rest,
  (M_SIZE, H_SIZE, B_SIZE,
   M_MIN, H_MIN, B_MIN,
   M_MAX, H_MAX, B_MAX,
   M, H, bias)).cub;

//==============================================================================
// 4x Cascaded Hysteresis (equivalent to 4×K45 = 180 substeps)
//==============================================================================
ja_hysteresis(H_audio, bias) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg
  with {
    // 4 cascaded lookups for better transient response
    M1 = lut_m_end(M_prev, H_audio, bias);
    s1 = lut_sum_rest(M_prev, H_audio, bias);

    M2 = lut_m_end(M1, H_audio, bias);
    s2 = lut_sum_rest(M1, H_audio, bias);

    M3 = lut_m_end(M2, H_audio, bias);
    s3 = lut_sum_rest(M2, H_audio, bias);

    M_end = lut_m_end(M3, H_audio, bias);
    s4 = lut_sum_rest(M3, H_audio, bias);

    // Average across all substeps
    M_avg = (s1 + s2 + s3 + s4) * inv_n;
  };
};

//==============================================================================
// Channel Processing
//==============================================================================
fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val, bias_level, lambda_tilt) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  input_gain = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain = ba.db2linear(drive_db) : si.smoo;
  drive_comp = (1.0 / drive_gain) * ba.db2linear(15.6);

  // Bias compensation (piecewise linear)
  bias_comp_db = ba.if(bias_level < 0.5,
    (bias_level - 0.5) * 20.0,
    (bias_level - 0.5) * 25.0);
  bias_comp = ba.db2linear(bias_comp_db);

  // DC blocker
  dc_blocker = fi.SVFTPT.HP2(7.0, 0.74);

  // Pre-JA attenuation to match H range
  pre_ja_atten = ba.db2linear(-12.8);

  // Lambda (wavelength) saturation - spectral tilt
  // Band: 200 Hz to 15000 Hz, Order 3
  // Tilt: 0 = flat, +0.1 = HF boost, -0.1 = HF cut
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  // Processing chain
  process_hyst(x) = ja_hysteresis(x, bias_level);
  wet_gained = _ * input_gain : *(drive_gain) : *(pre_ja_atten)
    : lambda_sat : process_hyst
    : dc_blocker : *(drive_comp) : *(bias_comp) : *(output_gain);
};

//==============================================================================
// UI
//==============================================================================
fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix, bias_level, lambda_tilt)
with {
  bias_level = hgroup("JA TabulateNd", hgroup("[00] BIAS",
    vslider("[0]Level [style:knob]", 0.4, 0.1, 0.9, 0.01)));
  input_gain_db = hgroup("JA TabulateNd", hgroup("[01] GAIN",
    vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA TabulateNd", hgroup("[01] GAIN",
    vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db = hgroup("JA TabulateNd", hgroup("[01] GAIN",
    vslider("[2]Drive [dB]", 0.0, -30.0, 30.0, 0.1)));
  mix = hgroup("JA TabulateNd", hgroup("[01] GAIN",
    vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));
  // Lambda tilt: 0 = flat, +0.1 = HF boost (bright), -0.1 = HF cut (warm)
  lambda_tilt = hgroup("JA TabulateNd", hgroup("[02] TAPE",
    vslider("[0]Lambda Tilt", 0.0, -0.1, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_ui);
