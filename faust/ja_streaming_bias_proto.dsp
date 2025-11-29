import("stdfaust.lib");
import("ja_lut_k32.lib");   // 2D LUT for K32 substeps 1..35 (lofi)
import("ja_lut_k60.lib");   // 2D LUT for K60 substeps 1..65
import("ja_lut_k120.lib");  // 2D LUT for K120 substeps 1..131
import("ja_lut_k240.lib");  // 2D LUT for K240 substeps 1..263
import("ja_lut_k480.lib");  // 2D LUT for K480 substeps 1..527
import("ja_lut_k960.lib");  // 2D LUT for K960 substeps 1..1055
import("ja_lut_k1920.lib"); // 2D LUT for K1920 substeps 1..2111 (ultra)

// Streaming JA hysteresis prototype with phase-locked bias oscillator.
// LUT-optimized: 1 real substep + 2D LUT lookup for remainder
// LUTs precomputed for bias_level=0.41, bias_scale=11.0

// ===== Physics parameters (fixed for prototype) =====
Ms              = 320.0;
a_density       = 720.0;
k_pinning       = 280.0;
c_reversibility = 0.18;
alpha_coupling  = 0.015;

// ===== User controls =====
input_gain  = hslider("Input Gain [dB]", 0.0, -24.0, 24.0, 0.1) : ba.db2linear : si.smoo;
output_gain = hslider("Output Gain [dB]", 15.9, -24.0, 48.0, 0.1) : ba.db2linear : si.smoo;
drive_db    = hslider("Drive [dB]", 0.0, -18.0, 18.0, 0.1);
drive_gain  = drive_db : si.smoo : ba.db2linear;

bias_mode = nentry("Bias Mode [style:menu{'K32 LoFi':0;'K60':1;'K120':2;'K240':3;'K480':4;'K960':5;'K1920 Ultra':6}]", 5, 0, 6, 1);

mix = hslider("Mix [Dry->Wet]", 1.0, 0.0, 1.0, 0.01) : si.smoo;

// ===== Derived constants (fixed bias for LUT) =====
Ms_safe    = ba.if(Ms > 1e-6, Ms, 1e-6);
alpha_norm = alpha_coupling;
a_norm     = a_density / Ms_safe;
k_norm     = k_pinning / Ms_safe;
c_norm     = c_reversibility;
bias_amp   = 0.41 * 11.0;  // Fixed for LUT compatibility

// ===== Precomputed bias lookup tables =====
// K32: 2 cycles = 4pi, 36 substeps (lofi character)
tablesize_36 = 36;
dphi_36 = 4.0 * ma.PI / tablesize_36;
bias_gen_36(n) = sin((float(ba.period(n)) + 0.5) * dphi_36);
bias_lut_36(idx) = rdtable(tablesize_36, bias_gen_36(tablesize_36), int(idx));

// K60: 3 cycles = 6pi, 66 substeps
tablesize_66 = 66;
dphi_66 = 6.0 * ma.PI / tablesize_66;
bias_gen_66(n) = sin((float(ba.period(n)) + 0.5) * dphi_66);
bias_lut_66(idx) = rdtable(tablesize_66, bias_gen_66(tablesize_66), int(idx));

// K120: 6 cycles = 12pi, 132 substeps
tablesize_132 = 132;
dphi_132 = 12.0 * ma.PI / tablesize_132;
bias_gen_132(n) = sin((float(ba.period(n)) + 0.5) * dphi_132);
bias_lut_132(idx) = rdtable(tablesize_132, bias_gen_132(tablesize_132), int(idx));

// K240: 12 cycles = 24pi, 264 substeps
tablesize_264 = 264;
dphi_264 = 24.0 * ma.PI / tablesize_264;
bias_gen_264(n) = sin((float(ba.period(n)) + 0.5) * dphi_264);
bias_lut_264(idx) = rdtable(tablesize_264, bias_gen_264(tablesize_264), int(idx));

// K480: 24 cycles = 48pi, 528 substeps
tablesize_528 = 528;
dphi_528 = 48.0 * ma.PI / tablesize_528;
bias_gen_528(n) = sin((float(ba.period(n)) + 0.5) * dphi_528);
bias_lut_528(idx) = rdtable(tablesize_528, bias_gen_528(tablesize_528), int(idx));

