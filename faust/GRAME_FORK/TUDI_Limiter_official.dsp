import("stdfaust.lib");

// ========================
// Tunnel diode "limiter"
// ========================

// --- Esaki model params (example values) ---
Vp = 50e-3; Ip = 5e-3;
Vv = 370e-3; Iv = 370e-6;
A2 = 8.0; Is = 1e-12; Vt = 25.85e-3;

// numeric safety
expClip(x) = exp(min(50.0, max(-50.0, x)));
signum(x)  = (x>0) - (x<0);

// diffusion current cap (prevents exp explosion at high voltage)
Idiff_max = 10e-3;  // 10mA cap - well above Ip, still reasonable
Iexcess_max = 50e-3; // cap the post-valley term (prevents huge currents if expClip saturates)

// |v| branch with capped thermal term
// Notes:
// - The "excess" (post-valley) term is gated to 0 below Vv to avoid a non-zero offset at v≈0.
// - This keeps esakiI(v) continuous around 0V (important for audio + the iterative solver).
esakiIpos(v) = tunnel + excess + thermal with {
  tunnel   = (v/Vp)*Ip*expClip(1.0 - (v/Vp));                                // tunnel current
  excess0  = Iv*max(0.0, expClip(A2*(v - Vv)) - 1.0);                        // excess current (starts at Vv)
  excess   = min(Iexcess_max, excess0);                                      // cap excess current
  thermal  = min(Idiff_max, Is*(expClip(v/Vt) - 1.0));                       // thermal diode current (capped)
};

// odd-symmetric extension (avoid DC offset)
esakiIraw(v) = signum(v) * esakiIpos(abs(v));

// Biasing trick (for limiter-like behavior):
// If you operate the symmetric shunt directly around 0V, the tunnel-diode's NDR region can
// produce an "expander-ish" mid-level slope (dI/dV < 0 => dVout/dVin > 1).
//
// By shifting the operating point to (at least) the valley voltage, you stay on the post-valley
// branch where dI/dV >= 0, so the shunt stage remains compressive/limiting instead of expanding.
// ========================
// UI Layout
// ========================
ui(x) = vgroup("[0] TUDI Limiter", x);
ui_row1(x) = ui(hgroup("[1]", x));
ui_row2(x) = ui(hgroup("[2]", x));

// --- Row 1: Main Controls ---
main_group(x) = ui_row1(hgroup("[0] Main", x));
input_dB = main_group(hslider("[0]Input [unit:dB][style:knob]", 0.0, -24.0, 24.0, 0.1));
drive_raw = main_group(hslider("[1]Drive [style:knob]", 0.1, 0.1, 1000.0, 0.1));
drive = drive_raw / 100.0;  // 0.001 to 10.0 V
makeup = main_group(hslider("[2]Makeup [style:knob]", 1.97, 0.0, 20.0, 0.01));
mix    = main_group(hslider("[3]Mix [style:knob]", 1.0, 0.0, 1.0, 0.01));
bypass = main_group(checkbox("[4]Bypass"));

// --- Row 1: Output Limiter ---
lim_group(x) = ui_row1(hgroup("[1] Output Limiter", x));
limiterOn  = lim_group(checkbox("[0]On"));
ceiling_dB = lim_group(hslider("[1]Ceiling [unit:dB][style:knob]", -0.1, -12.0, 0.0, 0.1));
lim_attack = lim_group(hslider("[2]Attack [unit:ms][style:knob]", 0.1, 0.1, 50.0, 0.1));
lim_hold   = lim_group(hslider("[3]Hold [unit:ms][style:knob]", 1.0, 0.0, 100.0, 0.1));
lim_rel    = lim_group(hslider("[4]Release [unit:ms][style:knob]", 65.0, 10.0, 500.0, 1.0));

// --- Row 2: Diode Model ---
diode_group(x) = ui_row2(hgroup("[0] Tunnel Diode", x));
R      = diode_group(hslider("[0]R Series [unit:Ω][style:knob]", 4.0, 1.0, 200.0, 1.0));
Rpar   = diode_group(hslider("[1]R Parallel [unit:Ω][style:knob]", 600.0, 5.0, 1000.0, 1.0));
biasV  = diode_group(hslider("[2]Bias [unit:V][style:knob]", 0.9, 0.0, 0.9, 0.01));
damp   = diode_group(hslider("[3]Damp [style:knob]", 0.6, 0.05, 1.0, 0.001));
rawNDR = diode_group(checkbox("[4]NDR (raw)"));

// Derived values
inputGain = ba.db2linear(input_dB);
biasEff  = max(Vv + 1e-6, biasV);
esakiIbiased(v) = signum(v) * (esakiIpos(biasEff + abs(v)) - esakiIpos(biasEff));
esakiI(v) = rawNDR*esakiIraw(v) + (1.0 - rawNDR)*esakiIbiased(v);

driveEff = max(1e-6, drive);
RparEff  = max(1e-6, Rpar);

// shunt element current = tunnel diode + parallel stabilizer resistor
Itotal(v) = esakiI(v) + (v / RparEff);

// Keep the iterative solver bounded without changing the small-signal equation
Vlim = 5.0;
clampV(x) = max(-Vlim, min(Vlim, x));

// Damped fixed-point update for Vin = Vd + R*Itotal(Vd)
// equivalent to relaxed Picard iteration on the implicit equation
g(vin, x) = clampV((1.0 - damp)*x + damp*(vin - R*Itotal(x)));

// --- Explicit iteration unrolls ---
// N = 4, 5, 6

iter4(vin, x0) = x4 with {
  x1 = g(vin, x0);
  x2 = g(vin, x1);
  x3 = g(vin, x2);
  x4 = g(vin, x3);
};

iter5(vin, x0) = x5 with {
  x1 = g(vin, x0);
  x2 = g(vin, x1);
  x3 = g(vin, x2);
  x4 = g(vin, x3);
  x5 = g(vin, x4);
};

iter6(vin, x0) = x6 with {
  x1 = g(vin, x0);
  x2 = g(vin, x1);
  x3 = g(vin, x2);
  x4 = g(vin, x3);
  x5 = g(vin, x4);
  x6 = g(vin, x5);
};

// --------- CHOOSE N HERE ---------
iterN(vin, x0) = iter4(vin, x0);  // change to iter2 / iter4 / iter6 and recompile
// ---------------------------------

// Recursive solve: vd[n] from vd[n-1] (1-sample state) [web:72][web:112]
solveVd(vin) = vd letrec { 'vd = iterN(vin, vd); };

// Volts-domain in / out rescaled back to audio
dc_blocker = fi.SVFTPT.HP2(10.0, 0.74);

wet = _ : *(driveEff) : solveVd : dc_blocker : *(makeup/driveEff);

// Dry/wet blend (input gain applied before split so both paths get it)
blended = _ : *(inputGain) <: (_, wet) : (*(1-mix), *(mix)) : +;

// Lookahead limiter (co.limiter_lad_mono: lookahead, ceiling, attack, hold, release)
outLimiter = co.limiter_lad_mono(
    0.005,                          // 5ms lookahead
    ba.db2linear(ceiling_dB),       // ceiling in linear
    lim_attack / 1000.0,            // attack in seconds
    lim_hold / 1000.0,              // hold in seconds
    lim_rel / 1000.0                // release in seconds
);

// Channel: blend -> optional limiter
channel = blended <: (_, outLimiter) : select2(limiterOn);

// Stereo + bypass A/B
process = par(i, 2,
  _ <: (_, channel) : select2(bypass)
);
