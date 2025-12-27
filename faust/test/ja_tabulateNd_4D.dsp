// ja_tabulateNd_4D.dsp - 4D LUT using ba.tabulateNd
// Adds bias asymmetry as 4th dimension for runtime 2nd harmonic control
//
// Dimensions:
// - M axis: 33 points, [-1, 1]
// - H axis: 65 points, [-40, 40]
// - Bias axis: 9 points, [0.1, 0.9]
// - Asym axis: 5 points, [0.0, 0.4]
// - Total: 33 × 65 × 9 × 5 = 96,525 points
// - 4x cascaded lookups (K45 = 45 substeps per lookup, 180 total)
//
// Expected CPU: ~0.5-1% (vs 0.2% for 3D, 3.4% for full physics K180)
// Tricubic 4D = 256 lookups per cascade × 4 = 1024/sample

import("stdfaust.lib");

//==============================================================================
// Tape Machine Presets (from FSM analysis)
//==============================================================================
// | Machine      | Ms  | a   | k   | c    | α     | biasLvl | biasAsym |
// |--------------|-----|-----|-----|------|-------|---------|----------|
// | Studer A800  | 320 | 750 | 300 | 0.18 | 0.015 | 0.42    | 0.08     |
// | Studer A810  | 360 | 900 | 240 | 0.24 | 0.012 | 0.50    | 0.05     |
// | Ampex ATR102 | 380 | 950 | 220 | 0.26 | 0.011 | 0.52    | 0.04     |
// | Otari MX5050 | 300 | 700 | 320 | 0.20 | 0.017 | 0.38    | 0.12     |

//==============================================================================
// Tape Formulation Presets (from FSM analysis)
//==============================================================================
// | Tape Type      | Ms  | a    | k   | c    | α     | biasLvl | biasAsym |
// |----------------|-----|------|-----|------|-------|---------|----------|
// | AMPEX 456      | 290 | 750  | 300 | 0.17 | 0.015 | 0.40    | 0.08     |
// | AMPEX 499      | 375 | 900  | 240 | 0.27 | 0.012 | 0.50    | 0.05     |
// | Quantegy GP9   | 450 | 1150 | 200 | 0.33 | 0.009 | 0.60    | 0.04     |
// | BASF 900       | 395 | 1050 | 220 | 0.29 | 0.011 | 0.55    | 0.06     |
// | Sony Metal     | 525 | 750  | 375 | 0.15 | 0.023 | 0.35    | 0.13     |
// | Cassette Ferric| 220 | 500  | 330 | 0.19 | 0.020 | 0.30    | 0.20     |

//==============================================================================
// JA Physics Core - K45 substeps for one table entry (compile-time)
//==============================================================================

// Default physics constants (Studer A800-ish)
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
diff_scale = 2.5;  // slightly higher for K45

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

// K45 substep sequence with asymmetric bias waveform
// bias_offset = sin(phase) + asym * sin(2*phase)
ja_k45_seq_4d(M_prev, H_prev, H_audio, M_sum, phase, bias_amp, bias_asym) =
  M_new, H_new, H_audio, M_sum_new, phase_new, bias_amp, bias_asym
with {
  midpoint = ma.frac((phase + substep_phase * 0.5) / two_pi) * two_pi;
  // Asymmetric bias: fundamental + 2nd harmonic
  bias_offset = sin(midpoint) + bias_asym * sin(2.0 * midpoint);
  step = ja_substep(bias_offset, M_prev, H_prev, H_audio, bias_amp);
  M_new = ba.selector(0, 2, step);
  H_new = ba.selector(1, 2, step);
  M_sum_new = M_sum + M_new;
  phase_new = ma.frac((phase + substep_phase) / two_pi) * two_pi;
};

// Run K45 substeps and return M_end (for 4D table)
ja_k45_m_end_4d(M_prev, H_audio, bias_level, bias_asym) = M_end
with {
  bias_amp = bias_level * bias_scale;
  // seq returns: M, H, H_audio, M_sum, phase, bias_amp, bias_asym
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym : seq(i, 45, ja_k45_seq_4d);
  M_end = ba.selector(0, 7, result);
};

// Run K45 substeps and return sum(M_rest) for averaging
ja_k45_sum_rest_4d(M_prev, H_audio, bias_level, bias_asym) = sum_rest
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym : seq(i, 45, ja_k45_seq_4d);
  M_sum = ba.selector(3, 7, result);
  M_end = ba.selector(0, 7, result);
  sum_rest = M_sum - M_end;  // sum of M[1..44], excludes M_end
};

