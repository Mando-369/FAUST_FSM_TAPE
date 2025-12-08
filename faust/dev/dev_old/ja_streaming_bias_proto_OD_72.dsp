import("stdfaust.lib");
// JA Hysteresis with 72 substeps (K60 Ultra) using ondemand gating
// Based on working test_gated_substeps.dsp pattern

// ===== User controls =====
input_gain  = hslider("Input Gain [dB]", 0.0, -24.0, 24.0, 0.1) : ba.db2linear : si.smoo;
output_gain = hslider("Output Gain [dB]", 12.6, -24.0, 48.0, 0.1) : ba.db2linear : si.smoo;
drive_db    = hslider("Drive [dB]", 0.0, -18.0, 18.0, 0.1);
drive_gain  = drive_db : si.smoo : ba.db2linear;
bias_level  = hslider("Bias Level", 0.3, 0.0, 1.0, 0.01) : si.smoo;
bias_scale  = hslider("Bias Scale", 11.0, 1.0, 100.0, 0.1) : si.smoo;
mix = hslider("Mix [Dry->Wet]", 1.0, 0.0, 1.0, 0.01) : si.smoo;

// ===== Constants =====
// K60 Ultra: 3 cycles × 24 substeps = 72 steps per sample
SUBSTEPS = 72;
bias_cycles = 3.0;

// ===== Physics parameters =====
Ms              = 320.0;
a_density       = 720.0;
k_pinning       = 280.0;
c_reversibility = 0.18;
alpha_coupling  = 0.015;

Ms_safe    = ba.if(Ms > 1e-6, Ms, 1e-6);
alpha_norm = alpha_coupling;
a_norm     = a_density / Ms_safe;
k_norm     = k_pinning / Ms_safe;
c_norm     = c_reversibility;
bias_amp   = bias_level * bias_scale;

sigma = 1e-6;
inv_a_norm = 1.0 / a_norm;
inv_substeps = 1.0 / float(SUBSTEPS);

// ===== Phase tracking =====
two_pi = 2.0 * ma.PI;
substep_phase = two_pi / float(SUBSTEPS / bias_cycles);  // phase per substep

phase_inc_per_sample = bias_cycles * two_pi;
bias_phase = phase_inc_per_sample : (+ ~ _) : ma.frac : *(two_pi);
bias_phase_start = bias_phase @ 1;

// ===== Fast tanh =====
fast_tanh(x) = t * (27.0 + x2) / (27.0 + 9.0 * x2)
with { t = max(-3.0, min(3.0, x)); x2 = t * t; };

// ===== JA physics =====
ja_physics(M_prev, H_prev, H_audio, phase) = M_new, H_new
with {
    bias_offset = sin(phase + 0.5 * substep_phase);
    H_new = H_audio + bias_amp * bias_offset;
    dH = H_new - H_prev;
    He = H_new + alpha_norm * M_prev;

    x_man = He * inv_a_norm;
    Man_e = fast_tanh(x_man);
    Man_e2 = Man_e * Man_e;
    dMan_dH = (1.0 - Man_e2) * inv_a_norm;

    dir = select2(dH >= 0.0, -1.0, 1.0);
    pin = dir * k_norm - alpha_norm * (Man_e - M_prev);
    inv_pin = 1.0 / (pin + sigma);

    denom = 1.0 - c_norm * alpha_norm * dMan_dH;
    inv_denom = 1.0 / (denom + 1e-9);
    dMdH = (c_norm * dMan_dH + (Man_e - M_prev) * inv_pin) * inv_denom;

    M_unclamped = M_prev + dMdH * dH;
    M_new = max(-1.0, min(1.0, M_unclamped));
};

// ===== Gated substep (compile-time i from seq) =====
gated_substep(i, M_prev, H_prev, H_audio, M_sum, phase) =
    M_out, H_out, H_audio, M_sum_out, phase + substep_phase
with {
    clk = (i < SUBSTEPS);  // compile-time!
    physics_result = clk : ondemand(ja_physics(M_prev, H_prev, H_audio, phase));
    M_computed = ba.selector(0, 2, physics_result);
    H_computed = ba.selector(1, 2, physics_result);

    M_out = select2(clk, M_prev, M_computed);
    H_out = select2(clk, H_prev, H_computed);
    M_sum_out = select2(clk, M_sum, M_sum + M_computed);
};

// ===== Main JA loop =====
ja_hysteresis(H_in) = (loop ~ (mem, mem)) : (!, !, _)
with {
    loop(recM, recH) = M_end, H_end, Mavg
    with {
        result = recM, recH, H_in, 0.0, bias_phase_start
            : seq(i, SUBSTEPS, gated_substep(i));

        M_end = ba.selector(0, 5, result);
        H_end = ba.selector(1, 5, result);
        M_sum = ba.selector(3, 5, result);
        Mavg = M_sum * inv_substeps;
    };
};

// ===== Tape stage =====
dc_blocker = fi.SVFTPT.HP2(10.0, 0.7071);
drive_comp = 1.0 / drive_gain;  // Compensate: +6dB drive → -6dB output

// 1 second fade-in to avoid startup noise
startup_fade = min(1.0, float(ba.time) / float(ma.SR));

tape_stage(x) =
    x * input_gain
    : *(drive_gain)
    : ja_hysteresis
    : dc_blocker
    : *(drive_comp)
    : *(startup_fade);

wet_gained = tape_stage : *(output_gain);
tape_channel = ef.dryWetMixer(mix, wet_gained);

process = par(i, 2, tape_channel);
