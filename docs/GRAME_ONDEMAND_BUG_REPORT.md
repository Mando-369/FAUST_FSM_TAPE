# Ondemand Primitive Bug Report for GRAME

**Date**: 2025-11-30 (Updated: 2025-12-01)
**Reporter**: Thomas Mandolini (OmegaDSP)
**Faust Fork**: `master-dev-ocpp-od-fir-2-FIR13` (ondemand branch)
**Repository**: https://github.com/grame-cncm/faust (dev fork)

---

## Summary

Two issues were discovered with the `ondemand` primitive. **One was user error** (now resolved), **one is a real bug**:

| Issue | Description | Status |
|-------|-------------|--------|
| Compiler hang | `seq + runtime idx counter + ondemand` | **User error** - fixed |
| Undeclared vars | `runtime clock + complex seq inside ondemand` | **Real bug** |

The real bug: `ondemand` generates invalid C++ code (undeclared `fTempXXSE` variables) when wrapping complex `seq(i, N, ...)` chains with a **runtime clock signal**, even though the same runtime clock works fine with simpler operations inside.

---

## Environment

- **macOS**: Darwin 24.6.0 (M4 Max)
- **Faust fork**: `tools/faust-ondemand/` (commit from master-dev-ocpp-od-fir-2-FIR13)
- **Compile flags**: `-double`
- **Target**: JUCE AU plugin via `faust2juce`

---

## Working vs Failing Case

### Working: LUT-based prototype

**File**: `faust/test/ja_streaming_bias_proto_od.dsp`

```faust
// Simple operations inside ondemand - LUT lookups
ja_loop_k28(M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
  M1_H1 = ja_substep0(bias_lut_27(0), M_prev, H_prev, H_audio);
  M1 = ba.selector(0, 2, M1_H1);
  M_end = ja_lookup_m_end_k28(M1, H_audio);      // LUT lookup
  sumM_rest = ja_lookup_sum_m_rest_k28(M1, H_audio);  // LUT lookup
  Mavg = (M1 + sumM_rest) * inv_27;
  H_end = H_audio + bias_amp * bias_lut_27(26);
};

ja_hysteresis(H_in) =
    sum(i, 10,
        clk(i) * (clk(i) : ondemand(loop(i, H_in)))
    )
with { ... };
```

**Result**: Compiles and runs successfully as AU plugin.

### Failing: Full substep computation

**File**: `faust/dev/jaStreamingBiasProtoOD24.dsp`

```faust
// Complex seq chain inside ondemand - full JA physics per substep
ja_substep_with_phase(M_prev, H_prev, H_audio, M_sum_prev, phi, D) =
  M_sum_new, M_new, H_new, H_audio, phi + D, D
with {
  bias_offset = sin(phi);
  // Full JA physics computation (~15 operations per substep)
  ja_result = (M_prev, H_prev, H_audio, M_sum_prev) : ja_substep(bias_offset);
  M_sum_new = ba.selector(0, 4, ja_result);
  M_new = ba.selector(1, 4, ja_result);
  H_new = ba.selector(2, 4, ja_result);
};

// 60 chained substeps
ja_loop60(M_prev, H_prev, H_audio, phi_b, dphi_) =
  M_prev, H_prev, H_audio, 0.0, phi0, D : seq(i, 60, ja_substep_with_phase)
  <: ba.selector(0, 6), ba.selector(1, 6), ba.selector(2, 6)
with {
  D = dphi_ / 60.0;
  phi0 = phi_b + 0.5 * D;
};

ja_hysteresis(H_in) =
    sum(i, 3,
        clk(i) * (clk(i) : ondemand(loop(i, H_in)))
    )
with {
    mode = int(bias_mode + 0.5);
    clk(i) = (mode == i);
    loopK(H, K, inv_n, phi_s, dphi) = (loop ~ (mem, mem))
        : ba.selector(0, 3) : *(inv_n)
    with { loop(recM, recH) = recM, recH, H, phi_s, dphi : K; };

    loop(0, H) = loopK(H, ja_loop24, inv_24, phi_k24, dphi_k24);
    loop(1, H) = loopK(H, ja_loop48, inv_48, phi_k48, dphi_k48);
    loop(2, H) = loopK(H, ja_loop60, inv_60, phi_k60, dphi_k60);
};
```