//==============================================================================
// 4D Tabulated Lookups using ba.tabulateNd
//==============================================================================

// Grid sizes
M_SIZE = 33;
H_SIZE = 65;
B_SIZE = 9;
A_SIZE = 5;

// Ranges
M_MIN = -1.0;
M_MAX = 1.0;
H_MIN = -40.0;
H_MAX = 40.0;
B_MIN = 0.1;
B_MAX = 0.9;
A_MIN = 0.0;
A_MAX = 0.4;

// Tabulated M_end lookup with tricubic interpolation (4D = 256 points per lookup)
lut_m_end_4d(M, H, bias, asym) = ba.tabulateNd(1, ja_k45_m_end_4d,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

// Tabulated sum_rest lookup with tricubic interpolation
lut_sum_rest_4d(M, H, bias, asym) = ba.tabulateNd(1, ja_k45_sum_rest_4d,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

//==============================================================================
// 4x Cascaded Hysteresis (equivalent to 4×K45 = 180 substeps)
//==============================================================================
ja_hysteresis_4d(H_audio, bias, asym) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg
  with {
    // 4 cascaded lookups for better transient response
    M1 = lut_m_end_4d(M_prev, H_audio, bias, asym);
    s1 = lut_sum_rest_4d(M_prev, H_audio, bias, asym);

    M2 = lut_m_end_4d(M1, H_audio, bias, asym);
    s2 = lut_sum_rest_4d(M1, H_audio, bias, asym);

    M3 = lut_m_end_4d(M2, H_audio, bias, asym);
    s3 = lut_sum_rest_4d(M2, H_audio, bias, asym);

    M_end = lut_m_end_4d(M3, H_audio, bias, asym);
    s4 = lut_sum_rest_4d(M3, H_audio, bias, asym);

    // Average across all substeps
    M_avg = (s1 + s2 + s3 + s4) * inv_n;
  };
};

//==============================================================================
// Channel Processing
//==============================================================================
fsm_channel_4d(input_gain_db, output_gain_db, drive_db, mix_val, bias_level, bias_asym, lambda_tilt) =
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

  // Asym compensation (adds ~1.8dB at asym=0.4)
  asym_comp_db = -4.5 * bias_asym;
  asym_comp = ba.db2linear(asym_comp_db);

  // DC blocker
  dc_blocker = fi.SVFTPT.HP2(7.0, 0.74);

  // Pre-JA attenuation to match H range
  pre_ja_atten = ba.db2linear(-12.8);

  // Lambda (wavelength) saturation - spectral tilt
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  // Processing chain
  process_hyst(x) = ja_hysteresis_4d(x, bias_level, bias_asym);
  wet_gained = _ * input_gain : *(drive_gain) : *(pre_ja_atten)
    : lambda_sat : process_hyst
    : dc_blocker : *(drive_comp) : *(bias_comp) : *(asym_comp) : *(output_gain);
};

//==============================================================================
// UI
//==============================================================================
fsm_channel_4d_ui =
  fsm_channel_4d(input_gain_db, output_gain_db, drive_db, mix, bias_level, bias_asym, lambda_tilt)
with {
  bias_level = hgroup("JA 4D TabulateNd", hgroup("[00] BIAS",
    vslider("[0]Level [style:knob]", 0.4, 0.1, 0.9, 0.01)));
  bias_asym = hgroup("JA 4D TabulateNd", hgroup("[00] BIAS",
    vslider("[1]Asym [style:knob]", 0.08, 0.0, 0.4, 0.01)));
  input_gain_db = hgroup("JA 4D TabulateNd", hgroup("[01] GAIN",
    vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA 4D TabulateNd", hgroup("[01] GAIN",
    vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db = hgroup("JA 4D TabulateNd", hgroup("[01] GAIN",
    vslider("[2]Drive [dB]", 0.0, -30.0, 30.0, 0.1)));
  mix = hgroup("JA 4D TabulateNd", hgroup("[01] GAIN",
    vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));
  lambda_tilt = hgroup("JA 4D TabulateNd", hgroup("[02] TAPE",
    vslider("[0]Lambda Tilt", 0.0, -0.1, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_4d_ui);
