import("stdfaust.lib");

// Test: Complex substep with ba.if inside (like full prototype)

fast_tanh(x) = t * (27.0 + x2) / (27.0 + 9.0 * x2)
with { t = max(-3.0, min(3.0, x)); x2 = t * t; };

// Similar to full prototype's ja_substep
ja_substep(bias_offset) = ja_step
with {
  ja_step(M_prev, H_prev, H_audio, M_sum_prev) = M_sum_new, M_new, H_new, H_audio
  with {
    H_new = H_audio + 4.51 * bias_offset;
    dH = H_new - H_prev;
    He = H_new + 0.015 * M_prev;

    x_man = He / 2.25;
    Man_e = fast_tanh(x_man);
    Man_e2 = Man_e * Man_e;
    dMan_dH = (1.0 - Man_e2) / 2.25;

    // The ba.if that might be problematic
    dir = ba.if(dH >= 0.0, 1.0, -1.0);
    pin = dir * 0.875 - 0.015 * (Man_e - M_prev);
    inv_pin = 1.0 / (pin + 1e-6);

    denom = 1.0 - 0.18 * 0.015 * dMan_dH;
    inv_denom = 1.0 / (denom + 1e-9);
    dMdH = (0.18 * dMan_dH + (Man_e - M_prev) * inv_pin) * inv_denom;
    dM_step = dMdH * dH;

    M_unclamped = M_prev + dM_step;
    M_new = max(-1.0, min(1.0, M_unclamped));
    M_sum_new = M_sum_prev + M_new;
  };
};

// Substep with phase tracking (using ba.selector for tuple extraction)
ja_substep_with_phase(M_prev, H_prev, H_audio, M_sum_prev, phi, D) =
  M_sum_new, M_new, H_new, H_audio, phi + D, D
with {
  bias_offset = sin(phi);
  ja_result = (M_prev, H_prev, H_audio, M_sum_prev) : ja_substep(bias_offset);
  M_sum_new = ba.selector(0, 4, ja_result);
  M_new = ba.selector(1, 4, ja_result);
  H_new = ba.selector(2, 4, ja_result);
};

// Loops with seq
ja_loop24(M_prev, H_prev, H_audio, phi_b, dphi_) =
  M_prev, H_prev, H_audio, 0.0, phi0, D : seq(i, 24, ja_substep_with_phase)
  <: ba.selector(1, 6), ba.selector(2, 6), ba.selector(0, 6)
with { D = dphi_ / 24.0; phi0 = phi_b + 0.5 * D; };

ja_loop48(M_prev, H_prev, H_audio, phi_b, dphi_) =
  M_prev, H_prev, H_audio, 0.0, phi0, D : seq(i, 48, ja_substep_with_phase)
  <: ba.selector(1, 6), ba.selector(2, 6), ba.selector(0, 6)
with { D = dphi_ / 48.0; phi0 = phi_b + 0.5 * D; };

ja_loop60(M_prev, H_prev, H_audio, phi_b, dphi_) =
  M_prev, H_prev, H_audio, 0.0, phi0, D : seq(i, 60, ja_substep_with_phase)
  <: ba.selector(1, 6), ba.selector(2, 6), ba.selector(0, 6)
with { D = dphi_ / 60.0; phi0 = phi_b + 0.5 * D; };

bias_mode = nentry("Mode", 1, 0, 2, 1);
inv_24 = 1.0/24.0; inv_48 = 1.0/48.0; inv_60 = 1.0/60.0;

ja_hysteresis(H_in) =
    sum(i, 3, clk(i) * (clk(i) : ondemand(loop(i, H_in))))
with {
    mode = int(bias_mode + 0.5);
    clk(i) = (mode == i);

    loopK(H, K, inv_n) = (loop ~ (mem, mem)) : ba.selector(2, 3) : *(inv_n)
    with { loop(recM, recH) = recM, recH, H, 0.0, 6.28318 : K; };

    loop(0, H) = loopK(H, ja_loop24, inv_24);
    loop(1, H) = loopK(H, ja_loop48, inv_48);
    loop(2, H) = loopK(H, ja_loop60, inv_60);
};

process = _ : ja_hysteresis;