**Result**: Faust compiles with warnings, but generated C++ fails to compile.

---

## Compilation Output

### Faust compilation (succeeds with warnings)

```
WARNING: slot x7 is associated with external signal clocked(0x147e11530, ...)
but used in clockenv 0x148d77700
```

(6 similar warnings for different clock environments)

### C++ compilation (fails)

```
FaustPluginProcessor.cpp:111635:26: error: use of undeclared identifier 'fTemp51SE'
    double fTemp2773SE = fTemp51SE + fTemp2772SE;
                         ^~~~~~~~~

FaustPluginProcessor.cpp:111636:26: error: use of undeclared identifier 'fTemp54SE'
    double fTemp2774SE = fTemp54SE + fTemp2773SE;
                         ^~~~~~~~~

FaustPluginProcessor.cpp:111637:26: error: use of undeclared identifier 'fTemp57SE'
    double fTemp2775SE = fTemp57SE + fTemp2774SE;
                         ^

FaustPluginProcessor.cpp:111638:26: error: use of undeclared identifier 'fTemp60SE'
    double fTemp2776SE = fTemp60SE + fTemp2775SE;
                         ^~~~~~~~~

... (20+ similar errors)

fatal error: too many errors emitted, stopping now
```

---

## Analysis

1. The `SE` suffix on variables suggests "Signal Environment" tracking
2. Variables like `fTemp51SE`, `fTemp54SE`, `fTemp57SE` follow a pattern (increments of 3)
3. These appear to be intermediate results from the `seq` chain that should have been generated but weren't
4. The warnings about "slot x7 associated with external signal... but used in clockenv" suggest signal/clock environment mismatch
5. The code generation appears to lose track of intermediate variables when `ondemand` wraps deeply nested `seq` chains

---

## Hypothesis

The `ondemand` primitive's code generator correctly handles:
- Simple expressions
- Single function calls (like LUT lookups)
- Shallow signal graphs

But fails when:
- `seq(i, N, f)` with large N (24, 48, 60)
- Complex multi-operation functions chained in seq
- Deep signal dependency chains inside the ondemand block

The signal environment (`SE`) variable generation appears to break down when tracking dependencies across many sequential operations inside a clocked region.

---

## Additional Findings (2025-11-30 Evening) - UPDATED 2025-12-01

### Compiler Hang Issue - USER ERROR (RESOLVED)

The compiler hang in `test_gated_substeps.dsp` was caused by **incorrect clock signal construction**:

**Wrong pattern (causes hang):**
```faust
gated_substep(M_prev, H_prev, ..., idx) = M_out, H_out, ..., idx + 1
with {
    clk = (idx < steps_this_sample);  // idx is RUNTIME counter!
    physics_result = clk : ondemand(ja_physics(...));
};
// Used with: seq(i, MAX_STEPS, gated_substep)
```

**Correct pattern (works perfectly):**
```faust
gated_substep(i, M_prev, H_prev, ...) = M_out, H_out, ...
with {
    clk = (i < steps_this_sample);  // i is COMPILE-TIME from seq!
    physics_result = clk : ondemand(ja_physics(...));
};
// Used with: seq(i, MAX_STEPS, gated_substep(i))
```

**Key insight**: The `ondemand` clock should be **compile-time determinable**. Using `i` from `seq(i, N, ...)` works; using a runtime counter hangs.

**Result**: With this fix, `test_gated_substeps.dsp` compiles and runs with 72 substeps (K60 Ultra).

---

### Undeclared Variables Issue - REAL BUG (Still Present)

The `jaStreamingBiasProtoOD24.dsp` still generates invalid C++ with undeclared `fTempXXSE` variables.

**Key observation**: The working `ja_streaming_bias_proto_od.dsp` uses the **same runtime clock**:
```faust
mode = int(bias_mode + 0.5);  // runtime from UI
clk(i) = (mode == i);         // runtime clock
```

But it works because operations **inside** ondemand are simple (1 substep + LUT).

The failing file has **complex `seq` chains inside**:
```faust
loop(2, H) = loopK(H, ja_loop60, ...);  // ja_loop60 = seq(i, 60, ...)
```

| Clock Type | Inside Ondemand | Result |
|------------|-----------------|--------|
| Runtime | Simple (LUT) | Works |
| Runtime | Complex (`seq(i,60,...)`) | **Bug** |
| Compile-time | Complex (`seq(i,72,...)`) | Works |

