import("stdfaust.lib");
// Streaming JA hysteresis prototype with running bias oscillator.

// ===== Physics parameters (fixed for prototype) =====
Ms              = 320.0;
a_density       = 720.0;
k_pinning       = 280.0;
c_reversibility = 0.18;
alpha_coupling  = 0.015;

// ===== User controls =====
input_gain  = hslider("Input Gain [dB]", 0.0, -24.0, 24.0, 0.1) : ba.db2linear : si.smoo;
output_gain = hslider("Output Gain [dB]", 34.0, -24.0, 48.0, 0.1) : ba.db2linear : si.smoo;
drive_db    = hslider("Drive [dB]", -10.0, -18.0, 18.0, 0.1);
drive_gain  = drive_db : si.smoo : ba.db2linear;

bias_level      = hslider("Bias Level", 0.4, 0.0, 1.0, 0.01) : si.smoo;
bias_scale      = hslider("Bias Scale", 11.0, 1.0, 100.0, 0.1) : si.smoo;
bias_mode = nentry("Bias Mode [style:menu{'K24 LoFi':0;'K48 Standard':1;'K60 HQ':2}]", 1, 0, 2, 1);
bias_ratio = hslider("Bias Ratio", 1.0, 0.98, 1.02, 0.001) : si.smoo;

mix = hslider("Mix [Dry->Wet]", 1.0, 0.0, 1.0, 0.01) : si.smoo;

// ===== Derived constants =====
Ms_safe    = ba.if(Ms > 1e-6, Ms, 1e-6);
alpha_norm = alpha_coupling;
a_norm     = a_density / Ms_safe;
k_norm     = k_pinning / Ms_safe;
c_norm     = c_reversibility;
bias_amp   = bias_level * bias_scale;

two_pi     = 2.0 * ma.PI;
inv_two_pi = 1.0 / two_pi;

// Phase oscillator for a given cycle count
// Returns (phi_start, phase_span)
make_phase_osc(cycles) = phi_start, phase_span
with {
  bias_freq   = cycles * ma.SR * bias_ratio;
  phase_inc   = two_pi * bias_freq / ma.SR;
  phi_unwrap  = phase_inc : (+ ~ _);
  phi_wrapped = two_pi * ma.frac(phi_unwrap * inv_two_pi);
  phi_start   = phi_wrapped @ 1;
  phase_span  = phase_inc;
};

// Phase oscillators for each mode (K24=2 cycles, K48/K60=3 cycles)
// Use ba.selector to extract individual outputs
phase_osc_k24 = make_phase_osc(2.0);
phi_k24  = ba.selector(0, 2, phase_osc_k24);
dphi_k24 = ba.selector(1, 2, phase_osc_k24);

phase_osc_k48 = make_phase_osc(3.0);
phi_k48  = ba.selector(0, 2, phase_osc_k48);
dphi_k48 = ba.selector(1, 2, phase_osc_k48);

phase_osc_k60 = make_phase_osc(3.0);
phi_k60  = ba.selector(0, 2, phase_osc_k60);
dphi_k60 = ba.selector(1, 2, phase_osc_k60);

sigma           = 1e-6;

// ===== Fast tanh approximation =====
fast_tanh(x) = t * (27.0 + x2) / (27.0 + 9.0 * x2)
with {
  t  = max(-3.0, min(3.0, x));
  x2 = t * t;
};

// ===== Precompute constants =====
inv_24 = 1.0 / 24.0;
inv_48 = 1.0 / 48.0;
inv_60 = 1.0 / 60.0;
inv_a_norm = 1.0 / a_norm;

// ===== Core JA step driven by current bias sample =====
ja_substep(bias_offset) = ja_step
with {
  ja_step(M_prev, H_prev, H_audio, M_sum_prev) = M_sum_new, M_new, H_new, H_audio
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
    M_sum_new   = M_sum_prev + M_new;
  };
};

// ===== Substep with phase tracking =====
// Input:  (M_prev, H_prev, H_audio, M_sum_prev, phi, D)
// Output: (M_sum_new, M_new, H_new, H_audio, phi+D, D)
ja_substep_with_phase(M_prev, H_prev, H_audio, M_sum_prev, phi, D) =
  M_sum_new, M_new, H_new, H_audio, phi + D, D
with {
  bias_offset = sin(phi);
  ja_result   = (M_prev, H_prev, H_audio, M_sum_prev) : ja_substep(bias_offset);
  M_sum_new   = ba.selector(0, 4, ja_result);
  M_new       = ba.selector(1, 4, ja_result);
  H_new       = ba.selector(2, 4, ja_result);
};

// ===== Loop helpers (using seq) =====
// Output order: (M, H, M_sum) so feedback gets (M, H) and we select M_sum at index 2
ja_loop24(M_prev, H_prev, H_audio, phi_b, dphi_) =
  M_prev, H_prev, H_audio, 0.0, phi0, D : seq(i, 24, ja_substep_with_phase)
  <: ba.selector(1, 6), ba.selector(2, 6), ba.selector(0, 6)
with {
  D    = dphi_ / 24.0;
  phi0 = phi_b + 0.5 * D;  // midpoint sampling
};

ja_loop48(M_prev, H_prev, H_audio, phi_b, dphi_) =
  M_prev, H_prev, H_audio, 0.0, phi0, D : seq(i, 48, ja_substep_with_phase)
  <: ba.selector(1, 6), ba.selector(2, 6), ba.selector(0, 6)
with {
  D    = dphi_ / 48.0;
  phi0 = phi_b + 0.5 * D;  // midpoint sampling
};

ja_loop60(M_prev, H_prev, H_audio, phi_b, dphi_) =
  M_prev, H_prev, H_audio, 0.0, phi0, D : seq(i, 60, ja_substep_with_phase)
  <: ba.selector(1, 6), ba.selector(2, 6), ba.selector(0, 6)
with {
  D    = dphi_ / 60.0;
  phi0 = phi_b + 0.5 * D;  // midpoint sampling
};

// ===== Streaming JA hysteresis with ondemand mode selection =====
// Only the active mode branch computes (massive CPU savings)
ja_hysteresis(H_in) =
    sum(i, 3,
        clk(i) * (clk(i) : ondemand(loop(i, H_in)))
    )
with {
    // Quantize bias_mode to integer 0..2
    mode = int(bias_mode + 0.5);

    // Clock for branch i (1 if active, 0 otherwise)
    clk(i) = (mode == i);

    // Generic loop wrapper: feedback + output selection + normalization
    // K returns (M, H, M_sum), feedback takes (M, H), we select M_sum at index 2
    loopK(H, K, inv_n, phi_s, dphi) = (loop ~ (mem, mem))
        : ba.selector(2, 3)
        : *(inv_n)
    with {
        loop(recM, recH) = recM, recH, H, phi_s, dphi : K;
    };

    // Mode-indexed loop selection with per-mode phase oscillators
    loop(0, H) = loopK(H, ja_loop24, inv_24, phi_k24, dphi_k24);
    loop(1, H) = loopK(H, ja_loop48, inv_48, phi_k48, dphi_k48);
    loop(2, H) = loopK(H, ja_loop60, inv_60, phi_k60, dphi_k60);
};

// ===== Prototype tape stage (no limiter/clipper) =====
tape_stage(x) =
  x * input_gain
  : *(drive_gain)
  : ja_hysteresis
  : fi.dcblockerat(10);

wet_gained = tape_stage : *(output_gain);

// Dry/wet mix for quick listening tests.
tape_channel = ef.dryWetMixer(mix, wet_gained);

process = par(i, 2, tape_channel);
 
