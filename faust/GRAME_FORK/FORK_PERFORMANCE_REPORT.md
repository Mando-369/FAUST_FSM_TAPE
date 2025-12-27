# Ondemand Fork Performance Report

**Date**: 2025-12-27
**Reporter**: Thomas Mandolini (OmegaDSP)
**Fork**: `master-dev-ocpp-od-fir-2-FIR15`
**Official**: `/opt/homebrew/bin/faust` (homebrew)

---

## Issue

The ondemand fork produces **~2.7x slower code** than official FAUST when using `ba.tabulateNd`, even for DSP that does NOT use the `ondemand` primitive.

**Confirmed**: This slowdown is **tabulateNd-specific**. Other DSP patterns (iterative solvers, exp, filters) show no overhead.

---

## Test Case

`ja_tabulateNd.dsp` - Single mode plugin using `ba.tabulateNd` (no ondemand).

Same DSP code, different compilers:

| Compiler | Reaper CPU | Ratio |
|----------|------------|-------|
| Official FAUST | 0.42% | 1.0x |
| Ondemand fork | 1.2% | **2.9x** |

---

## Environment

- macOS Darwin 24.6.0 (M4 Max)
- Reaper 7.x
- AU plugin format
- Sample rate: 48kHz (assumed)

---

## Reproduction

```bash
# Build with official FAUST
/opt/homebrew/bin/faust2juce -jucemodulesdir ~/Dev/JUCE_Projects/FAUST_FSM_TAPE/JUCE/modules \
  ja_tabulateNd.dsp

# Build with ondemand fork
export PATH="$HOME/Dev/JUCE_Projects/FAUST_FSM_TAPE/tools/faust-ondemand/build/bin:$PATH"
export FAUSTLIB="$HOME/Dev/JUCE_Projects/FAUST_FSM_TAPE/tools/faust-ondemand/share/faust"
faust2juce -jucemodulesdir ~/Dev/JUCE_Projects/FAUST_FSM_TAPE/JUCE/modules \
  ja_tabulateNd.dsp

# Load both in Reaper, compare CPU
```

---

## Control Test: TUDI_Limiter (No tabulateNd)

`TUDI_Limiter.dsp` - Tunnel diode limiter using iterative solver, exp, filters. No tabulateNd.

| Compiler | Reaper CPU | Ratio |
|----------|------------|-------|
| Official FAUST | ~same | 1.0x |
| Ondemand fork | ~same | **1.0x** |

**Conclusion**: Fork overhead is tabulateNd-specific, not general.

---

## Unexplained Observation: CPU Drops When Both Plugins Active

When loading both `ja_tabulateNd` and `ja_tabulateNd_3D_quality_modes` simultaneously, CPU drops for each plugin:

| Plugin | Format | Alone | Both Active |
|--------|--------|-------|-------------|
| ja_tabulateNd | AU | 1.2% | 0.8% |
| ja_tabulateNd_3D_quality_modes | AU | 1.2% | 0.75% |
| ja_tabulateNd | VST3 | 1.5% | 0.85% |
| ja_tabulateNd_3D_quality_modes | VST3 | 1.5% | 0.79% |

**No explanation for this behavior.**

Hypotheses:
1. CPU frequency scaling - more load triggers higher clock speed, making each plugin appear more efficient
2. Reaper's CPU measurement artifact - percentage calculated differently with multiple FX
3. Memory/cache effects - shared code paths benefit from being hot in cache
4. SIMD batching - multiple plugins processed together more efficiently

---

## Questions for GRAME

1. **Is the 2.7x overhead expected?** Does the fork's code generation add overhead even when `ondemand` is not used?

2. **Can the fork match official performance** for DSP that doesn't use ondemand primitives?

3. **CPU measurement anomaly**: Any insight into why CPU drops when multiple plugins are active?

---

## Impact

Users who want to use `ondemand` with `ba.tabulateNd` pay a ~2.7x CPU penalty on LUT-based code. This limits the usefulness of the primitive for CPU optimization in LUT-heavy DSP.

**Confirmed**: Other DSP patterns (iterative solvers, exp, filters) are NOT affected.

---

## Contact

Thomas Mandolini
thomas.mand0369@gmail.com
https://github.com/Mando-369/FAUST_FSM_TAPE
