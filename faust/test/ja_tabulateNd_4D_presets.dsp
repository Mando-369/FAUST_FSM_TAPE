// ja_tabulateNd_4D_presets.dsp - 4D LUT with machine presets via ondemand
//
// 4 Machine Presets (each with its own 4D LUT):
// - Studer A800:  warm, thick
// - Studer A810:  clean mastering
// - Ampex ATR-102: smooth, hi-fi
// - Otari MX5050: punchy, gritty
//
// 4D Grid per preset: M × H × Bias × Asym = 33 × 65 × 9 × 5 = 96,525 points
// Total: 4 presets × 96k × 2 LUTs = ~6 MB
// K90 substeps × 4 cascades = 360 total (matches 3D quality)
//
// Expected CPU: ~0.8-1% (only active preset computes via ondemand)
// Expected compile: ~30s

import("stdfaust.lib");

//==============================================================================
// Machine Preset Physics Constants
//==============================================================================
// | Machine      | Ms  | a   | k   | c    | α     |
// |--------------|-----|-----|-----|------|-------|
// | Studer A800  | 320 | 750 | 300 | 0.18 | 0.015 |
// | Studer A810  | 360 | 900 | 240 | 0.24 | 0.012 |
// | Ampex ATR102 | 380 | 950 | 220 | 0.26 | 0.011 |
// | Otari MX5050 | 300 | 700 | 320 | 0.20 | 0.017 |

// A800 - warm, thick
Ms_a800 = 320.0; a_a800 = 750.0; k_a800 = 300.0; c_a800 = 0.18; alpha_a800 = 0.015;

// A810 - clean mastering
Ms_a810 = 360.0; a_a810 = 900.0; k_a810 = 240.0; c_a810 = 0.24; alpha_a810 = 0.012;

// ATR-102 - smooth, hi-fi
Ms_atr = 380.0; a_atr = 950.0; k_atr = 220.0; c_atr = 0.26; alpha_atr = 0.011;

// MX5050 - punchy, gritty
Ms_mx = 300.0; a_mx = 700.0; k_mx = 320.0; c_mx = 0.20; alpha_mx = 0.017;

// Shared constants
bias_scale = 11.0;
sigma = 1e-3;
diff_scale = 2.0;  // for K90

two_pi = 2.0 * ma.PI;
substeps = 90;
substep_phase = two_pi / substeps;
inv_n = 1.0 / substeps;

//==============================================================================
// Parameterized JA Physics (compile-time evaluation for LUT building)
//==============================================================================

// Single JA substep with explicit physics params
ja_substep_p(Ms, a_density, k_pinning, c_rev, alpha_p, bias_offset, M_prev, H_prev, H_audio, bias_amp) = M_new, H_new
with {
  Ms_safe = max(Ms, 1e-6);
  alpha_norm = alpha_p;
  a_norm = a_density / Ms_safe;
  inv_a_norm = 1.0 / max(a_norm, 1e-9);
  k_norm = k_pinning / Ms_safe;
  c_norm = c_rev;

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

//==============================================================================
// K90 Sequence Generators (one per preset)
//==============================================================================

// Generic K90 seq with physics params
ja_k90_seq_p(Ms, a_d, k_p, c_r, alpha_p, M_prev, H_prev, H_audio, M_sum, phase, bias_amp, bias_asym) =
  M_new, H_new, H_audio, M_sum_new, phase_new, bias_amp, bias_asym
with {
  midpoint = ma.frac((phase + substep_phase * 0.5) / two_pi) * two_pi;
  bias_offset = sin(midpoint) + bias_asym * sin(2.0 * midpoint);
  step = ja_substep_p(Ms, a_d, k_p, c_r, alpha_p, bias_offset, M_prev, H_prev, H_audio, bias_amp);
  M_new = ba.selector(0, 2, step);
  H_new = ba.selector(1, 2, step);
  M_sum_new = M_sum + M_new;
  phase_new = ma.frac((phase + substep_phase) / two_pi) * two_pi;
};

// A800 K90 M_end
ja_k90_m_end_a800(M_prev, H_audio, bias_level, bias_asym) = M_end
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym
    : seq(i, 90, ja_k90_seq_p(Ms_a800, a_a800, k_a800, c_a800, alpha_a800));
  M_end = ba.selector(0, 7, result);
};

