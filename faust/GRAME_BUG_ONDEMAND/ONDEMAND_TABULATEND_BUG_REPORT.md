# Ondemand + ba.tabulateNd Bug Report

**Date**: 2025-12-26
**Reporter**: Thomas Mandolini (OmegaDSP)
**Faust Fork**: `master-dev-ocpp-od-fir-2-FIR15` (also tested FIR13 - same result)
**Repository**: https://github.com/Mando-369/FAUST_FSM_TAPE

---

## Summary

`ondemand` does **not gate** `ba.tabulateNd` LUT lookups when using runtime clock signals. All branches compute regardless of clock state, resulting in CPU = N × single_branch_cost instead of expected single_branch_cost.

This is different from the previous bug (runtime clock + complex seq = invalid C++). Here the code compiles and runs, but ondemand simply doesn't reduce CPU.

---

## Environment

- **macOS**: Darwin 24.6.0 (M4 Max)
- **Faust fork**: `tools/faust-ondemand/` (master-dev-ocpp-od-fir-2-FIR13)
- **Compile flags**: `-double -t 600`
- **Target**: JUCE AU plugin via `faust2juce`
- **DAW**: Reaper

---

## Files

**Primary reproduction** (recommended - fast compile & load):

| File | Description | CPU | Status |
|------|-------------|-----|--------|
| `ja_tabulateNd_3D_quality_modes.dsp` | 3 quality modes (K23/K45/K90) with 3D LUT | 1.5% | **Bug** - all compute |
| `ja_tabulateNd_3D_quality_modes.sh` | Build script (~7s compile) | - | - |

Expected: CPU varies by mode (K23 < K45 < K90)
Actual: 1.5% constant regardless of mode selection

**Secondary reproduction** (slower - 3 min compile, 60s load):

| File | Description | CPU | Status |
|------|-------------|-----|--------|
| `ja_tabulateNd_4D_presets.dsp` | 4 machine presets with 4D LUT | 4.4% | **Bug** - all compute |
| `ja_tabulateNd_4D_presets.sh` | Build script (~3 min compile) | - | - |

---

## Comparison with Working ondemand

**Working**: `jahysteresislib_proto_OD_3_modes.dsp` (full physics, no tabulateNd)

```faust
// Full physics inside ondemand - WORKS
ja_hysteresis(H_in) =
  sum(i, 3, clk(i) * (clk(i) : ondemand(loop(i, H_in))))
with {
  mode = int(quality_mode + 0.5);  // runtime clock
  clk(i) = (mode == i);
  loop(0, H) = loopK(H, ja_loop_k45, inv_45);  // ja_loop_k45 = seq(i, 45, ...)
  loop(1, H) = loopK(H, ja_loop_k23, inv_23);
  loop(2, H) = loopK(H, ja_loop_k11, inv_11);
};
```

**Result**: CPU changes based on mode (K180=3.4%, K92=1.9%, K44=1.0%)

**Failing**: `ja_tabulateNd_4D_presets.dsp` (tabulateNd LUTs)

```faust
// tabulateNd inside ondemand - FAILS TO GATE
ja_hysteresis_presets(H_audio, bias, asym, preset) = output
with {
  clk(i) = (int(preset + 0.5) == i);  // same runtime clock pattern!

  p0 = clk(0) : ondemand(cascade_a800(H_audio, bias, asym));
  p1 = clk(1) : ondemand(cascade_a810(H_audio, bias, asym));
  p2 = clk(2) : ondemand(cascade_atr(H_audio, bias, asym));
  p3 = clk(3) : ondemand(cascade_mx(H_audio, bias, asym));

  output = clk(0) * p0 + clk(1) * p1 + clk(2) * p2 + clk(3) * p3;
};

// Each cascade uses tabulateNd LUTs
cascade_a800(H_audio, bias, asym) = (loop ~ _) : (!, _)
with {
  loop(M_prev) = M_end, M_avg with {
    M1 = lut_m_end_a800(M_prev, H_audio, bias, asym);  // ba.tabulateNd
    // ... 4 cascaded lookups
  };
};

lut_m_end_a800(M, H, bias, asym) = ba.tabulateNd(1, ja_k90_m_end_a800,
  (33, 65, 9, 5, -1, -40, 0.1, 0.0, 1, 40, 0.9, 0.4, M, H, bias, asym)).cub;
```

**Result**: CPU = 4.4% regardless of preset selection (all 4 compute)

---

## Analysis

| Pattern | Inside ondemand | Result |
|---------|-----------------|--------|
| Runtime clock | Full physics (`seq(i, N, ...)`) | **Works** |
| Runtime clock | `ba.tabulateNd` LUT | **Fails to gate** |
| Compile-time clock | Either | Works |

The key difference:
- `seq(i, N, f)` generates inline computation that ondemand can gate
- `ba.tabulateNd` generates `rdtable` lookups + interpolation that ondemand apparently cannot gate

---

## Hypothesis

`ba.tabulateNd` generates code that includes:
1. Table initialization (compile-time)
2. Index calculation (runtime)
3. `rdtable` reads (runtime)
4. Interpolation math (runtime)

The `ondemand` primitive may be failing to wrap steps 2-4 in the clock-gated conditional, causing all LUT lookups to execute regardless of clock state.

Alternatively, `rdtable` reads may be considered "side-effect-free" by the compiler and hoisted outside the clock-gated region as an optimization, defeating the purpose of ondemand.

---

## Workaround

Currently none that preserves both runtime preset selection AND tabulateNd performance.

Options:
1. Use full physics with ondemand (works, but higher CPU)
2. Build separate plugins per preset (works, but no runtime switching)
3. Accept all LUTs computing (4× expected CPU)

---

## Confirmed: Bug Affects All tabulateNd Dimensions

**Tested both 3D and 4D** - bug is NOT dimension-specific.

| Test | Dimensions | Expected CPU | Actual CPU | Result |
|------|------------|--------------|------------|--------|
| 4D presets (4 machines) | M×H×Bias×Asym | ~1.1% (active only) | 4.4% (all compute) | **BUG** |
| 3D quality modes (K23/K45/K90) | M×H×Bias | Variable by mode | 1.5% (constant) | **BUG** |

The bug affects `ba.tabulateNd` regardless of dimensionality. Each mode's LUT computes even when its clock signal is 0.

**3D test file**: `faust/test/ja_tabulateNd_3D_quality_modes.dsp`

---

## Questions for GRAME

1. **Is this expected behavior?** Should `ondemand` work with `ba.tabulateNd` LUT lookups?

2. **rdtable gating**: Does `ondemand` correctly gate `rdtable` reads, or are they hoisted outside the clock conditional?

3. **Possible fix**: Could the ondemand code generator be modified to keep tabulateNd interpolation inside the gated block?

---

## Reproduction Steps

**Recommended** (fast - 7s compile, instant load):
```bash
cd /path/to/FAUST_FSM_TAPE/faust/GRAME_BUG_ONDEMAND
./ja_tabulateNd_3D_quality_modes.sh

# Load in DAW
# Switch between K23 Eco / K45 Standard / K90 HQ
# Observe: CPU stays at 1.5% (should vary by mode)
```

Alternative (slower - 3 min compile, 60s load):
```bash
cd /path/to/FAUST_FSM_TAPE/faust/GRAME_BUG_ONDEMAND
./ja_tabulateNd_4D_presets.sh

# Load in DAW, CPU = 4.4% regardless of preset selection
```

---

## Contact

Thomas Mandolini
thomas.mand0369@gmail.com
https://github.com/Mando-369/FAUST_FSM_TAPE
