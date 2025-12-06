// jahysteresis.lib Prototype - Single Mode (K60 Ultra)
// Matching C++ reference: 3 cycles × 24 = 72 substeps
// With proper phase continuity across samples

import("stdfaust.lib");

//==============================================================================
// tape_channel: Main processing function
//==============================================================================
tape_channel(input_gain_db, output_gain_db, drive_db, mix_val) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  // ===== Gains =====
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = 1.0 / drive_gain;

  // ===== Physics parameters (exposed) =====
  Ms              = hslider("Ms (Saturation)", 320.0, 100.0, 1000.0, 1.0) : si.smoo;
  a_density       = hslider("a (Density)", 720.0, 100.0, 2000.0, 1.0) : si.smoo;
  k_pinning       = hslider("k (Pinning)", 280.0, 50.0, 1000.0, 1.0) : si.smoo;
  c_reversibility = hslider("c (Reversibility)", 0.18, 0.0, 1.0, 0.01) : si.smoo;
  alpha_coupling  = hslider("alpha (Coupling)", 0.015, 0.001, 0.1, 0.001) : si.smoo;

  // ===== Bias parameters (exposed) =====
  bias_level = hslider("Bias Level", 0.4, 0.0, 1.0, 0.01) : si.smoo;
  bias_scale = hslider("Bias Scale", 11.0, 1.0, 100.0, 0.1) : si.smoo;

  // ===== Derived constants =====
  Ms_safe    = max(Ms, 1e-6);
  alpha_norm = alpha_coupling;
  a_norm     = a_density / Ms_safe;
  inv_a_norm = 1.0 / max(a_norm, 1e-9);
  k_norm     = k_pinning / Ms_safe;
  c_norm     = c_reversibility;
  sigma      = 1e-6;
  bias_amp   = bias_level * bias_scale;

  two_pi = 2.0 * ma.PI;

  // ===== K60 Ultra: 3 cycles, 24 substeps/cycle = 72 total =====
  substep_phase = two_pi / 24.0;  // 2π per cycle / 24 substeps
  inv_n = 1.0 / 72.0;

  // ===== Wrap to [0, 2π) like C++: if (phase >= 2π) phase -= 2π =====
  wrap_2pi(p) = ba.if(p >= two_pi, p - two_pi, p);

  // ===== Fast tanh (matching C++) =====
  fast_tanh(x) = clamped * (27.0 + x2) / (27.0 + 9.0 * x2)
  with {
    clamped = max(-3.0, min(3.0, x));
    x2 = clamped * clamped;
  };

  // ===== Core JA substep =====
  ja_substep(bias_offset, M_prev, H_prev, H_audio) = M_new, H_new
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
  };

  // ===== Substep with phase tracking =====
  // State: (M, H, H_audio, M_sum, phase)
  ja_substep_seq(M_prev, H_prev, H_audio, M_sum_prev, phase) =
    M_new, H_new, H_audio, M_sum_new, phase_wrapped
  with {
    midpoint = ma.frac((phase + substep_phase * 0.5) / two_pi) * two_pi;
    bias_offset = sin(midpoint);
    step_result = ja_substep(bias_offset, M_prev, H_prev, H_audio);
    M_new = ba.selector(0, 2, step_result);
    H_new = ba.selector(1, 2, step_result);
    M_sum_new = M_sum_prev + M_new;
    phase_advanced = phase + substep_phase;
    phase_wrapped = wrap_2pi(phase_advanced);
  };

  // ===== 72 substeps via seq =====
  // Output order: M_final, H_final, phase_final, M_sum
  // First 3 are for feedback, last is for averaging
  ja_loop_72(M_prev, H_prev, H_audio, phi_start) =
    M_prev, H_prev, H_audio, 0.0, phi_start : seq(i, 72, ja_substep_seq)
    <: ba.selector(0, 5), ba.selector(1, 5), ba.selector(4, 5), ba.selector(3, 5);
  // [0]=M_final, [1]=H_final, [2]=phase_final, [3]=M_sum

  // ===== Main loop with M, H, and phase feedback =====
  // Feedback takes first 3 outputs (M, H, phase), M_sum is position 3
  ja_hysteresis(H_in) = (loop ~ (mem, mem, mem)) : ba.selector(3, 4) : *(inv_n)
  with {
    loop(recM, recH, recPhi) = recM, recH, H_in, recPhi : ja_loop_72;
  };

  // ===== DC blocker =====
  dc_blocker = fi.SVFTPT.HP2(10.0, 0.74);

  // ===== Tape stage =====
  tape_stage(x) =
    x * input_gain
    : *(drive_gain)
    : ja_hysteresis
    : dc_blocker
    : *(drive_comp);

  wet_gained = tape_stage : *(output_gain);
};

//==============================================================================
// tape_channel_ui: UI wrapper
//==============================================================================
tape_channel_ui =
  tape_channel(input_gain_db, output_gain_db, drive_db, mix)
with {
  input_gain_db  = hslider("Input Gain [dB]", 0.0, -24.0, 24.0, 0.1);
  output_gain_db = hslider("Output Gain [dB]", 15.5, -24.0, 48.0, 0.1);
  drive_db       = hslider("Drive [dB]", 0.0, -18.0, 29.0, 0.1);
  mix            = hslider("Mix [Dry->Wet]", 1.0, 0.0, 1.0, 0.01);
};

process = par(i, 2, tape_channel_ui);
