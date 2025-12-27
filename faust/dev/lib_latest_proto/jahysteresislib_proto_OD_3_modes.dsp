// jahysteresis.lib Prototype - 3 Modes with ondemand (4× CASCADED)
// Jiles-Atherton model of ferromagnetic hysteresis — a physically-based
// mathematical description relating magnetization (M) to applied field (H).
// Combined with phase-locked bias oscillator for analog tape emulation.
//
// 4× CASCADED ARCHITECTURE: matches lite version's quality
//
// 3 quality modes:
// - K180: 4 cascades × 45 = 180 substeps (High Quality, matches tabulateNd)
// - K92:  4 cascades × 23 = 92 substeps (Standard)
// - K44:  4 cascades × 11 = 44 substeps (Eco)

import("stdfaust.lib");

//==============================================================================
// fsm_channel: Main processing function
//==============================================================================
fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val,
             Ms, a_density, k_pinning, c_reversibility, alpha_coupling,
             bias_level, bias_scale, bias_asym, lambda_tilt, quality_mode) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  // ===== Gains =====
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = (1.0 / drive_gain) * ba.db2linear(15.6);  // drive inverse + JA makeup

  // ===== Derived constants =====
  Ms_safe    = max(Ms, 1e-6);
  alpha_norm = alpha_coupling;
  a_norm     = a_density / Ms_safe;
  inv_a_norm = 1.0 / max(a_norm, 1e-9);
  k_norm     = k_pinning / Ms_safe;
  c_norm     = c_reversibility;
  sigma      = 1e-3;  // Moderate safety - keeps inv_pin finite when pin hits zero

  // ===== Bias params (smoothed) =====
  bias_amp = bias_level * bias_scale : si.smoo;
  bias_asym_s = bias_asym : si.smoo;

  // ===== Diff scale per mode (tuned for balanced harmonics) =====
  // K180 = 2.0, K92 = 2.53, K44 = 3.93
  diff_scale = ba.selectn(3, quality_mode, 2.0, 2.53, 3.93);

  // Bias compensation (piecewise linear per mode, measured at scale=11)
  // Reference: bias_level=0.40 = 0dB for all modes
  scale_factor = bias_scale / 11.0;
  bias_comp_db_0 = ba.if(bias_level < 0.40, (bias_level - 0.40) * 22.0, (bias_level - 0.40) * 14.0);
  bias_comp_db_1 = ba.if(bias_level < 0.40, (bias_level - 0.40) * 24.62, (bias_level - 0.40) * 15.42);
  bias_comp_db_2 = ba.if(bias_level < 0.40, (bias_level - 0.40) * 27.69, (bias_level - 0.40) * 15.59);
  bias_comp_db = ba.selectn(3, quality_mode, bias_comp_db_0, bias_comp_db_1, bias_comp_db_2) * scale_factor;
  bias_comp = ba.db2linear(bias_comp_db);

  two_pi = 2.0 * ma.PI;

  // ===== Phase increments per mode (4 cascades) =====
  // K180: 4 cascades × 45 substeps = 180 total, phase = 2π/45
  // K92:  4 cascades × 23 substeps = 92 total, phase = 2π/23
  // K44:  4 cascades × 11 substeps = 44 total, phase = 2π/11
  substep_phase_180 = two_pi / 45.0;
  substep_phase_92 = two_pi / 23.0;
  substep_phase_44 = two_pi / 11.0;

  inv_n_180 = 1.0 / 180.0;
  inv_n_92 = 1.0 / 92.0;
  inv_n_44 = 1.0 / 44.0;

  // ===== Wrap to [0, 2π) =====
  wrap_2pi(p) = ba.if(p >= two_pi, p - two_pi, p);

  // ===== Real tanh =====
  ja_tanh = ma.tanh;

  // ===== Core JA substep (parameterized by substep_phase) =====
  ja_substep(bias_offset, M_prev, H_prev, H_audio) = M_new, H_new
  with {
    H_new = H_audio + bias_amp * bias_offset;
    dH    = H_new - H_prev;
    He    = H_new + alpha_norm * M_prev;

    x_man    = He * inv_a_norm;
    Man_e    = ja_tanh(x_man);
    Man_e2   = Man_e * Man_e;
    dMan_dH  = (1.0 - Man_e2) * inv_a_norm;

    // Soft clamp on (Man_e - M_prev) to stabilize pinning term
    diff = Man_e - M_prev;
    diff_clamped = diff / (1.0 + abs(diff) * diff_scale);

    dir      = ba.if(dH >= 0.0, 1.0, -1.0);
    pin      = dir * k_norm - alpha_norm * diff_clamped;
    inv_pin  = 1.0 / (pin + sigma);

    denom     = 1.0 - c_norm * alpha_norm * dMan_dH;
    inv_denom = 1.0 / (denom + 1e-9);
    dMdH      = (c_norm * dMan_dH + diff_clamped * inv_pin) * inv_denom;
    dM_step   = dMdH * dH;

    M_unclamped = M_prev + dM_step;
    M_new       = max(-1.0, min(1.0, M_unclamped));
  };

  // ===== Substep with phase tracking + bias asymmetry (K180) =====
  ja_substep_seq_180(M_prev, H_prev, H_audio, M_sum_prev, phase) =
    M_new, H_new, H_audio, M_sum_new, phase_wrapped
  with {
    midpoint = ma.frac((phase + substep_phase_180 * 0.5) / two_pi) * two_pi;
    bias_offset = sin(midpoint) + bias_asym_s * sin(2.0 * midpoint);
    step_result = ja_substep(bias_offset, M_prev, H_prev, H_audio);
    M_new = ba.selector(0, 2, step_result);
    H_new = ba.selector(1, 2, step_result);
    M_sum_new = M_sum_prev + M_new;
    phase_advanced = phase + substep_phase_180;
    phase_wrapped = wrap_2pi(phase_advanced);
  };

  // ===== Substep with phase tracking + bias asymmetry (K92) =====
  ja_substep_seq_92(M_prev, H_prev, H_audio, M_sum_prev, phase) =
    M_new, H_new, H_audio, M_sum_new, phase_wrapped
  with {
    midpoint = ma.frac((phase + substep_phase_92 * 0.5) / two_pi) * two_pi;
    bias_offset = sin(midpoint) + bias_asym_s * sin(2.0 * midpoint);
    step_result = ja_substep(bias_offset, M_prev, H_prev, H_audio);
    M_new = ba.selector(0, 2, step_result);
    H_new = ba.selector(1, 2, step_result);
    M_sum_new = M_sum_prev + M_new;
    phase_advanced = phase + substep_phase_92;
    phase_wrapped = wrap_2pi(phase_advanced);
  };

  // ===== Substep with phase tracking + bias asymmetry (K44) =====
  ja_substep_seq_44(M_prev, H_prev, H_audio, M_sum_prev, phase) =
    M_new, H_new, H_audio, M_sum_new, phase_wrapped
  with {
    midpoint = ma.frac((phase + substep_phase_44 * 0.5) / two_pi) * two_pi;
    bias_offset = sin(midpoint) + bias_asym_s * sin(2.0 * midpoint);
    step_result = ja_substep(bias_offset, M_prev, H_prev, H_audio);
    M_new = ba.selector(0, 2, step_result);
    H_new = ba.selector(1, 2, step_result);
    M_sum_new = M_sum_prev + M_new;
    phase_advanced = phase + substep_phase_44;
    phase_wrapped = wrap_2pi(phase_advanced);
  };

  //==============================================================================
  // 4× CASCADED LOOPS - matches lite version quality
  //==============================================================================

  // ===== Single cascade helpers (1 bias cycle each) =====
  ja_k45_cascade(M_in, H_in, H_audio, phi_in) =
    M_in, H_in, H_audio, 0.0, phi_in : seq(i, 45, ja_substep_seq_180)
    <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(4, 5), ba.selector(3, 5);

  ja_k23_cascade(M_in, H_in, H_audio, phi_in) =
    M_in, H_in, H_audio, 0.0, phi_in : seq(i, 23, ja_substep_seq_92)
    <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(4, 5), ba.selector(3, 5);

  ja_k11_cascade(M_in, H_in, H_audio, phi_in) =
    M_in, H_in, H_audio, 0.0, phi_in : seq(i, 11, ja_substep_seq_44)
    <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(4, 5), ba.selector(3, 5);

  // ===== K180 Cascaded: 4 × K45 = 180 substeps (HQ, matches tabulateNd) =====
  ja_loop_180(M_prev, H_prev, H_audio, phi_start) = M_end, H_end, phi_end, M_sum_total
  with {
    c1 = ja_k45_cascade(M_prev, H_prev, H_audio, phi_start);
    M1 = ba.selector(0, 4, c1); H1 = ba.selector(1, 4, c1);
    phi1 = ba.selector(2, 4, c1); s1 = ba.selector(3, 4, c1);

    c2 = ja_k45_cascade(M1, H1, H_audio, phi1);
    M2 = ba.selector(0, 4, c2); H2 = ba.selector(1, 4, c2);
    phi2 = ba.selector(2, 4, c2); s2 = ba.selector(3, 4, c2);

    c3 = ja_k45_cascade(M2, H2, H_audio, phi2);
    M3 = ba.selector(0, 4, c3); H3 = ba.selector(1, 4, c3);
    phi3 = ba.selector(2, 4, c3); s3 = ba.selector(3, 4, c3);

    c4 = ja_k45_cascade(M3, H3, H_audio, phi3);
    M_end = ba.selector(0, 4, c4); H_end = ba.selector(1, 4, c4);
    phi_end = ba.selector(2, 4, c4); s4 = ba.selector(3, 4, c4);

    M_sum_total = s1 + s2 + s3 + s4;
  };

  // ===== K92 Cascaded: 4 × K23 = 92 substeps (Standard) =====
  ja_loop_92(M_prev, H_prev, H_audio, phi_start) = M_end, H_end, phi_end, M_sum_total
  with {
    c1 = ja_k23_cascade(M_prev, H_prev, H_audio, phi_start);
    M1 = ba.selector(0, 4, c1); H1 = ba.selector(1, 4, c1);
    phi1 = ba.selector(2, 4, c1); s1 = ba.selector(3, 4, c1);

    c2 = ja_k23_cascade(M1, H1, H_audio, phi1);
    M2 = ba.selector(0, 4, c2); H2 = ba.selector(1, 4, c2);
    phi2 = ba.selector(2, 4, c2); s2 = ba.selector(3, 4, c2);

    c3 = ja_k23_cascade(M2, H2, H_audio, phi2);
    M3 = ba.selector(0, 4, c3); H3 = ba.selector(1, 4, c3);
    phi3 = ba.selector(2, 4, c3); s3 = ba.selector(3, 4, c3);

    c4 = ja_k23_cascade(M3, H3, H_audio, phi3);
    M_end = ba.selector(0, 4, c4); H_end = ba.selector(1, 4, c4);
    phi_end = ba.selector(2, 4, c4); s4 = ba.selector(3, 4, c4);

    M_sum_total = s1 + s2 + s3 + s4;
  };

  // ===== K44 Cascaded: 4 × K11 = 44 substeps (Eco) =====
  ja_loop_44(M_prev, H_prev, H_audio, phi_start) = M_end, H_end, phi_end, M_sum_total
  with {
    c1 = ja_k11_cascade(M_prev, H_prev, H_audio, phi_start);
    M1 = ba.selector(0, 4, c1); H1 = ba.selector(1, 4, c1);
    phi1 = ba.selector(2, 4, c1); s1 = ba.selector(3, 4, c1);

    c2 = ja_k11_cascade(M1, H1, H_audio, phi1);
    M2 = ba.selector(0, 4, c2); H2 = ba.selector(1, 4, c2);
    phi2 = ba.selector(2, 4, c2); s2 = ba.selector(3, 4, c2);

    c3 = ja_k11_cascade(M2, H2, H_audio, phi2);
    M3 = ba.selector(0, 4, c3); H3 = ba.selector(1, 4, c3);
    phi3 = ba.selector(2, 4, c3); s3 = ba.selector(3, 4, c3);

    c4 = ja_k11_cascade(M3, H3, H_audio, phi3);
    M_end = ba.selector(0, 4, c4); H_end = ba.selector(1, 4, c4);
    phi_end = ba.selector(2, 4, c4); s4 = ba.selector(3, 4, c4);

    M_sum_total = s1 + s2 + s3 + s4;
  };

  // ===== Mode selection from UI =====
  // 0 = K180 (HQ), 1 = K92 (Standard), 2 = K44 (Eco)
  mode = int(quality_mode + 0.5);
  clk(i) = (mode == i);

  // ===== Streaming JA hysteresis with ondemand mode selection =====
  ja_hysteresis(H_in) =
    sum(i, 3, clk(i) * (clk(i) : ondemand(loop(i, H_in))))
  with {
    // Generic loopK: wraps ja_loop with feedback and scaling
    loopK(H, K, inv_n) = ((loop ~ (mem, mem, mem)) : ba.selector(3, 4) : *(inv_n))
    with {
      loop(recM, recH, recPhi) = recM, recH, H, recPhi : K;
    };

    // Mode-indexed loop selection
    loop(0, H) = loopK(H, ja_loop_180, inv_n_180);
    loop(1, H) = loopK(H, ja_loop_92, inv_n_92);
    loop(2, H) = loopK(H, ja_loop_44, inv_n_44);
  };

  // ===== DC blocker =====
  dc_blocker = fi.SVFTPT.HP2(7.0, 0.7071);

  // ===== Wavelength saturation (λ response) =====
  // Tilt filter: HF boost before JA simulates shorter wavelengths saturating harder
  // alpha > 0 = HF boost, range -0.5 to +0.5 (~-3 to +3 dB/octave)
  // Band: 200 Hz to 15200 Hz (extended tape-relevant range)
  // Order 3 = good accuracy vs CPU tradeoff
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  // ===== Mode compensation (dB) =====
  // K180 = -0.5dB, K92 = -0.3dB, K44 = +0.8dB
  mode_comp_db = ba.selectn(3, quality_mode, -0.5, -0.3, 0.8);
  mode_comp = ba.db2linear(mode_comp_db);

  // ===== Asym compensation (dB) - linear from 0dB at asym=0 =====
  // K180: slope -1.5, K92: slope -1.8, K44: slope -4.9
  asym_slope = ba.selectn(3, quality_mode, -1.5, -1.8, -4.9);
  asym_comp_db = bias_asym_s * asym_slope;
  asym_comp = ba.db2linear(asym_comp_db);

  // ===== FSM stage =====
  fsm_stage(x) =
    x * input_gain
    : *(drive_gain)
    : lambda_sat
    : ja_hysteresis
    : dc_blocker
    : *(drive_comp)
    : *(bias_comp)
    : *(mode_comp)
    : *(asym_comp);

  wet_gained = fsm_stage : *(output_gain);
};

