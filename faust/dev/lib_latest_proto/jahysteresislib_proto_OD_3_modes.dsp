// jahysteresis.lib Prototype - 3 Modes with ondemand
// Jiles-Atherton model of ferromagnetic hysteresis — a physically-based
// mathematical description relating magnetization (M) to applied field (H).
// Combined with phase-locked bias oscillator for analog tape emulation.
//
// 3 quality modes (integer cycles to avoid bias leakage):
// - K96: 4 cycles × 24 = 96 substeps (High Quality)
// - K48: 4 cycles × 12 = 48 substeps (Standard)
// - K24: 4 cycles × 6  = 24 substeps (Eco)

import("stdfaust.lib");

//==============================================================================
// fsm_channel: Main processing function
//==============================================================================
fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val,
             Ms, a_density, k_pinning, c_reversibility, alpha_coupling,
             bias_level, bias_scale, bias_asym, diff_scale, lambda_tilt, quality_mode) =
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
  bias_amp   = bias_level * bias_scale;  // No smoothing - si.smoo breaks ondemand

  // Bias compensation (piecewise linear from measured data)
  // Reference: bias_amp=4.4 (bias_level=0.4, bias_scale=11) = 0dB
  // bias_amp 0.44 → comp -8.2dB, bias_amp 10.78 → comp +6.4dB
  // Slopes: low = 8.2/3.96 = 2.07, high = 6.4/6.38 = 1.003
  bias_comp_db = ba.if(bias_amp < 4.4,
                       (bias_amp - 4.4) * 2.07,
                       (bias_amp - 4.4) * 1.003);
  bias_comp = ba.db2linear(bias_comp_db);

  two_pi = 2.0 * ma.PI;

  // ===== Phase increments per mode (4 cycles each) =====
  // K96: 4 cycles × 24 substeps = 96 total, phase = 2π/24
  // K48: 4 cycles × 12 substeps = 48 total, phase = 2π/12
  // K24: 4 cycles × 6 substeps  = 24 total, phase = 2π/6
  substep_phase_96 = two_pi / 24.0;
  substep_phase_48 = two_pi / 12.0;
  substep_phase_24 = two_pi / 6.0;

  inv_n_96 = 1.0 / 96.0;
  inv_n_48 = 1.0 / 48.0;
  inv_n_24 = 1.0 / 24.0;

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

  // ===== Substep with phase tracking + bias asymmetry (K96) =====
  ja_substep_seq_96(M_prev, H_prev, H_audio, M_sum_prev, phase) =
    M_new, H_new, H_audio, M_sum_new, phase_wrapped
  with {
    midpoint = ma.frac((phase + substep_phase_96 * 0.5) / two_pi) * two_pi;
    bias_offset = sin(midpoint) + bias_asym * sin(2.0 * midpoint);
    step_result = ja_substep(bias_offset, M_prev, H_prev, H_audio);
    M_new = ba.selector(0, 2, step_result);
    H_new = ba.selector(1, 2, step_result);
    M_sum_new = M_sum_prev + M_new;
    phase_advanced = phase + substep_phase_96;
    phase_wrapped = wrap_2pi(phase_advanced);
  };

  // ===== Substep with phase tracking + bias asymmetry (K48) =====
  ja_substep_seq_48(M_prev, H_prev, H_audio, M_sum_prev, phase) =
    M_new, H_new, H_audio, M_sum_new, phase_wrapped
  with {
    midpoint = ma.frac((phase + substep_phase_48 * 0.5) / two_pi) * two_pi;
    bias_offset = sin(midpoint) + bias_asym * sin(2.0 * midpoint);
    step_result = ja_substep(bias_offset, M_prev, H_prev, H_audio);
    M_new = ba.selector(0, 2, step_result);
    H_new = ba.selector(1, 2, step_result);
    M_sum_new = M_sum_prev + M_new;
    phase_advanced = phase + substep_phase_48;
    phase_wrapped = wrap_2pi(phase_advanced);
  };

  // ===== Substep with phase tracking + bias asymmetry (K24) =====
  ja_substep_seq_24(M_prev, H_prev, H_audio, M_sum_prev, phase) =
    M_new, H_new, H_audio, M_sum_new, phase_wrapped
  with {
    midpoint = ma.frac((phase + substep_phase_24 * 0.5) / two_pi) * two_pi;
    bias_offset = sin(midpoint) + bias_asym * sin(2.0 * midpoint);
    step_result = ja_substep(bias_offset, M_prev, H_prev, H_audio);
    M_new = ba.selector(0, 2, step_result);
    H_new = ba.selector(1, 2, step_result);
    M_sum_new = M_sum_prev + M_new;
    phase_advanced = phase + substep_phase_24;
    phase_wrapped = wrap_2pi(phase_advanced);
  };

  // ===== K96: 4 cycles × 24 = 96 substeps (for 48 kHz) =====
  ja_loop_96(M_prev, H_prev, H_audio, phi_start) =
    M_prev, H_prev, H_audio, 0.0, phi_start : seq(i, 96, ja_substep_seq_96)
    <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(4, 5), ba.selector(3, 5);

  // ===== K48: 4 cycles × 12 = 48 substeps (for 96 kHz) =====
  ja_loop_48(M_prev, H_prev, H_audio, phi_start) =
    M_prev, H_prev, H_audio, 0.0, phi_start : seq(i, 48, ja_substep_seq_48)
    <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(4, 5), ba.selector(3, 5);

  // ===== K24: 4 cycles × 6 = 24 substeps (Light) =====
  ja_loop_24(M_prev, H_prev, H_audio, phi_start) =
    M_prev, H_prev, H_audio, 0.0, phi_start : seq(i, 24, ja_substep_seq_24)
    <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(4, 5), ba.selector(3, 5);

  // ===== Mode selection from UI =====
  // 0 = K96 (HQ), 1 = K48 (Standard), 2 = K24 (Eco)
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
    loop(0, H) = loopK(H, ja_loop_96, inv_n_96);
    loop(1, H) = loopK(H, ja_loop_48, inv_n_48);
    loop(2, H) = loopK(H, ja_loop_24, inv_n_24);
  };

  // ===== DC blocker =====
  dc_blocker = fi.SVFTPT.HP2(10.0, 0.74);

  // ===== Wavelength saturation (λ response) =====
  // Tilt filter: HF boost before JA simulates shorter wavelengths saturating harder
  // alpha > 0 = HF boost, range -0.5 to +0.5 (~-3 to +3 dB/octave)
  // Band: 200 Hz to 15200 Hz (extended tape-relevant range)
  // Order 3 = good accuracy vs CPU tradeoff
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  // ===== FSM stage =====
  fsm_stage(x) =
    x * input_gain
    : *(drive_gain)
    : lambda_sat
    : ja_hysteresis
    : dc_blocker
    : *(drive_comp)
    : *(bias_comp);

  wet_gained = fsm_stage : *(output_gain);
};

