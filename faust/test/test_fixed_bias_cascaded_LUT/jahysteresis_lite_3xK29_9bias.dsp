// jahysteresis_lite - 3x K29 Cascaded LUT with 9 Bias Presets
// Discrete bias selection (0.1-0.9 in 0.1 steps)

import("stdfaust.lib");
import("ja_lut_k29_bias_01.lib");
import("ja_lut_k29_bias_02.lib");
import("ja_lut_k29_bias_03.lib");
import("ja_lut_k29_bias_04.lib");
import("ja_lut_k29_bias_05.lib");
import("ja_lut_k29_bias_06.lib");
import("ja_lut_k29_bias_07.lib");
import("ja_lut_k29_bias_08.lib");
import("ja_lut_k29_bias_09.lib");

fsm_channel(input_gain_db, output_gain_db, drive_db, mix_val,
             lambda_tilt, bias_preset) =
  ef.dryWetMixer(mix_val, wet_gained)
with {
  input_gain  = ba.db2linear(input_gain_db) : si.smoo;
  output_gain = ba.db2linear(output_gain_db) : si.smoo;
  drive_gain  = ba.db2linear(drive_db) : si.smoo;
  drive_comp  = (1.0 / drive_gain) * ba.db2linear(15.6);
  inv_n = 1.0 / 87.0;
  mode = int(bias_preset + 0.5);

  // 3xK29 hysteresis for each bias preset
  ja_hysteresis_01(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_01(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_01(recM, H);
      M2 = ja_lookup_m_end_k29_bias_01(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_01(M1, H);
      M_end = ja_lookup_m_end_k29_bias_01(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_01(M2, H);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  ja_hysteresis_02(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_02(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_02(recM, H);
      M2 = ja_lookup_m_end_k29_bias_02(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_02(M1, H);
      M_end = ja_lookup_m_end_k29_bias_02(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_02(M2, H);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  ja_hysteresis_03(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_03(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_03(recM, H);
      M2 = ja_lookup_m_end_k29_bias_03(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_03(M1, H);
      M_end = ja_lookup_m_end_k29_bias_03(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_03(M2, H);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  ja_hysteresis_04(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_04(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_04(recM, H);
      M2 = ja_lookup_m_end_k29_bias_04(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_04(M1, H);
      M_end = ja_lookup_m_end_k29_bias_04(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_04(M2, H);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  ja_hysteresis_05(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_05(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_05(recM, H);
      M2 = ja_lookup_m_end_k29_bias_05(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_05(M1, H);
      M_end = ja_lookup_m_end_k29_bias_05(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_05(M2, H);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  ja_hysteresis_06(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_06(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_06(recM, H);
      M2 = ja_lookup_m_end_k29_bias_06(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_06(M1, H);
      M_end = ja_lookup_m_end_k29_bias_06(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_06(M2, H);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  ja_hysteresis_07(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_07(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_07(recM, H);
      M2 = ja_lookup_m_end_k29_bias_07(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_07(M1, H);
      M_end = ja_lookup_m_end_k29_bias_07(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_07(M2, H);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  ja_hysteresis_08(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_08(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_08(recM, H);
      M2 = ja_lookup_m_end_k29_bias_08(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_08(M1, H);
      M_end = ja_lookup_m_end_k29_bias_08(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_08(M2, H);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  ja_hysteresis_09(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_09(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_09(recM, H);
      M2 = ja_lookup_m_end_k29_bias_09(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_09(M1, H);
      M_end = ja_lookup_m_end_k29_bias_09(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_09(M2, H);
      Mavg = (s1+s2+s3) * inv_n;
    };
  };

  // Discrete selection (no crossfade)
  ja_hysteresis(H_in) = ba.selectn(9, mode,
    ja_hysteresis_01(H_in), ja_hysteresis_02(H_in), ja_hysteresis_03(H_in),
    ja_hysteresis_04(H_in), ja_hysteresis_05(H_in), ja_hysteresis_06(H_in),
    ja_hysteresis_07(H_in), ja_hysteresis_08(H_in), ja_hysteresis_09(H_in));

  dc_blocker = fi.SVFTPT.HP2(5.0, 0.7071);
  lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);

  // Bias compensation per preset (measured)
  bias_comp_db = ba.selectn(9, mode, -7.5, -5.4, -2.5, 0.4, 3.0, 5.2, 7.1, 8.9, 10.5);
  bias_comp = ba.db2linear(bias_comp_db);

  fsm_stage(x) = x * input_gain : *(drive_gain) : lambda_sat : ja_hysteresis
    : dc_blocker : *(drive_comp) : *(bias_comp);
  wet_gained = fsm_stage : *(output_gain);
};

fsm_channel_ui =
  fsm_channel(input_gain_db, output_gain_db, drive_db, mix, lambda_tilt, bias_preset)
with {
  bias_preset    = hgroup("JA Lite 3xK29", hgroup("[00] BIAS", vslider("[0]Bias [style:knob]", 4, 0, 8, 1)));
  input_gain_db  = hgroup("JA Lite 3xK29", hgroup("[01] GAIN", vslider("[0]Input [dB]", 0.0, -24.0, 24.0, 0.1)));
  output_gain_db = hgroup("JA Lite 3xK29", hgroup("[01] GAIN", vslider("[1]Output [dB]", 0.0, -24.0, 24.0, 0.1)));
  drive_db       = hgroup("JA Lite 3xK29", hgroup("[01] GAIN", vslider("[2]Drive [dB]", 0.0, -18.0, 29.0, 0.1)));
  mix            = hgroup("JA Lite 3xK29", hgroup("[01] GAIN", vslider("[3]Mix", 1.0, 0.0, 1.0, 0.01)));
  lambda_tilt    = hgroup("JA Lite 3xK29", hgroup("[02] TAPE", vslider("[0]Lambda Tilt", 0.0, -0.1, 0.1, 0.001)));
};

process = par(i, 2, fsm_channel_ui);
