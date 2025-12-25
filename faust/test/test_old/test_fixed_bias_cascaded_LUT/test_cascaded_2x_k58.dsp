// Cascaded LUT with K58 (2 cascades = K116)
// Single bias preset (mid) to test compilation

import("stdfaust.lib");
import("ja_lut_k58_bias_mid.lib");

//==============================================================================
// fsm_channel: 2x K58 cascade = K116
//==============================================================================
fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val, lambda_tilt) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = (1.0 / drive_gain) * ba.db2linear(15.6);

  inv_n = 1.0 / 116.0;

  // 2x K58 CASCADE = K116
  ja_hysteresis_lut(H_audio) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg
    with {
      // Cascade 1 (substeps 0-57)
      M1 = ja_lookup_m_end_k58_bias_mid(recM, H_audio);
      sum1 = ja_lookup_sum_m_rest_k58_bias_mid(recM, H_audio);

      // Cascade 2 (substeps 58-115)
      M_end = ja_lookup_m_end_k58_bias_mid(M1, H_audio);
      sum2 = ja_lookup_sum_m_rest_k58_bias_mid(M1, H_audio);

      // Average over 116 substeps
      Mavg = (sum1 + sum2) * inv_n;
    };
  };

  dc_blocker = fi.SVFTPT.HP2(5.0, 0.7071);
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  fsm_stage(x) =
    x * input_gain
    : *(drive_gain)
    : lambda_sat
    : ja_hysteresis_lut
    : dc_blocker
    : *(drive_comp);

  wet_gained = fsm_stage : *(output_gain);
};

fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix, lambda_tilt)
with {
  input_gain_db  = hgroup("JA Lite", vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1));
  output_gain_db = hgroup("JA Lite", vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1));
  drive_db       = hgroup("JA Lite", vslider("[2]Drive [dB]", 0.0, -18.0, 29.0, 0.1));
  mix            = hgroup("JA Lite", vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01));
  lambda_tilt    = hgroup("JA Lite", vslider("[4]Lambda Tilt", 0.0, -0.1, 0.1, 0.001));
};

process = par(i, 2, fsm_channel_ui);
