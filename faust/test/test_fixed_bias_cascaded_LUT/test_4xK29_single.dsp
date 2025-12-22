// Test 4×K29 single preset (mid) with bilinear
import("stdfaust.lib");
import("ja_lut_k29_bias_mid.lib");

fsm_channel(in_db, out_db, drive_db, mix_val, tilt) =
  ef.dryWetMixer(mix_val, wet)
with {
  in_g = ba.db2linear(in_db) : si.smoo;
  out_g = ba.db2linear(out_db) : si.smoo;
  drv = ba.db2linear(drive_db) : si.smoo;
  comp = (1.0/drv) * ba.db2linear(15.6);
  inv_n = 1.0/116.0;

  ja_hyst(H) = (loop ~ _) : (!, _)
  with {
    loop(recM) = M_end, Mavg with {
      M1 = ja_lookup_m_end_k29_bias_mid(recM, H);
      s1 = ja_lookup_sum_m_rest_k29_bias_mid(recM, H);
      M2 = ja_lookup_m_end_k29_bias_mid(M1, H);
      s2 = ja_lookup_sum_m_rest_k29_bias_mid(M1, H);
      M3 = ja_lookup_m_end_k29_bias_mid(M2, H);
      s3 = ja_lookup_sum_m_rest_k29_bias_mid(M2, H);
      M_end = ja_lookup_m_end_k29_bias_mid(M3, H);
      s4 = ja_lookup_sum_m_rest_k29_bias_mid(M3, H);
      Mavg = (s1+s2+s3+s4) * inv_n;
    };
  };

  dc = fi.SVFTPT.HP2(5.0, 0.7071);
  lam = fi.spectral_tilt(3, 200, 15000, tilt);
  wet = _ * in_g : *(drv) : lam : ja_hyst : dc : *(comp) : *(out_g);
};

process = par(i, 2, fsm_channel(
  hgroup("JA", vslider("[0]In", 0, -24, 24, 0.1)),
  hgroup("JA", vslider("[1]Out", 0, -24, 24, 0.1)),
  hgroup("JA", vslider("[2]Drv", 0, -18, 29, 0.1)),
  hgroup("JA", vslider("[3]Mix", 1, 0, 1, 0.01)),
  hgroup("JA", vslider("[4]Tilt", 0, -0.1, 0.1, 0.001))));
