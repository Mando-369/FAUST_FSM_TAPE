// ja_bias_averaged.dsp - Bias-Averaged JA Hysteresis
// 4 mini-substeps per sample using cycle-averaged anhysteretic curve
// Based on FSM_UPDATE concept: average bias effect instead of explicit oscillator
//
// Key insight: Instead of 28-92 substeps computing sin(phase) at each,
// we compute the cycle-averaged Man and dMan using 4-point quadrature.
// Then apply 4 mini-steps to spread dH across multiple pinning applications.
// This captures both bias linearization AND compression behavior.
// Total: 4 substeps × 4 quadrature points × 2 functions = 32 tanh calls/sample

import("stdfaust.lib");

//==============================================================================
// fsm_channel: Main processing with bias-averaged JA
//==============================================================================
fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val,
             bias_level, bias_scale) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  // Gains
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = (1.0 / drive_gain) * ba.db2linear(15.6);

  // Physics constants (matching JA defaults)
  Ms = 320.0;
  a_density = 720.0;
  k_pinning = 280.0;
  c_rev = 0.18;
  alpha = 0.015;

  // Derived constants
  Ms_safe = max(Ms, 1e-6);
  alpha_norm = alpha;
  a_norm = a_density / Ms_safe;
  inv_a_norm = 1.0 / max(a_norm, 1e-9);
  k_norm = k_pinning / Ms_safe;
  c_norm = c_rev;
  sigma = 1e-6;

  // Bias amplitude (smoothed)
  bias_amp = (bias_level * bias_scale) : si.smoo;

  // 4-point quadrature offsets for bias averaging
  // Phases: 0, π/2, π, 3π/2
  bias_off_0 = 0.0;   // sin(0)
  bias_off_1 = 1.0;   // sin(π/2)
  bias_off_2 = 0.0;   // sin(π)
  bias_off_3 = -1.0;  // sin(3π/2)

  // Soft clamp scale (matches OD_3_modes K92)
  diff_scale = 2.53;

  // Bias-averaged anhysteretic: mean of tanh at 4 bias offsets
  man_avg(He) = (m0 + m1 + m2 + m3) * 0.25
  with {
    m0 = ma.tanh((He + bias_amp * bias_off_0) * inv_a_norm);
    m1 = ma.tanh((He + bias_amp * bias_off_1) * inv_a_norm);
    m2 = ma.tanh((He + bias_amp * bias_off_2) * inv_a_norm);
    m3 = ma.tanh((He + bias_amp * bias_off_3) * inv_a_norm);
  };

  // Bias-averaged derivative: mean of (1 - tanh^2) at 4 offsets
  dman_avg(He) = (d0 + d1 + d2 + d3) * 0.25
  with {
    t0 = ma.tanh((He + bias_amp * bias_off_0) * inv_a_norm);
    t1 = ma.tanh((He + bias_amp * bias_off_1) * inv_a_norm);
    t2 = ma.tanh((He + bias_amp * bias_off_2) * inv_a_norm);
    t3 = ma.tanh((He + bias_amp * bias_off_3) * inv_a_norm);
    d0 = (1.0 - t0*t0) * inv_a_norm;
    d1 = (1.0 - t1*t1) * inv_a_norm;
    d2 = (1.0 - t2*t2) * inv_a_norm;
    d3 = (1.0 - t3*t3) * inv_a_norm;
  };

  // JA substep using bias-averaged Man and dMan
  // Apply pinning term with fractional dH for compression behavior
  ja_substep(M_prev, H_prev, H_target, dH_frac) = M_new, H_new
  with {
    H_new = H_prev + dH_frac;
    dH = dH_frac;
    He = H_new + alpha_norm * M_prev;

    // Use bias-averaged anhysteretic
    Man_e = man_avg(He);
    dMan_dH = dman_avg(He);

    // Soft clamp on diff to stabilize pinning term (key for compression!)
    diff_raw = Man_e - M_prev;
    diff = diff_raw / (1.0 + abs(diff_raw) * diff_scale);

    dir = ba.if(dH >= 0.0, 1.0, -1.0);
    pin = dir * k_norm - alpha_norm * diff;
    inv_pin = 1.0 / (pin + sigma);

    denom = 1.0 - c_norm * alpha_norm * dMan_dH;
    inv_denom = 1.0 / (denom + 1e-9);
    dMdH = (c_norm * dMan_dH + diff * inv_pin) * inv_denom;
    dM = dMdH * dH;

    M_unclamped = M_prev + dM;
    M_new = max(-1.0, min(1.0, M_unclamped));
  };

  // 4 mini-steps per sample with averaged bias values
  // Spreads dH across multiple pinning term applications = compression!
  n_substeps = 4;
  inv_n = 1.0 / n_substeps;

  ja_update(M_prev, H_prev, H_audio) = M4, H_audio
  with {
    dH_total = H_audio - H_prev;
    dH_frac = dH_total * inv_n;

    // 4 sequential substeps
    step1 = ja_substep(M_prev, H_prev, H_audio, dH_frac);
    M1 = ba.selector(0, 2, step1);
    H1 = ba.selector(1, 2, step1);

    step2 = ja_substep(M1, H1, H_audio, dH_frac);
    M2 = ba.selector(0, 2, step2);
    H2 = ba.selector(1, 2, step2);

    step3 = ja_substep(M2, H2, H_audio, dH_frac);
    M3 = ba.selector(0, 2, step3);
    H3 = ba.selector(1, 2, step3);

    step4 = ja_substep(M3, H3, H_audio, dH_frac);
    M4 = ba.selector(0, 2, step4);
  };

  // Streaming hysteresis with feedback
  ja_hysteresis(H_in) = ((loop ~ (_, _)) : (!, !, _))
  with {
    loop(M_prev, H_prev) = M_new, H_new, M_new
    with {
      result = ja_update(M_prev, H_prev, H_in);
      M_new = ba.selector(0, 2, result);
      H_new = ba.selector(1, 2, result);
    };
  };

  // DC blocker
  dc_blocker = fi.SVFTPT.HP2(5.0, 0.7071);

  // Bias compensation (piecewise linear)
  bias_comp_db = ba.if(bias_level < 0.5,
    (bias_level - 0.5) * 20.0,
    (bias_level - 0.5) * 25.0);
  bias_comp = ba.db2linear(bias_comp_db);

  // Processing chain
  fsm_stage(x) = x * input_gain : *(drive_gain) : ja_hysteresis
    : dc_blocker : *(drive_comp) : *(bias_comp);
  wet_gained = fsm_stage : *(output_gain);
};

//==============================================================================
// UI
//==============================================================================
fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix, bias_level, bias_scale)
with {
  bias_level     = hgroup("JA Bias-Avg", hgroup("[00] BIAS", vslider("[0]Level [style:knob]", 0.4, 0.01, 1.0, 0.01)));
  bias_scale     = hgroup("JA Bias-Avg", hgroup("[00] BIAS", vslider("[1]Scale [style:knob]", 11.0, 1.0, 50.0, 0.1)));
  input_gain_db  = hgroup("JA Bias-Avg", hgroup("[01] GAIN", vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA Bias-Avg", hgroup("[01] GAIN", vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db       = hgroup("JA Bias-Avg", hgroup("[01] GAIN", vslider("[2]Drive [dB]", 0.0, -18.0, 29.0, 0.1)));
  mix            = hgroup("JA Bias-Avg", hgroup("[01] GAIN", vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));
};

process = par(i, 2, fsm_channel_ui);
