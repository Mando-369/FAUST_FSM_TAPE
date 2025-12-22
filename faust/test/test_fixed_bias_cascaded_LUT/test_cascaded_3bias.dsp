// Cascaded LUT with 3 bias presets (no ondemand)
// Uses ba.selectn for runtime selection

import("stdfaust.lib");
import("ja_lut_k29_bias_low.lib");
import("ja_lut_k29_bias_mid.lib");
import("ja_lut_k29_bias_high.lib");

//==============================================================================
// fsm_channel: 3 bias presets with ba.selectn
//==============================================================================
fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val, lambda_tilt, bias_preset) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = (1.0 / drive_gain) * ba.db2linear(15.6);

  inv_n = 1.0 / 116.0;
  mode = int(bias_preset + 0.5);

  // CASCADED LUT for each bias preset
  ja_hysteresis_low(H_audio) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg
    with {
      M1 = ja_lookup_m_end_k29_bias_low(recM, H_audio);
      sum1 = ja_lookup_sum_m_rest_k29_bias_low(recM, H_audio);
      M2 = ja_lookup_m_end_k29_bias_low(M1, H_audio);
      sum2 = ja_lookup_sum_m_rest_k29_bias_low(M1, H_audio);
      M3 = ja_lookup_m_end_k29_bias_low(M2, H_audio);
      sum3 = ja_lookup_sum_m_rest_k29_bias_low(M2, H_audio);
      M_end = ja_lookup_m_end_k29_bias_low(M3, H_audio);
      sum4 = ja_lookup_sum_m_rest_k29_bias_low(M3, H_audio);
      Mavg = (sum1 + sum2 + sum3 + sum4) * inv_n;
    };
  };

  ja_hysteresis_mid(H_audio) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg
    with {
      M1 = ja_lookup_m_end_k29_bias_mid(recM, H_audio);
      sum1 = ja_lookup_sum_m_rest_k29_bias_mid(recM, H_audio);
      M2 = ja_lookup_m_end_k29_bias_mid(M1, H_audio);
      sum2 = ja_lookup_sum_m_rest_k29_bias_mid(M1, H_audio);
      M3 = ja_lookup_m_end_k29_bias_mid(M2, H_audio);
      sum3 = ja_lookup_sum_m_rest_k29_bias_mid(M2, H_audio);
      M_end = ja_lookup_m_end_k29_bias_mid(M3, H_audio);
      sum4 = ja_lookup_sum_m_rest_k29_bias_mid(M3, H_audio);
      Mavg = (sum1 + sum2 + sum3 + sum4) * inv_n;
    };
  };

  ja_hysteresis_high(H_audio) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg
    with {
      M1 = ja_lookup_m_end_k29_bias_high(recM, H_audio);
      sum1 = ja_lookup_sum_m_rest_k29_bias_high(recM, H_audio);
      M2 = ja_lookup_m_end_k29_bias_high(M1, H_audio);
      sum2 = ja_lookup_sum_m_rest_k29_bias_high(M1, H_audio);
      M3 = ja_lookup_m_end_k29_bias_high(M2, H_audio);
      sum3 = ja_lookup_sum_m_rest_k29_bias_high(M2, H_audio);
      M_end = ja_lookup_m_end_k29_bias_high(M3, H_audio);
      sum4 = ja_lookup_sum_m_rest_k29_bias_high(M3, H_audio);
      Mavg = (sum1 + sum2 + sum3 + sum4) * inv_n;
    };
  };

  // Select which output to use (all 3 compute, but only one is selected)
  ja_hysteresis(H_in) = ba.selectn(3, mode, ja_hysteresis_low(H_in), ja_hysteresis_mid(H_in), ja_hysteresis_high(H_in));

  dc_blocker = fi.SVFTPT.HP2(5.0, 0.7071);
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  // Bias compensation per preset
  bias_comp_db = ba.selectn(3, bias_preset, -2.0, 0.0, 2.5);
  bias_comp = ba.db2linear(bias_comp_db);

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

fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix, lambda_tilt, bias_preset)
with {
  bias_preset    = hgroup("JA Lite", hgroup("[0] PRESET", nentry("[0]Bias [style:menu{'Low':0;'Mid':1;'High':2}]", 1, 0, 2, 1)));
  input_gain_db  = hgroup("JA Lite", hgroup("[1] GAIN", vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA Lite", hgroup("[1] GAIN", vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db       = hgroup("JA Lite", hgroup("[1] GAIN", vslider("[2]Drive [dB]", 0.0, -18.0, 29.0, 0.1)));
  mix            = hgroup("JA Lite", hgroup("[1] GAIN", vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));
  lambda_tilt    = hgroup("JA Lite", hgroup("[2] TAPE", vslider("[0]Lambda Tilt", 0.0, -0.1, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_ui);