//==============================================================================
// fsm_channel_ui: UI wrapper with all controls
//==============================================================================
fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix,
               Ms, a_density, k_pinning, c_reversibility, alpha_coupling,
               bias_level, bias_scale, bias_asym, diff_scale, lambda_tilt, quality_mode)
with {
  // Group 0: QUALITY - mode selection (uses ondemand, only active mode computes)
  quality_mode = hgroup("JA", hgroup("[00] QUALITY", nentry("[0]Mode [style:menu{'K96 HQ':0;'K48 Standard':1;'K24 Eco':2}]", 0, 0, 2, 1)));

  // Group 1: GAIN
  input_gain_db  = hgroup("JA", hgroup("[01] GAIN", vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA", hgroup("[01] GAIN", vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db       = hgroup("JA", hgroup("[01] GAIN", vslider("[2]Drive [dB]", 0.0, -18.0, 29.0, 0.1)));
  mix            = hgroup("JA", hgroup("[01] GAIN", vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));

  // Group 2: BIAS - asymmetry adds 2nd harmonic (0.5 max before inversion)
  // No si.smoo on bias params - breaks ondemand code generation
  bias_level = hgroup("JA", hgroup("[02] BIAS", vslider("[0]Level", 0.4, 0.0, 1.0, 0.01)));
  bias_scale = hgroup("JA", hgroup("[02] BIAS", vslider("[1]Scale", 11.0, 1.0, 100.0, 0.1)));
  bias_asym  = hgroup("JA", hgroup("[02] BIAS", vslider("[2]Asym", 0.0, 0.0, 0.5, 0.01)));

  // Group 3: STABILIZATION
  diff_scale = hgroup("JA", hgroup("[03] STAB", vslider("[0]Diff Scale", 1.0, 0.0, 4.0, 0.01)));

  // Group 4: TAPE - wavelength saturation (λ response)
  // Tilt: 0 = flat, +0.5 = +3 dB/oct HF boost, -0.5 = -3 dB/oct HF cut
  lambda_tilt = hgroup("JA", hgroup("[04] TAPE", vslider("[0]λ Tilt", 0.0, -0.5, 0.5, 0.001)));

  // Group 5: PHYSICS
  Ms              = hgroup("JA", hgroup("[05] PHYSICS", vslider("[0]Ms", 320.0, 100.0, 1000.0, 1.0)));
  a_density       = hgroup("JA", hgroup("[05] PHYSICS", vslider("[1]a", 720.0, 100.0, 2000.0, 1.0)));
  k_pinning       = hgroup("JA", hgroup("[05] PHYSICS", vslider("[2]k", 280.0, 50.0, 1000.0, 1.0)));
  c_reversibility = hgroup("JA", hgroup("[05] PHYSICS", vslider("[3]c", 0.18, 0.0, 1.0, 0.01)));
  alpha_coupling  = hgroup("JA", hgroup("[05] PHYSICS", vslider("[4]alpha", 0.015, 0.001, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_ui);
