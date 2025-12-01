import("stdfaust.lib");
// Test: Gated substeps with ondemand (Option B from CURRENT_STATUS.md)
// Goal: Only active substeps execute, matching C++ scheduler behavior
//
// C++ reference (JAHysteresisScheduler.cpp lines 62-99):
//   K60 Normal: 3 cycles × 22 substeps/cycle = 66 steps per sample
//   K60 Ultra:  3 cycles × 24 substeps/cycle = 72 steps per sample
//
// This prototype uses ondemand to gate individual substeps so only
// steps_this_sample (66 or 72) actually compute.

// ===== Constants =====
// Full K60 Ultra: 3 cycles × 24 substeps = 72 steps per sample
MAX_STEPS = 72;

// K60 Ultra mode
bias_cycles_per_sample = 3.0;
substeps_per_cycle = 24;
steps_this_sample = 72;

// ===== User controls =====
input_gain  = hslider("Input Gain [dB]", 0.0, -24.0, 24.0, 0.1) : ba.db2linear : si.smoo;
output_gain = hslider("Output Gain [dB]", 34.0, -24.0, 48.0, 0.1) : ba.db2linear : si.smoo;
drive_db    = hslider("Drive [dB]", -10.0, -18.0, 18.0, 0.1);
drive_gain  = drive_db : si.smoo : ba.db2linear;
bias_level  = hslider("Bias Level", 0.41, 0.0, 1.0, 0.01) : si.smoo;
bias_scale  = hslider("Bias Scale", 11.0, 1.0, 100.0, 0.1) : si.smoo;
mix = hslider("Mix [Dry->Wet]", 1.0, 0.0, 1.0, 0.01) : si.smoo;

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

fast_tanh(x) = t * (27.0 + x2) / (27.0 + 9.0 * x2)
with { t = max(-3.0, min(3.0, x)); x2 = t * t; };

// ===== Phase tracking (matching C++ JAHysteresisScheduler) =====
// Phase advances by (biasCycles × 2π) per sample = 3 × 2π = 6π
// Each substep advances by substepPhase = 2π / substepsPerCycle

two_pi = 2.0 * ma.PI;
substep_phase = two_pi / substeps_per_cycle;  // phase increment per substep

// Phase accumulator: advances by (biasCycles * 2π) per sample, wrapped to [0, 2π)
phase_inc_per_sample = bias_cycles_per_sample * two_pi;
bias_phase = phase_inc_per_sample : (+ ~ _) : ma.frac : *(two_pi);
// Phase at START of this sample (before any substeps)
bias_phase_start = bias_phase @ 1;

// ===== JA substep physics (the expensive part) =====
ja_physics(M_prev, H_prev, H_audio, phase) = M_new, H_new
with {
    // C++ uses midpoint: sin(phase + substepPhase * 0.5)
    bias_offset = sin(phase + 0.5 * substep_phase);
    H_new = H_audio + bias_amp * bias_offset;
    dH = H_new - H_prev;
    He = H_new + alpha_norm * M_prev;

    x_man = He * inv_a_norm;
    Man_e = fast_tanh(x_man);
    Man_e2 = Man_e * Man_e;
    dMan_dH = (1.0 - Man_e2) * inv_a_norm;

    dir = select2(dH >= 0.0, 0.0 - 1.0, 1.0);
    pin = dir * k_norm - alpha_norm * (Man_e - M_prev);
    inv_pin = 1.0 / (pin + sigma);

    denom = 1.0 - c_norm * alpha_norm * dMan_dH;
    inv_denom = 1.0 / (denom + 1e-9);
    dMdH = (c_norm * dMan_dH + (Man_e - M_prev) * inv_pin) * inv_denom;

    M_unclamped = M_prev + dMdH * dH;
    M_new = max(-1.0, min(1.0, M_unclamped));
};

// ===== Gated substep using ondemand =====
// When clk=1: compute JA physics
// When clk=0: ondemand outputs 0, we use pass-through values
// KEY FIX: Use compile-time i parameter instead of runtime idx counter
gated_substep(i, M_prev, H_prev, H_audio, M_sum, phase) =
    M_out, H_out, H_audio, M_sum_out, phase + substep_phase
with {
    // i is compile-time constant from seq - this is what makes ondemand work!
    clk = (i < steps_this_sample);

    // ondemand computes ja_physics only when clk=1
    physics_result = clk : ondemand(ja_physics(M_prev, H_prev, H_audio, phase));
    M_computed = ba.selector(0, 2, physics_result);
    H_computed = ba.selector(1, 2, physics_result);

    // Output: use computed values when active, pass-through when inactive
    M_out = select2(clk, M_prev, M_computed);
    H_out = select2(clk, H_prev, H_computed);
    M_sum_out = select2(clk, M_sum, M_sum + M_computed);
};

// ===== Main loop =====
// Chain MAX_STEPS gated substeps, only steps_this_sample will actually compute
ja_hysteresis(H_in) = (loop ~ (mem, mem)) : (!, !, _)
with {
    // Feedback loop: chain all substeps, output (M_end, H_end, Mavg)
    loop(recM, recH) = M_end, H_end, Mavg
    with {
        // Run through all gated substeps
        // Start at bias_phase_start (phase carried from previous sample)
        // KEY: seq passes compile-time i to gated_substep(i)
        result = recM, recH, H_in, 0.0, bias_phase_start
            : seq(i, MAX_STEPS, gated_substep(i));

        // Extract final state (5 outputs: M, H, H_audio, M_sum, phase)
        M_end = ba.selector(0, 5, result);
        H_end = ba.selector(1, 5, result);
        M_sum = ba.selector(3, 5, result);

        // Average magnetization (divide by actual steps taken)
        Mavg = M_sum / float(steps_this_sample);
    };
};

// ===== Tape stage with gain structure =====
tape_stage(x) =
    x * input_gain
    : *(drive_gain)
    : ja_hysteresis
    : fi.dcblockerat(10);

wet_gained = tape_stage : *(output_gain);

// Dry/wet mix
tape_channel = ef.dryWetMixer(mix, wet_gained);

// Stereo output
process = par(i, 2, tape_channel);