//==============================================================================
// fsm_channel_ui: UI wrapper with all controls
//==============================================================================
fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix,
               Ms, a_density, k_pinning, c_reversibility, alpha_coupling,
               bias_level, bias_scale, bias_asym, lambda_tilt, quality_mode)
with {
  // Group 0: QUALITY - mode selection (uses ondemand, only active mode computes)
  quality_mode = hgroup("JA", hgroup("[00] QUALITY", nentry("[0]Mode [style:menu{'K180 HQ':0;'K92 Standard':1;'K44 Eco':2}]", 1, 0, 2, 1)));

  // Group 1: GAIN
  input_gain_db  = hgroup("JA", hgroup("[01] GAIN", vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA", hgroup("[01] GAIN", vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db       = hgroup("JA", hgroup("[01] GAIN", vslider("[2]Drive [dB]", 0.0, -12.0, 50.0, 0.1)));
  mix            = hgroup("JA", hgroup("[01] GAIN", vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));

  // Group 2: BIAS - asymmetry adds 2nd harmonic
  bias_level = hgroup("JA", hgroup("[02] BIAS", vslider("[0]Level", 0.4, 0.01, 1.0, 0.01)));
  bias_scale = hgroup("JA", hgroup("[02] BIAS", vslider("[1]Scale", 11.0, 1.0, 100.0, 0.1)));
  bias_asym  = hgroup("JA", hgroup("[02] BIAS", vslider("[2]Asym", 0.0, 0.0, 0.5, 0.01)));

  // Group 3: TAPE - wavelength saturation (λ response)
  // Tilt: 0 = flat, +0.1 = HF boost, -0.1 = HF cut
  lambda_tilt = hgroup("JA", hgroup("[03] TAPE", vslider("[0]λ Tilt", 0.0, -0.1, 0.1, 0.001)));

  // Group 4: PHYSICS
  Ms              = hgroup("JA", hgroup("[04] PHYSICS", vslider("[0]Ms", 320.0, 100.0, 1000.0, 1.0)));
  a_density       = hgroup("JA", hgroup("[04] PHYSICS", vslider("[1]a", 720.0, 100.0, 2000.0, 1.0)));
  k_pinning       = hgroup("JA", hgroup("[04] PHYSICS", vslider("[2]k", 280.0, 50.0, 1000.0, 1.0)));
  c_reversibility = hgroup("JA", hgroup("[04] PHYSICS", vslider("[3]c", 0.18, 0.0, 1.0, 0.01)));
  alpha_coupling  = hgroup("JA", hgroup("[04] PHYSICS", vslider("[4]alpha", 0.015, 0.001, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_ui);
