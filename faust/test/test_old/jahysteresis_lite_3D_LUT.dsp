// jahysteresis_lite - 3D LUT with Continuous Bias Control
// Single 3D LUT with trilinear interpolation for runtime bias parameter
// 3x K29 cascaded = 87 substeps equivalent

import("stdfaust.lib");
import("ja_lut_k29_3d.lib");

fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val,
             lambda_tilt, bias_level) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = (1.0 / drive_gain) * ba.db2linear(15.6);
  inv_n = 1.0 / 87.0;

  // Smooth bias to prevent zipper noise
  bias_s = bias_level : si.smoo;

  // 3x K29 cascaded with 3D LUT (trilinear interpolation on bias)
  ja_hysteresis(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      // First cascade
      M1 = ja_lookup_m_end_k29(recM, H, bias_s);
      s1 = ja_lookup_sum_m_rest_k29(recM, H, bias_s);
      // Second cascade
      M2 = ja_lookup_m_end_k29(M1, H, bias_s);
      s2 = ja_lookup_sum_m_rest_k29(M1, H, bias_s);
      // Third cascade
      M_end = ja_lookup_m_end_k29(M2, H, bias_s);
      s3 = ja_lookup_sum_m_rest_k29(M2, H, bias_s);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  dc_blocker = fi.SVFTPT.HP2(5.0, 0.7071);
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  // Dynamic bias compensation (piecewise linear, measured)
  // Reference: bias=0.5 = 0dB
  // Low bias (0.1) = -7.5dB, High bias (0.9) = +10.5dB
  bias_comp_db = ba.if(bias_s < 0.5,
    (bias_s - 0.5) * 18.75,   // slope below 0.5: -7.5/(0.5-0.1) = -18.75
    (bias_s - 0.5) * 26.25);  // slope above 0.5: +10.5/(0.9-0.5) = +26.25
  bias_comp = ba.db2linear(bias_comp_db);

  fsm_stage(x) = x * input_gain : *(drive_gain) : lambda_sat : ja_hysteresis
    : dc_blocker : *(drive_comp) : *(bias_comp);
  wet_gained = fsm_stage : *(output_gain);
};

fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix, lambda_tilt, bias_level)
with {
  bias_level     = hgroup("JA Lite 3D", hgroup("[00] BIAS", vslider("[0]Bias [style:knob]", 0.5, 0.1, 0.9, 0.01)));
  input_gain_db  = hgroup("JA Lite 3D", hgroup("[01] GAIN", vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA Lite 3D", hgroup("[01] GAIN", vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db       = hgroup("JA Lite 3D", hgroup("[01] GAIN", vslider("[2]Drive [dB]", 0.0, -18.0, 29.0, 0.1)));
  mix            = hgroup("JA Lite 3D", hgroup("[01] GAIN", vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));
  lambda_tilt    = hgroup("JA Lite 3D", hgroup("[02] TAPE", vslider("[0]Lambda Tilt", 0.0, -0.1, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_ui);
