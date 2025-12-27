// ja_tabulateNd_3D_quality_modes.dsp - Test ondemand with 3D tabulateNd
// Tests if ondemand bug is specific to 4D or affects all tabulateNd
//
// 3 quality modes using ondemand:
// - Mode 0: K23 Eco (~92 total substeps)
// - Mode 1: K45 Standard (~180 total substeps)
// - Mode 2: K90 HQ (~360 total substeps)
//
// Expected: CPU should change based on mode selection
// Bug: CPU stays constant (all modes compute)

import("stdfaust.lib");

//==============================================================================
// JA Physics Core - Shared constants
//==============================================================================

Ms = 320.0;
a_density = 720.0;
k_pinning = 280.0;
c_rev = 0.18;
alpha = 0.015;
bias_scale = 11.0;

Ms_safe = max(Ms, 1e-6);
alpha_norm = alpha;
a_norm = a_density / Ms_safe;
inv_a_norm = 1.0 / max(a_norm, 1e-9);
k_norm = k_pinning / Ms_safe;
c_norm = c_rev;
sigma = 1e-3;

two_pi = 2.0 * ma.PI;

//==============================================================================
// Single JA substep (shared by all K values)
//==============================================================================
ja_substep(diff_scale, bias_offset, M_prev, H_prev, H_audio, bias_amp) = M_new, H_new
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

//==============================================================================
// K23 Eco Mode
//==============================================================================
substeps_k23 = 23;
substep_phase_k23 = two_pi / substeps_k23;
inv_n_k23 = 1.0 / substeps_k23;
diff_scale_k23 = 3.5;

ja_k23_seq(M_prev, H_prev, H_audio, M_sum, phase, bias_amp) =
  M_new, H_new, H_audio, M_sum_new, phase_new, bias_amp
with {
  midpoint = ma.frac((phase + substep_phase_k23 * 0.5) / two_pi) * two_pi;
  bias_offset = sin(midpoint);
  step = ja_substep(diff_scale_k23, bias_offset, M_prev, H_prev, H_audio, bias_amp);
  M_new = ba.selector(0, 2, step);
  H_new = ba.selector(1, 2, step);
  M_sum_new = M_sum + M_new;
  phase_new = ma.frac((phase + substep_phase_k23) / two_pi) * two_pi;
};

ja_k23_m_end(M_prev, H_audio, bias_level) = M_end
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp : seq(i, 23, ja_k23_seq);
  M_end = ba.selector(0, 6, result);
};

ja_k23_sum_rest(M_prev, H_audio, bias_level) = sum_rest
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp : seq(i, 23, ja_k23_seq);
  M_sum = ba.selector(3, 6, result);
  M_end = ba.selector(0, 6, result);
  sum_rest = M_sum - M_end;
};

//==============================================================================
// K45 Standard Mode
//==============================================================================
substeps_k45 = 45;
substep_phase_k45 = two_pi / substeps_k45;
inv_n_k45 = 1.0 / substeps_k45;
diff_scale_k45 = 2.5;

ja_k45_seq(M_prev, H_prev, H_audio, M_sum, phase, bias_amp) =
  M_new, H_new, H_audio, M_sum_new, phase_new, bias_amp
with {
  midpoint = ma.frac((phase + substep_phase_k45 * 0.5) / two_pi) * two_pi;
  bias_offset = sin(midpoint);
  step = ja_substep(diff_scale_k45, bias_offset, M_prev, H_prev, H_audio, bias_amp);
  M_new = ba.selector(0, 2, step);
  H_new = ba.selector(1, 2, step);
  M_sum_new = M_sum + M_new;
  phase_new = ma.frac((phase + substep_phase_k45) / two_pi) * two_pi;
};

ja_k45_m_end(M_prev, H_audio, bias_level) = M_end
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp : seq(i, 45, ja_k45_seq);
  M_end = ba.selector(0, 6, result);
};

ja_k45_sum_rest(M_prev, H_audio, bias_level) = sum_rest
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp : seq(i, 45, ja_k45_seq);
  M_sum = ba.selector(3, 6, result);
  M_end = ba.selector(0, 6, result);
  sum_rest = M_sum - M_end;
};