ja_k90_sum_rest_a800(M_prev, H_audio, bias_level, bias_asym) = sum_rest
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym
    : seq(i, 90, ja_k90_seq_p(Ms_a800, a_a800, k_a800, c_a800, alpha_a800));
  M_sum = ba.selector(3, 7, result);
  M_end = ba.selector(0, 7, result);
  sum_rest = M_sum - M_end;
};

// A810 K90 M_end
ja_k90_m_end_a810(M_prev, H_audio, bias_level, bias_asym) = M_end
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym
    : seq(i, 90, ja_k90_seq_p(Ms_a810, a_a810, k_a810, c_a810, alpha_a810));
  M_end = ba.selector(0, 7, result);
};

ja_k90_sum_rest_a810(M_prev, H_audio, bias_level, bias_asym) = sum_rest
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym
    : seq(i, 90, ja_k90_seq_p(Ms_a810, a_a810, k_a810, c_a810, alpha_a810));
  M_sum = ba.selector(3, 7, result);
  M_end = ba.selector(0, 7, result);
  sum_rest = M_sum - M_end;
};

// ATR-102 K90 M_end
ja_k90_m_end_atr(M_prev, H_audio, bias_level, bias_asym) = M_end
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym
    : seq(i, 90, ja_k90_seq_p(Ms_atr, a_atr, k_atr, c_atr, alpha_atr));
  M_end = ba.selector(0, 7, result);
};

ja_k90_sum_rest_atr(M_prev, H_audio, bias_level, bias_asym) = sum_rest
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym
    : seq(i, 90, ja_k90_seq_p(Ms_atr, a_atr, k_atr, c_atr, alpha_atr));
  M_sum = ba.selector(3, 7, result);
  M_end = ba.selector(0, 7, result);
  sum_rest = M_sum - M_end;
};

// MX5050 K90 M_end
ja_k90_m_end_mx(M_prev, H_audio, bias_level, bias_asym) = M_end
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym
    : seq(i, 90, ja_k90_seq_p(Ms_mx, a_mx, k_mx, c_mx, alpha_mx));
  M_end = ba.selector(0, 7, result);
};

ja_k90_sum_rest_mx(M_prev, H_audio, bias_level, bias_asym) = sum_rest
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp, bias_asym
    : seq(i, 90, ja_k90_seq_p(Ms_mx, a_mx, k_mx, c_mx, alpha_mx));
  M_sum = ba.selector(3, 7, result);
  M_end = ba.selector(0, 7, result);
  sum_rest = M_sum - M_end;
};

//==============================================================================
// 4D Tabulated Lookups (one pair per preset)
//==============================================================================

// Grid sizes
M_SIZE = 33;
H_SIZE = 65;
B_SIZE = 9;
A_SIZE = 5;

// Ranges
M_MIN = -1.0; M_MAX = 1.0;
H_MIN = -40.0; H_MAX = 40.0;
B_MIN = 0.1; B_MAX = 0.9;
A_MIN = 0.0; A_MAX = 0.4;

