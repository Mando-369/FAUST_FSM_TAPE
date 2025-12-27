# Ondemand + ba.tabulateNd - Investigation Report

**Date**: 2025-12-27 (Updated)
**Reporter**: Thomas Mandolini (OmegaDSP)
**Faust Fork**: `master-dev-ocpp-od-fir-2-FIR15`
**Repository**: https://github.com/Mando-369/FAUST_FSM_TAPE

---

## Summary

**RESOLVED**: The perceived 3x CPU difference was NOT an ondemand bug, but a **compiler difference** between official FAUST and the ondemand fork.

When both plugins are built with the same compiler (ondemand fork), they show **1:1 CPU ratio**, confirming ondemand works correctly with `ba.tabulateNd`.

---

## Original Issue

Initial observation in Reaper (M4 Max):
- `ja_tabulateNd.dsp` (single mode): 0.45% CPU
- `ja_tabulateNd_3D_quality_modes.dsp` (3 modes): 1.2% CPU
- Ratio: ~1:3 → suspected ondemand failure

---

## Root Cause

The single-mode plugin was built with **official FAUST** (`/opt/homebrew/bin/faust2juce`), while the 3-mode plugin was built with the **ondemand fork** (`tools/faust-ondemand`).

The ondemand fork produces ~2.7x slower code than official FAUST, which explains the CPU difference.

---

## Verification Tests

### Test 1: Static Benchmark (faustbench)

Both DSPs compiled with ondemand fork:

**M4 Max Results:**
| Test | MBytes/sec | CPU % | DSP Size |
|------|------------|-------|----------|
| ja_tabulateNd.dsp (single) | 15.6 | 4.75% | 328 |
| ja_tabulateNd_3D_quality_modes.dsp (3-mode) | 16.3 | 4.62% | 352 |

**M1 Results:**
| Test | MBytes/sec | CPU % | DSP Size |
|------|------------|-------|----------|
| ja_tabulateNd.dsp (single) | 10.45 | 6.58% | 328 |
| ja_tabulateNd_3D_quality_modes.dsp (3-mode) | 10.32 | 6.66% | 352 |

**Ratio: ~1:1** → ondemand works in static benchmark

### Test 2: Reaper with Same Compiler

Both plugins rebuilt with ondemand fork:

**M4 Max Reaper Results:**
| Plugin | Format | Alone | With Other |
|--------|--------|-------|------------|
| ja_tabulateNd | AU | 1.2% | 0.8% |
| ja_tabulateNd_3D_quality_modes | AU | 1.2% | 0.75% |
| ja_tabulateNd | VST3 | 1.5% | 0.85% |
| ja_tabulateNd_3D_quality_modes | VST3 | 1.5% | 0.79% |

**Ratio: 1:1** → ondemand works correctly

---

## Observations

1. **Mode switching behavior**: When changing K mode (K23/K45/K90), CPU drops for a couple seconds, then stabilizes. This confirms ondemand IS gating inactive branches.

2. **CPU drops when both loaded**: When both plugins are loaded simultaneously, each shows lower CPU. Likely CPU frequency scaling or measurement artifact.

3. **VST3 overhead**: VST3 shows ~25% higher CPU than AU (1.5% vs 1.2%).

4. **Fork performance**: The ondemand fork produces ~2.7x slower code than official FAUST. This is the overhead for ondemand support.

---

## Conclusion

**Ondemand works correctly with ba.tabulateNd.**

The original "bug report" was based on comparing apples to oranges (different compilers). When both plugins are built with the ondemand fork, they show identical CPU usage, confirming proper branch gating.

---

## Benchmark Commands

See `BENCHMARK_COMMANDS.md` for reproducible benchmark instructions.

---

## Contact

Thomas Mandolini
thomas.mand0369@gmail.com
https://github.com/Mando-369/FAUST_FSM_TAPE