//==============================================================================
// K90 HQ Mode
//==============================================================================
substeps_k90 = 90;
substep_phase_k90 = two_pi / substeps_k90;
inv_n_k90 = 1.0 / substeps_k90;
diff_scale_k90 = 2.0;

ja_k90_seq(M_prev, H_prev, H_audio, M_sum, phase, bias_amp) =
  M_new, H_new, H_audio, M_sum_new, phase_new, bias_amp
with {
  midpoint = ma.frac((phase + substep_phase_k90 * 0.5) / two_pi) * two_pi;
  bias_offset = sin(midpoint);
  step = ja_substep(diff_scale_k90, bias_offset, M_prev, H_prev, H_audio, bias_amp);
  M_new = ba.selector(0, 2, step);
  H_new = ba.selector(1, 2, step);
  M_sum_new = M_sum + M_new;
  phase_new = ma.frac((phase + substep_phase_k90) / two_pi) * two_pi;
};

ja_k90_m_end(M_prev, H_audio, bias_level) = M_end
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp : seq(i, 90, ja_k90_seq);
  M_end = ba.selector(0, 6, result);
};

ja_k90_sum_rest(M_prev, H_audio, bias_level) = sum_rest
with {
  bias_amp = bias_level * bias_scale;
  result = M_prev, 0.0, H_audio, 0.0, 0.0, bias_amp : seq(i, 90, ja_k90_seq);
  M_sum = ba.selector(3, 6, result);
  M_end = ba.selector(0, 6, result);
  sum_rest = M_sum - M_end;
};

//==============================================================================
// 3D Tabulated Lookups - One per quality mode
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

// K23 LUTs
lut_m_end_k23(M, H, bias) = ba.tabulateNd(1, ja_k23_m_end,
  (M_SIZE, H_SIZE, B_SIZE, M_MIN, H_MIN, B_MIN, M_MAX, H_MAX, B_MAX, M, H, bias)).cub;

lut_sum_rest_k23(M, H, bias) = ba.tabulateNd(1, ja_k23_sum_rest,
  (M_SIZE, H_SIZE, B_SIZE, M_MIN, H_MIN, B_MIN, M_MAX, H_MAX, B_MAX, M, H, bias)).cub;

// K45 LUTs
lut_m_end_k45(M, H, bias) = ba.tabulateNd(1, ja_k45_m_end,
  (M_SIZE, H_SIZE, B_SIZE, M_MIN, H_MIN, B_MIN, M_MAX, H_MAX, B_MAX, M, H, bias)).cub;

lut_sum_rest_k45(M, H, bias) = ba.tabulateNd(1, ja_k45_sum_rest,
  (M_SIZE, H_SIZE, B_SIZE, M_MIN, H_MIN, B_MIN, M_MAX, H_MAX, B_MAX, M, H, bias)).cub;

// K90 LUTs
lut_m_end_k90(M, H, bias) = ba.tabulateNd(1, ja_k90_m_end,
  (M_SIZE, H_SIZE, B_SIZE, M_MIN, H_MIN, B_MIN, M_MAX, H_MAX, B_MAX, M, H, bias)).cub;

lut_sum_rest_k90(M, H, bias) = ba.tabulateNd(1, ja_k90_sum_rest,
  (M_SIZE, H_SIZE, B_SIZE, M_MIN, H_MIN, B_MIN, M_MAX, H_MAX, B_MAX, M, H, bias)).cub;

//==============================================================================
// 4x Cascaded Hysteresis per mode
//==============================================================================

// K23 cascade (4×23 = 92 substeps)
cascade_k23(H_audio, bias) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg with {
    M1 = lut_m_end_k23(M_prev, H_audio, bias);
    s1 = lut_sum_rest_k23(M_prev, H_audio, bias);
    M2 = lut_m_end_k23(M1, H_audio, bias);
    s2 = lut_sum_rest_k23(M1, H_audio, bias);
    M3 = lut_m_end_k23(M2, H_audio, bias);
    s3 = lut_sum_rest_k23(M2, H_audio, bias);
    M_end = lut_m_end_k23(M3, H_audio, bias);
    s4 = lut_sum_rest_k23(M3, H_audio, bias);
    M_avg = (s1 + s2 + s3 + s4) * inv_n_k23;
  };
};

