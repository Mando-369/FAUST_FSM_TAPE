// Test: Single Mode 3 (96 substeps) + Extended Bias Asymmetry
// Jiles-Atherton model of ferromagnetic hysteresis
// 4 cycles × 24 substeps/cycle = 96 total
// No ondemand - single fixed mode for clean CPU testing

import("stdfaust.lib");

//==============================================================================
// fsm_channel: Mode 3 only (96 substeps)
//==============================================================================
fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val,
             Ms, a_density, k_pinning, c_reversibility, alpha_coupling,
             bias_level, bias_scale, bias_asym, diff_scale) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  // ===== Gains =====
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = (1.0 / drive_gain) * ba.db2linear(15.6);

  // ===== Derived constants =====
  Ms_safe    = max(Ms, 1e-6);
  alpha_norm = alpha_coupling;
  a_norm     = a_density / Ms_safe;
  inv_a_norm = 1.0 / max(a_norm, 1e-9);
  k_norm     = k_pinning / Ms_safe;
  c_norm     = c_reversibility;
  sigma      = 1e-3;
  bias_amp   = bias_level * bias_scale;

  // Bias compensation
  bias_comp_db = ba.if(bias_amp < 4.4,
                       (bias_amp - 4.4) * 2.07,
                       (bias_amp - 4.4) * 1.003);
  bias_comp = ba.db2linear(bias_comp_db);

  two_pi = 2.0 * ma.PI;

  // ===== Mode 3: 96 substeps (4 cycles × 24) =====
  substep_phase = two_pi / 24.0;
  inv_n = 1.0 / 96.0;

  wrap_2pi(p) = ba.if(p >= two_pi, p - two_pi, p);

  // ===== Core JA substep =====
  ja_substep(bias_offset, M_prev, H_prev, H_audio) = M_new, H_new
  with {
    H_new = H_audio + bias_amp * bias_offset;
    dH    = H_new - H_prev;
    He    = H_new + alpha_norm * M_prev;

    x_man    = He * inv_a_norm;
    Man_e    = ma.tanh(x_man);
    Man_e2   = Man_e * Man_e;
    dMan_dH  = (1.0 - Man_e2) * inv_a_norm;

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

  // ===== Substep with phase tracking + bias asymmetry =====
  ja_substep_seq(M_prev, H_prev, H_audio, M_sum_prev, phase) =
    M_new, H_new, H_audio, M_sum_new, phase_wrapped
  with {
    midpoint = ma.frac((phase + substep_phase * 0.5) / two_pi) * two_pi;
    // Bias asymmetry: sin + 2nd harmonic for even harmonics
    bias_offset = sin(midpoint) + bias_asym * sin(2.0 * midpoint);
    step_result = ja_substep(bias_offset, M_prev, H_prev, H_audio);
    M_new = ba.selector(0, 2, step_result);
    H_new = ba.selector(1, 2, step_result);
    M_sum_new = M_sum_prev + M_new;
    phase_advanced = phase + substep_phase;
    phase_wrapped = wrap_2pi(phase_advanced);
  };

  // ===== 96 substeps via seq =====
  ja_loop(M_prev, H_prev, H_audio, phi_start) =
    M_prev, H_prev, H_audio, 0.0, phi_start : seq(i, 96, ja_substep_seq)
    <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(4, 5), ba.selector(3, 5);

  // ===== Main loop with M, H, and phase feedback =====
  ja_hysteresis(H_in) = (loop ~ (mem, mem, mem)) : ba.selector(3, 4) : *(inv_n)
  with {
    loop(recM, recH, recPhi) = recM, recH, H_in, recPhi : ja_loop;
  };

  // ===== DC blocker =====
  dc_blocker = fi.SVFTPT.HP2(10.0, 0.74);

  // ===== FSM stage =====
  fsm_stage(x) =
    x * input_gain
    : *(drive_gain)
    : ja_hysteresis
    : dc_blocker
    : *(drive_comp)
    : *(bias_comp);

  wet_gained = fsm_stage : *(output_gain);
};

//==============================================================================
// fsm_channel_ui: UI wrapper - extended bias_asym range (0-1.0)
//==============================================================================
fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix,
               Ms, a_density, k_pinning, c_reversibility, alpha_coupling,
               bias_level, bias_scale, bias_asym, diff_scale)
with {
  // Group 1: GAIN
  input_gain_db  = hgroup("JA", hgroup("[01] GAIN", vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA", hgroup("[01] GAIN", vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db       = hgroup("JA", hgroup("[01] GAIN", vslider("[2]Drive [dB]", 0.0, -18.0, 29.0, 0.1)));
  mix            = hgroup("JA", hgroup("[01] GAIN", vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));

  // Group 2: BIAS - asymmetry 0-0.5 (beyond 0.5 inverts)
  bias_level = hgroup("JA", hgroup("[02] BIAS", vslider("[0]Level", 0.4, 0.0, 1.0, 0.01)));
  bias_scale = hgroup("JA", hgroup("[02] BIAS", vslider("[1]Scale", 11.0, 1.0, 100.0, 0.1)));
  bias_asym  = hgroup("JA", hgroup("[02] BIAS", vslider("[2]Asym", 0.0, 0.0, 0.5, 0.01)));

  // Group 3: STABILIZATION
  diff_scale = hgroup("JA", hgroup("[03] STAB", vslider("[0]Diff Scale", 1.0, 0.0, 4.0, 0.01)));

  // Group 4: PHYSICS
  Ms              = hgroup("JA", hgroup("[04] PHYSICS", vslider("[0]Ms", 320.0, 100.0, 1000.0, 1.0)));
  a_density       = hgroup("JA", hgroup("[04] PHYSICS", vslider("[1]a", 720.0, 100.0, 2000.0, 1.0)));
  k_pinning       = hgroup("JA", hgroup("[04] PHYSICS", vslider("[2]k", 280.0, 50.0, 1000.0, 1.0)));
  c_reversibility = hgroup("JA", hgroup("[04] PHYSICS", vslider("[3]c", 0.18, 0.0, 1.0, 0.01)));
  alpha_coupling  = hgroup("JA", hgroup("[04] PHYSICS", vslider("[4]alpha", 0.015, 0.001, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_ui);