// K960: 48 cycles = 96pi, 1056 substeps
tablesize_1056 = 1056;
dphi_1056 = 96.0 * ma.PI / tablesize_1056;
bias_gen_1056(n) = sin((float(ba.period(n)) + 0.5) * dphi_1056);
bias_lut_1056(idx) = rdtable(tablesize_1056, bias_gen_1056(tablesize_1056), int(idx));

// K1920: 96 cycles = 192pi, 2112 substeps (ultra resolution)
tablesize_2112 = 2112;
dphi_2112 = 192.0 * ma.PI / tablesize_2112;
bias_gen_2112(n) = sin((float(ba.period(n)) + 0.5) * dphi_2112);
bias_lut_2112(idx) = rdtable(tablesize_2112, bias_gen_2112(tablesize_2112), int(idx));

sigma       = 1e-6;
inv_36      = 1.0 / 36.0;
inv_66      = 1.0 / 66.0;
inv_132     = 1.0 / 132.0;
inv_264     = 1.0 / 264.0;
inv_528     = 1.0 / 528.0;
inv_1056    = 1.0 / 1056.0;
inv_2112    = 1.0 / 2112.0;
inv_a_norm  = 1.0 / a_norm;

// ===== Real tanh (we can afford it now with LUT optimization) =====
fast_tanh(x) = ma.tanh(x);

// ===== Generic substep 0 (parameterized by bias LUT) =====
ja_substep0(bias_val, M_prev, H_prev, H_audio) = M1, H1
with {
  H1 = H_audio + bias_amp * bias_val;
  dH = H1 - H_prev;
  He = H1 + alpha_norm * M_prev;

  x_man   = He * inv_a_norm;
  Man_e   = fast_tanh(x_man);
  Man_e2  = Man_e * Man_e;
  dMan_dH = (1.0 - Man_e2) * inv_a_norm;

  dir      = ba.if(dH >= 0.0, 1.0, -1.0);
  pin      = dir * k_norm - alpha_norm * (Man_e - M_prev);
  inv_pin  = 1.0 / (pin + sigma);

  denom     = 1.0 - c_norm * alpha_norm * dMan_dH;
  inv_denom = 1.0 / (denom + 1e-9);
  dMdH      = (c_norm * dMan_dH + (Man_e - M_prev) * inv_pin) * inv_denom;
  dM_step   = dMdH * dH;

  M_unclamped = M_prev + dM_step;
  M1          = max(-1.0, min(1.0, M_unclamped));
};

// ===== K32 LUT loop (lofi) =====
ja_loop_k32(M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
  M1_H1 = ja_substep0(bias_lut_36(0), M_prev, H_prev, H_audio);
  M1 = ba.selector(0, 2, M1_H1);
  M_end = ja_lookup_m_end_k32(M1, H_audio);
  sumM_rest = ja_lookup_sum_m_rest_k32(M1, H_audio);
  Mavg = (M1 + sumM_rest) * inv_36;
  H_end = H_audio + bias_amp * bias_lut_36(35);
};

// ===== K60 LUT loop =====
ja_loop_k60(M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
  M1_H1 = ja_substep0(bias_lut_66(0), M_prev, H_prev, H_audio);
  M1 = ba.selector(0, 2, M1_H1);
  M_end = ja_lookup_m_end_k60(M1, H_audio);
  sumM_rest = ja_lookup_sum_m_rest_k60(M1, H_audio);
  Mavg = (M1 + sumM_rest) * inv_66;
  H_end = H_audio + bias_amp * bias_lut_66(65);
};

// ===== K120 LUT loop =====
ja_loop_k120(M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
  M1_H1 = ja_substep0(bias_lut_132(0), M_prev, H_prev, H_audio);
  M1 = ba.selector(0, 2, M1_H1);
  M_end = ja_lookup_m_end_k120(M1, H_audio);
  sumM_rest = ja_lookup_sum_m_rest_k120(M1, H_audio);
  Mavg = (M1 + sumM_rest) * inv_132;
  H_end = H_audio + bias_amp * bias_lut_132(131);
};