// K45 cascade (4×45 = 180 substeps)
cascade_k45(H_audio, bias) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg with {
    M1 = lut_m_end_k45(M_prev, H_audio, bias);
    s1 = lut_sum_rest_k45(M_prev, H_audio, bias);
    M2 = lut_m_end_k45(M1, H_audio, bias);
    s2 = lut_sum_rest_k45(M1, H_audio, bias);
    M3 = lut_m_end_k45(M2, H_audio, bias);
    s3 = lut_sum_rest_k45(M2, H_audio, bias);
    M_end = lut_m_end_k45(M3, H_audio, bias);
    s4 = lut_sum_rest_k45(M3, H_audio, bias);
    M_avg = (s1 + s2 + s3 + s4) * inv_n_k45;
  };
};

// K90 cascade (4×90 = 360 substeps)
cascade_k90(H_audio, bias) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg with {
    M1 = lut_m_end_k90(M_prev, H_audio, bias);
    s1 = lut_sum_rest_k90(M_prev, H_audio, bias);
    M2 = lut_m_end_k90(M1, H_audio, bias);
    s2 = lut_sum_rest_k90(M1, H_audio, bias);
    M3 = lut_m_end_k90(M2, H_audio, bias);
    s3 = lut_sum_rest_k90(M2, H_audio, bias);
    M_end = lut_m_end_k90(M3, H_audio, bias);
    s4 = lut_sum_rest_k90(M3, H_audio, bias);
    M_avg = (s1 + s2 + s3 + s4) * inv_n_k90;
  };
};

//==============================================================================
// Quality Mode Selection via ondemand
//==============================================================================
ja_hysteresis_quality(H_audio, bias, quality_mode) = output
with {
  mode = int(quality_mode + 0.5);
  clk(i) = (mode == i);

  // Each mode gated by ondemand
  q0 = clk(0) : ondemand(cascade_k23(H_audio, bias));
  q1 = clk(1) : ondemand(cascade_k45(H_audio, bias));
  q2 = clk(2) : ondemand(cascade_k90(H_audio, bias));

  output = clk(0) * q0 + clk(1) * q1 + clk(2) * q2;
};

//==============================================================================
// Channel Processing
//==============================================================================
fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val, bias_level, quality_mode) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  input_gain = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain = ba.db2linear(drive_db) : si.smoo;
  drive_comp = (1.0 / drive_gain) * ba.db2linear(15.6);

  bias_comp_db = ba.if(bias_level < 0.5,
    (bias_level - 0.5) * 20.0,
    (bias_level - 0.5) * 25.0);
  bias_comp = ba.db2linear(bias_comp_db);

  dc_blocker = fi.SVFTPT.HP2(7.0, 0.74);
  pre_ja_atten = ba.db2linear(-12.8);

  process_hyst(x) = ja_hysteresis_quality(x, bias_level, quality_mode);
  wet_gained = _ * input_gain : *(drive_gain) : *(pre_ja_atten)
    : process_hyst
    : dc_blocker : *(drive_comp) : *(bias_comp) : *(output_gain);
};

//==============================================================================
// UI
//==============================================================================
fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix, bias_level, quality_mode)
with {
  quality_mode = hgroup("JA 3D Quality Test", hgroup("[00] QUALITY",
    vslider("[0]Mode [style:menu{'K23 Eco':0;'K45 Standard':1;'K90 HQ':2}]", 1, 0, 2, 1)));
  bias_level = hgroup("JA 3D Quality Test", hgroup("[01] BIAS",
    vslider("[0]Level [style:knob]", 0.4, 0.1, 0.9, 0.01)));
  input_gain_db = hgroup("JA 3D Quality Test", hgroup("[02] GAIN",
    vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA 3D Quality Test", hgroup("[02] GAIN",
    vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db = hgroup("JA 3D Quality Test", hgroup("[02] GAIN",
    vslider("[2]Drive [dB]", 0.0, -30.0, 30.0, 0.1)));
  mix = hgroup("JA 3D Quality Test", hgroup("[02] GAIN",
    vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));
};

process = par(i, 2, fsm_channel_ui);