// A800 LUTs
lut_m_end_a800(M, H, bias, asym) = ba.tabulateNd(1, ja_k90_m_end_a800,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

lut_sum_rest_a800(M, H, bias, asym) = ba.tabulateNd(1, ja_k90_sum_rest_a800,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

// A810 LUTs
lut_m_end_a810(M, H, bias, asym) = ba.tabulateNd(1, ja_k90_m_end_a810,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

lut_sum_rest_a810(M, H, bias, asym) = ba.tabulateNd(1, ja_k90_sum_rest_a810,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

// ATR-102 LUTs
lut_m_end_atr(M, H, bias, asym) = ba.tabulateNd(1, ja_k90_m_end_atr,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

lut_sum_rest_atr(M, H, bias, asym) = ba.tabulateNd(1, ja_k90_sum_rest_atr,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

// MX5050 LUTs
lut_m_end_mx(M, H, bias, asym) = ba.tabulateNd(1, ja_k90_m_end_mx,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

lut_sum_rest_mx(M, H, bias, asym) = ba.tabulateNd(1, ja_k90_sum_rest_mx,
  (M_SIZE, H_SIZE, B_SIZE, A_SIZE,
   M_MIN, H_MIN, B_MIN, A_MIN,
   M_MAX, H_MAX, B_MAX, A_MAX,
   M, H, bias, asym)).cub;

//==============================================================================
// 4x Cascaded Hysteresis per Preset
//==============================================================================

cascade_a800(H_audio, bias, asym) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg with {
    M1 = lut_m_end_a800(M_prev, H_audio, bias, asym);
    s1 = lut_sum_rest_a800(M_prev, H_audio, bias, asym);
    M2 = lut_m_end_a800(M1, H_audio, bias, asym);
    s2 = lut_sum_rest_a800(M1, H_audio, bias, asym);
    M3 = lut_m_end_a800(M2, H_audio, bias, asym);
    s3 = lut_sum_rest_a800(M2, H_audio, bias, asym);
    M_end = lut_m_end_a800(M3, H_audio, bias, asym);
    s4 = lut_sum_rest_a800(M3, H_audio, bias, asym);
    M_avg = (s1 + s2 + s3 + s4) * inv_n;
  };
};

cascade_a810(H_audio, bias, asym) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg with {
    M1 = lut_m_end_a810(M_prev, H_audio, bias, asym);
    s1 = lut_sum_rest_a810(M_prev, H_audio, bias, asym);
    M2 = lut_m_end_a810(M1, H_audio, bias, asym);
    s2 = lut_sum_rest_a810(M1, H_audio, bias, asym);
    M3 = lut_m_end_a810(M2, H_audio, bias, asym);
    s3 = lut_sum_rest_a810(M2, H_audio, bias, asym);
    M_end = lut_m_end_a810(M3, H_audio, bias, asym);
    s4 = lut_sum_rest_a810(M3, H_audio, bias, asym);
    M_avg = (s1 + s2 + s3 + s4) * inv_n;
  };
};

cascade_atr(H_audio, bias, asym) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg with {
    M1 = lut_m_end_atr(M_prev, H_audio, bias, asym);
    s1 = lut_sum_rest_atr(M_prev, H_audio, bias, asym);
    M2 = lut_m_end_atr(M1, H_audio, bias, asym);
    s2 = lut_sum_rest_atr(M1, H_audio, bias, asym);
    M3 = lut_m_end_atr(M2, H_audio, bias, asym);
    s3 = lut_sum_rest_atr(M2, H_audio, bias, asym);
    M_end = lut_m_end_atr(M3, H_audio, bias, asym);
    s4 = lut_sum_rest_atr(M3, H_audio, bias, asym);
    M_avg = (s1 + s2 + s3 + s4) * inv_n;
  };
};

cascade_mx(H_audio, bias, asym) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg with {
    M1 = lut_m_end_mx(M_prev, H_audio, bias, asym);
    s1 = lut_sum_rest_mx(M_prev, H_audio, bias, asym);
    M2 = lut_m_end_mx(M1, H_audio, bias, asym);
    s2 = lut_sum_rest_mx(M1, H_audio, bias, asym);
    M3 = lut_m_end_mx(M2, H_audio, bias, asym);
    s3 = lut_sum_rest_mx(M2, H_audio, bias, asym);
    M_end = lut_m_end_mx(M3, H_audio, bias, asym);
    s4 = lut_sum_rest_mx(M3, H_audio, bias, asym);
    M_avg = (s1 + s2 + s3 + s4) * inv_n;
  };
};

//==============================================================================
// Preset Selection via Ondemand
//==============================================================================

ja_hysteresis_presets(H_audio, bias, asym, preset) = output
with {
  // Clock signals for ondemand (only one is 1 at a time)
  clk(i) = (int(preset + 0.5) == i);

  // Each preset only computes when its clock is active
  p0 = clk(0) : ondemand(cascade_a800(H_audio, bias, asym));
  p1 = clk(1) : ondemand(cascade_a810(H_audio, bias, asym));
  p2 = clk(2) : ondemand(cascade_atr(H_audio, bias, asym));
  p3 = clk(3) : ondemand(cascade_mx(H_audio, bias, asym));

  // Sum (only one is non-zero)
  output = clk(0) * p0 + clk(1) * p1 + clk(2) * p2 + clk(3) * p3;
};

//==============================================================================
// Channel Processing
//==============================================================================
fsm_channel_presets(preset, input_gain_db, output_gain_db, drive_db, mix_val, bias_level, bias_asym, lambda_tilt) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  input_gain = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain = ba.db2linear(drive_db) : si.smoo;
  drive_comp = (1.0 / drive_gain) * ba.db2linear(15.6);

  // Bias compensation
  bias_comp_db = ba.if(bias_level < 0.5,
    (bias_level - 0.5) * 20.0,
    (bias_level - 0.5) * 25.0);
  bias_comp = ba.db2linear(bias_comp_db);

  // Asym compensation
  asym_comp_db = -4.5 * bias_asym;
  asym_comp = ba.db2linear(asym_comp_db);

  // DC blocker
  dc_blocker = fi.SVFTPT.HP2(7.0, 0.74);

  // Pre-JA attenuation
  pre_ja_atten = ba.db2linear(-12.8);

  // Lambda tilt
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  // Processing
  process_hyst(x) = ja_hysteresis_presets(x, bias_level, bias_asym, preset);
  wet_gained = _ * input_gain : *(drive_gain) : *(pre_ja_atten)
    : lambda_sat : process_hyst
    : dc_blocker : *(drive_comp) : *(bias_comp) : *(asym_comp) : *(output_gain);
};

//==============================================================================
// UI
//==============================================================================
fsm_channel_presets_ui =
  fsm_channel_presets(preset, input_gain_db, output_gain_db, drive_db, mix, bias_level, bias_asym, lambda_tilt)
with {
  // Preset selector: 0=A800, 1=A810, 2=ATR-102, 3=MX5050
  preset = hgroup("JA 4D Presets", hgroup("[00] MACHINE",
    nentry("[0]Preset [style:menu{'A800 Warm':0;'A810 Clean':1;'ATR-102 HiFi':2;'MX5050 Gritty':3}]", 0, 0, 3, 1)));

  bias_level = hgroup("JA 4D Presets", hgroup("[01] BIAS",
    vslider("[0]Level [style:knob]", 0.4, 0.1, 0.9, 0.01)));
  bias_asym = hgroup("JA 4D Presets", hgroup("[01] BIAS",
    vslider("[1]Asym [style:knob]", 0.08, 0.0, 0.4, 0.01)));

  input_gain_db = hgroup("JA 4D Presets", hgroup("[02] GAIN",
    vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA 4D Presets", hgroup("[02] GAIN",
    vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db = hgroup("JA 4D Presets", hgroup("[02] GAIN",
    vslider("[2]Drive [dB]", 0.0, -30.0, 30.0, 0.1)));
  mix = hgroup("JA 4D Presets", hgroup("[02] GAIN",
    vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));

  lambda_tilt = hgroup("JA 4D Presets", hgroup("[03] TAPE",
    vslider("[0]Lambda Tilt", 0.0, -0.1, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_presets_ui);