// ===== K240 LUT loop =====
ja_loop_k240(M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
  M1_H1 = ja_substep0(bias_lut_264(0), M_prev, H_prev, H_audio);
  M1 = ba.selector(0, 2, M1_H1);
  M_end = ja_lookup_m_end_k240(M1, H_audio);
  sumM_rest = ja_lookup_sum_m_rest_k240(M1, H_audio);
  Mavg = (M1 + sumM_rest) * inv_264;
  H_end = H_audio + bias_amp * bias_lut_264(263);
};

// ===== K480 LUT loop =====
ja_loop_k480(M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
  M1_H1 = ja_substep0(bias_lut_528(0), M_prev, H_prev, H_audio);
  M1 = ba.selector(0, 2, M1_H1);
  M_end = ja_lookup_m_end_k480(M1, H_audio);
  sumM_rest = ja_lookup_sum_m_rest_k480(M1, H_audio);
  Mavg = (M1 + sumM_rest) * inv_528;
  H_end = H_audio + bias_amp * bias_lut_528(527);
};

// ===== K960 LUT loop =====
ja_loop_k960(M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
  M1_H1 = ja_substep0(bias_lut_1056(0), M_prev, H_prev, H_audio);
  M1 = ba.selector(0, 2, M1_H1);
  M_end = ja_lookup_m_end_k960(M1, H_audio);
  sumM_rest = ja_lookup_sum_m_rest_k960(M1, H_audio);
  Mavg = (M1 + sumM_rest) * inv_1056;
  H_end = H_audio + bias_amp * bias_lut_1056(1055);
};

// ===== K1920 LUT loop (ultra) =====
ja_loop_k1920(M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
  M1_H1 = ja_substep0(bias_lut_2112(0), M_prev, H_prev, H_audio);
  M1 = ba.selector(0, 2, M1_H1);
  M_end = ja_lookup_m_end_k1920(M1, H_audio);
  sumM_rest = ja_lookup_sum_m_rest_k1920(M1, H_audio);
  Mavg = (M1 + sumM_rest) * inv_2112;
  H_end = H_audio + bias_amp * bias_lut_2112(2111);
};

// ===== Streaming JA hysteresis with mode selection =====
ja_hysteresis(H_in) =
  ba.if(bias_mode < 0.5, loopK32(H_in),
  ba.if(bias_mode < 1.5, loopK60(H_in),
  ba.if(bias_mode < 2.5, loopK120(H_in),
  ba.if(bias_mode < 3.5, loopK240(H_in),
  ba.if(bias_mode < 4.5, loopK480(H_in),
  ba.if(bias_mode < 5.5, loopK960(H_in),
                         loopK1920(H_in)))))))
with {
  loopK32(H) = (loop ~ (mem, mem)) : ba.selector(2, 3)
  with { loop(recM, recH) = recM, recH, H : ja_loop_k32; };

  loopK60(H) = (loop ~ (mem, mem)) : ba.selector(2, 3)
  with { loop(recM, recH) = recM, recH, H : ja_loop_k60; };

  loopK120(H) = (loop ~ (mem, mem)) : ba.selector(2, 3)
  with { loop(recM, recH) = recM, recH, H : ja_loop_k120; };

  loopK240(H) = (loop ~ (mem, mem)) : ba.selector(2, 3)
  with { loop(recM, recH) = recM, recH, H : ja_loop_k240; };

  loopK480(H) = (loop ~ (mem, mem)) : ba.selector(2, 3)
  with { loop(recM, recH) = recM, recH, H : ja_loop_k480; };

  loopK960(H) = (loop ~ (mem, mem)) : ba.selector(2, 3)
  with { loop(recM, recH) = recM, recH, H : ja_loop_k960; };

  loopK1920(H) = (loop ~ (mem, mem)) : ba.selector(2, 3)
  with { loop(recM, recH) = recM, recH, H : ja_loop_k1920; };
};

// ===== Prototype tape stage =====
dc_blocker = fi.SVFTPT.HP2(10.0, 0.7071);

tape_stage(x) =
  x * input_gain
  : *(drive_gain)
  : ja_hysteresis
  : dc_blocker;

wet_gained = tape_stage : *(output_gain);
tape_channel = ef.dryWetMixer(mix, wet_gained);

process = tape_channel, tape_channel;