---

### Original Findings

The phase-locked "stroboscopic" algorithm itself is fine in FAUST:

| File | Description | CPU | Status |
|------|-------------|-----|--------|
| `faust/dev/ja_streaming_bias_proto_OD_24.dsp` | Full 60-step chain, each `ja_step_sc` written explicitly | ~24 % | Compiles & runs |
| `faust/test/ja_streaming_bias_proto_od.dsp` | 1 real JA step + LUT lookup per sample | <1 % | Compiles & runs |
| `faust/dev/test_gated_substeps.dsp` | Prototype that wraps `seq(i, N, gated_substep)` where `gated_substep` uses `clk : ondemand(...)` to skip inactive substeps | N/A | Compiler hangs (killed with `Signal 14`) |

Key observations from `test_gated_substeps.dsp`:

1. Replacing the ondemand call with a direct `ja_physics` invocation (same `seq` structure) compiles instantly → no algebraic loop in the DSP graph.
2. Keeping ondemand but manually unrolling a fixed number of gated stages (no `seq`) also compiles → runtime gating itself is OK.
3. Only the combination “`seq` + runtime index + `clk : ondemand(...)`” triggers the hang; the ondemand compiler prints thousands of “slot … used in clockenv …” warnings and never emits C++ before the watchdog kills it.

So the regression is not with the JA physics or the stroboscopic math; it is specifically the ondemand compiler’s clock-environment analysis when a `seq` chain feeds an ondemand-gated block.

---

## Reproduction Steps

1. Clone the repo: `https://github.com/Mando-369/FAUST_FSM_TAPE`
2. Build faust-ondemand fork (already in `tools/faust-ondemand/`)
3. Run:
```bash
cd /path/to/FAUST_FSM_TAPE
export PATH="$(pwd)/tools/faust-ondemand/build/bin:$PATH"
export FAUSTARCH="$(pwd)/tools/faust-ondemand/architecture"
export FAUSTLIB="$(pwd)/tools/faust-ondemand/share/faust"
export FAUSTINC="$(pwd)/tools/faust-ondemand/architecture"

# This works:
faust -double faust/test/ja_streaming_bias_proto_od.dsp -o test_ok.cpp

# This generates invalid C++:
faust -double faust/dev/jaStreamingBiasProtoOD24.dsp -o test_fail.cpp
```

4. Try to compile `test_fail.cpp` - will fail with undeclared identifier errors

---

## Files

- **Working DSP**: `faust/test/ja_streaming_bias_proto_od.dsp`
- **Failing DSP**: `faust/dev/jaStreamingBiasProtoOD24.dsp`
- **Generated C++ (failing)**: `faust/dev/jaStreamingBiasProtoOD24/FaustPluginProcessor.cpp`

---

## Workarounds

### For substep-level gating (full physics precision):
Use **compile-time clock** by passing `i` from `seq` to the gated function:
```faust
gated_substep(i, ...) = ... with { clk = (i < N); ... : ondemand(...) };
seq(i, MAX_STEPS, gated_substep(i))
```
See `faust/dev/test_gated_substeps.dsp` for working 72-substep implementation.

### For mode selection with runtime UI control:
Use simple operations inside ondemand (LUT lookups work, complex seq chains don't).
See `faust/test/ja_streaming_bias_proto_od.dsp` for working LUT-based approach.

---

## Questions for GRAME

1. **Runtime clock + complex seq bug**: Why does `ondemand` fail to generate correct C++ when the clock is a runtime signal AND the block contains complex `seq(i, N, ...)` chains? Simple operations work fine with the same runtime clock pattern.

2. **Signal Environment tracking**: The undeclared `fTempXXSE` variables suggest the SE (Signal Environment) tracking breaks down. Is this a known limitation when combining runtime clocks with deep signal dependency chains?

3. **Compile-time clock requirement**: Should `ondemand` clocks always be compile-time determinable for complex operations? If so, this should be documented.

4. **The "clockenv" warnings**: Are the warnings like "slot x7 associated with external signal... but used in clockenv..." indicative of this bug, or a separate issue?

---

## Contact

Thomas Mandolini
thomas.mand0369@gmail.com
https://github.com/Mando-369/FAUST_FSM_TAPE
