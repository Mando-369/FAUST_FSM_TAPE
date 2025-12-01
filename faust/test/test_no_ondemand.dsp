import("stdfaust.lib");

// Simple test: seq without ondemand

fast_tanh(x) = t * (27.0 + x2) / (27.0 + 9.0 * x2)
with {
  t  = max(-3.0, min(3.0, x));
  x2 = t * t;
};

// Substep with phase tracking (6 in, 6 out)
ja_substep_with_phase(M_prev, H_prev, H_audio, M_sum_prev, phi, D) =
  M_sum_new, M_new, H_new, H_audio, phi + D, D
with {
  bias_offset = sin(phi);
  H_new = H_audio + 4.51 * bias_offset;
  dH = H_new - H_prev;
  He = H_new + 0.015 * M_prev;
  Man_e = fast_tanh(He / 2.25);
  dMdH = (1.0 - Man_e * Man_e) / 2.25;
  M_new = max(-1.0, min(1.0, M_prev + dMdH * dH));
  M_sum_new = M_sum_prev + M_new;
};

// Loop with seq - NO ondemand
ja_loop24(M_prev, H_prev, H_audio, phi_b, dphi_) =
  M_prev, H_prev, H_audio, 0.0, phi0, D : seq(i, 24, ja_substep_with_phase)
  <: ba.selector(1, 6), ba.selector(2, 6), ba.selector(0, 6)
with {
  D = dphi_ / 24.0;
  phi0 = phi_b + 0.5 * D;
};

// Simple feedback loop - NO ondemand
process_loop(H) = (loop ~ (mem, mem)) : ba.selector(2, 3) : *(1.0/24.0)
with {
  loop(recM, recH) = recM, recH, H, 0.0, 6.28318 : ja_loop24;
};

process = _ : process_loop;
