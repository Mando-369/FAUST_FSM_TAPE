// jahysteresis_lite.lib Prototype - 3 Bias Presets with CASCADED LUT
// Jiles-Atherton model optimized with cascaded 2D LUT lookups.
// 4 x K29 LUT lookups = K116 total (4 cycles x 29 substeps).
// Pure LUT - no real substeps, course-corrects every 29 steps.
//
// 3 bias presets via ondemand (only active preset computes):
// - Low:  bias_level=0.29 (subtle)
// - Mid:  bias_level=0.47 (balanced)
// - High: bias_level=0.74 (saturated)

import("stdfaust.lib");

// Import the 3 precomputed K29 LUTs (1 cycle each, chained 4x = K116)
import("ja_lut_k29_bias_low.lib");
import("ja_lut_k29_bias_mid.lib");
import("ja_lut_k29_bias_high.lib");

//==============================================================================
// fsm_channel: Main processing function
//==============================================================================
fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val,
             lambda_tilt, bias_preset) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  // ===== Gains =====
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = (1.0 / drive_gain) * ba.db2linear(15.6);  // drive inverse + JA makeup

  // ===== Fixed physics constants (matches C++ and LUT generation) =====
  // These are baked into the LUT - cannot be changed at runtime
  Ms = 320.0;
  a_density = 720.0;
  k_pinning = 280.0;
  c_reversibility = 0.18;
  alpha_coupling = 0.015;

  Ms_safe    = max(Ms, 1e-6);
  alpha_norm = alpha_coupling;
  a_norm     = a_density / Ms_safe;
  inv_a_norm = 1.0 / max(a_norm, 1e-9);
  k_norm     = k_pinning / Ms_safe;
  c_norm     = c_reversibility;
  sigma      = 1e-6;  // Matches C++

  // ===== K116: 4 cycles x 29 substeps = 116 total =====
  two_pi = 2.0 * ma.PI;
  substep_phase = two_pi / 29.0;  // phase per substep
  inv_n = 1.0 / 116.0;

  // ===== Real tanh =====
  ja_tanh = ma.tanh;

  // ===== ja_dMdH: compute dM/dH at point (M, H) with direction dH_sign =====
  // Matches C++ JAHysteresisScheduler::computeDMdH exactly
  ja_dMdH(M, H, dH_sign) = result
  with {
    He = H + alpha_norm * M;
    Man_e = ja_tanh(He * inv_a_norm);
    dMan_dH = (1.0 - Man_e * Man_e) * inv_a_norm;
    diff = Man_e - M;
    dir = ba.if(dH_sign >= 0.0, 1.0, -1.0);
    pin = dir * k_norm - alpha_norm * diff;
    denom = 1.0 - c_norm * alpha_norm * dMan_dH;
    result = (c_norm * dMan_dH + diff / (pin + sigma)) / (denom + 1e-9);
  };

  // ===== Core JA substep using RK4 (only 1 per sample) =====
  // Matches C++ JAHysteresisScheduler::executeSubstepRK4 exactly
  ja_substep(bias_val, M_prev, H_prev, H_audio, bias_amplitude) = M_new, H_new
  with {
    H_new = H_audio + bias_amplitude * bias_val;
    dH = H_new - H_prev;
    h = dH;

    // k1: slope at start point
    k1 = ja_dMdH(M_prev, H_prev, dH);

    // k2: slope at midpoint using k1
    k2 = ja_dMdH(M_prev + 0.5*k1*h, H_prev + 0.5*h, dH);

    // k3: slope at midpoint using k2
    k3 = ja_dMdH(M_prev + 0.5*k2*h, H_prev + 0.5*h, dH);

    // k4: slope at endpoint using k3
    k4 = ja_dMdH(M_prev + k3*h, H_new, dH);

    // Weighted average of slopes
    dM = (k1 + 2.0*k2 + 2.0*k3 + k4) * h / 6.0;
    M_new = max(-1.0, min(1.0, M_prev + dM));
  };

  // ===== CASCADED LUT-based hysteresis =====
  // 4 x K29 lookups = K116 total. Course-corrects every 29 substeps.
  // Each K29 LUT covers 1 bias cycle (2π), chained 4 times = 4 cycles (8π).
  ja_hysteresis_lut(lut_m_end, lut_sum, bias_amplitude, H_audio) =
    (loop ~ mem) : ba.selector(1, 2)
  with {
    loop(recM) = M_end, Mavg
    with {
      // Cascade 1: substeps 0-28
      M1 = lut_m_end(recM, H_audio);
      sum1 = lut_sum(recM, H_audio);

      // Cascade 2: substeps 29-57
      M2 = lut_m_end(M1, H_audio);
      sum2 = lut_sum(M1, H_audio);

      // Cascade 3: substeps 58-86
      M3 = lut_m_end(M2, H_audio);
      sum3 = lut_sum(M2, H_audio);

      // Cascade 4: substeps 87-115
      M_end = lut_m_end(M3, H_audio);
      sum4 = lut_sum(M3, H_audio);

      // Total average: (sum1 + sum2 + sum3 + sum4) / 116
      Mavg = (sum1 + sum2 + sum3 + sum4) * inv_n;
    };
  };

  // ===== Mode selection from UI =====
  // 0 = Low, 1 = Mid, 2 = High
  mode = int(bias_preset + 0.5);
  clk(i) = (mode == i);

  // ===== Fixed bias amplitudes per preset (MUST match LUT generation) =====
  // Low=0.29×11=3.19, Mid=0.47×11=5.17, High=0.74×11=8.14
  bias_amp_low = 0.29 * 11.0;
  bias_amp_mid = 0.47 * 11.0;
  bias_amp_high = 0.74 * 11.0;

  // ===== Streaming JA hysteresis with ondemand bias preset selection =====
  ja_hysteresis(H_in) =
    sum(i, 3, clk(i) * (clk(i) : ondemand(loop(i, H_in))))
  with {
    // Mode-indexed LUT selection with matching bias amplitudes (K29 cascaded 4x)
    loop(0, H) = ja_hysteresis_lut(ja_lookup_m_end_k29_bias_low, ja_lookup_sum_m_rest_k29_bias_low, bias_amp_low, H);
    loop(1, H) = ja_hysteresis_lut(ja_lookup_m_end_k29_bias_mid, ja_lookup_sum_m_rest_k29_bias_mid, bias_amp_mid, H);
    loop(2, H) = ja_hysteresis_lut(ja_lookup_m_end_k29_bias_high, ja_lookup_sum_m_rest_k29_bias_high, bias_amp_high, H);
  };

  // ===== DC blocker =====
  dc_blocker = fi.SVFTPT.HP2(5.0, 0.7071);

  // ===== Wavelength saturation =====
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  // ===== Bias compensation (per preset, measured) =====
  // Reference: mid preset = 0dB
  bias_comp_db = ba.selectn(3, bias_preset, -2.0, 0.0, 2.5);
  bias_comp = ba.db2linear(bias_comp_db);

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
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix, lambda_tilt, bias_preset)
with {
  // Group 0: BIAS PRESET - LUT selection (uses ondemand, only active preset computes)
  bias_preset = hgroup("JA Lite", hgroup("[00] PRESET", nentry("[0]Bias [style:menu{'Low (0.29)':0;'Mid (0.47)':1;'High (0.74)':2}]", 1, 0, 2, 1)));

  // Group 1: GAIN
  input_gain_db  = hgroup("JA Lite", hgroup("[01] GAIN", vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA Lite", hgroup("[01] GAIN", vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db       = hgroup("JA Lite", hgroup("[01] GAIN", vslider("[2]Drive [dB]", 0.0, -18.0, 29.0, 0.1)));
  mix            = hgroup("JA Lite", hgroup("[01] GAIN", vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));

  // Group 2: TAPE - wavelength saturation (post-JA tilt)
  lambda_tilt = hgroup("JA Lite", hgroup("[02] TAPE", vslider("[0]Lambda Tilt", 0.0, -0.1, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_ui);
