# JA Hysteresis CPU Optimization — Working Summary (from this chat)

Date: 2025-11-29  
Goal: **Keep mastering-grade JA + phase-locked bias sound** but massively reduce CPU cost (esp. divisions + 66 sequential substeps/sample).

---

## What we established from your notes + code

### Current structure (bottleneck)
- Your implementation runs a **JA hysteresis update per bias substep**, with **fixed substeps per sample** for modes like K60:
  - Example: **K60 Normal = 3.0 bias cycles/sample × 22 substeps/cycle = 66 substeps/sample**.
- Per your research notes, the heavy cost is:
  - **198 divisions/sample** in K60 (3 per substep) + hundreds of multiplies.
  - `sin()` LUT barely helps (~1%) because **divisions + sequential dependency** dominate.
- Sequential dependency is fundamental: each substep depends on previous `M` and `H`, so parallelism is limited.

Sources: `JA_OPTIMIZATION_RESEARCH.md`, and your observed measurements.

---

## Why “cheap approximations” didn’t work (and likely won’t)
- Polynomial `tanh` approximation changed tone (distortion artifacts).
- `float` instead of `double` degraded quality due to **state accumulation sensitivity** of hysteresis.
- FAUST `-vec` got worse (likely overhead + stateful chain).

These outcomes match the expectation for a stateful physics model.

---

## The key new insight from your *actual C++ scheduler*
### Fixed bias stepping creates a *deterministic* inner loop
From `JAHysteresisScheduler`:
- In K60 Normal, `biasCyclesPerSample * substepsPerCycle` is an **integer**, so:
  - `biasPhase` returns to the same phase each sample.
  - You execute the **same bias LUT sequence each sample** and `stepsTaken` is fixed.

### Only the FIRST substep depends on cross-sample history
In `executeSubstep()`:
- Cross-sample coupling only enters through `dH = HNew - HPrev`.
- After substep 0, inside a sample, you set `HPrev = HNew`, so for substeps i ≥ 1:
  - `HNew_i = HAudio + biasAmp * b_i`
  - `dH_i = biasAmp * (b_i - b_{i-1})`
- Therefore substeps 1..N-1 are a deterministic map driven by:
  - the post-step-0 state `M1`
  - the current audio field `HAudio`
  - and the mode’s fixed bias sequence.

This makes the algorithm ideal for a **stroboscopic / Poincaré map reduction**.

---

## Recommended “max reduction without changing the physics”
### Runtime strategy: **1 real JA substep + 1 LUT lookup**
Keep your exact physics for history correctness, but replace the repeated deterministic bias chain.

Per sample:
1. Compute substep 0 **exactly** (full `executeSubstep` once).
2. Replace substeps 1..N-1 with a **2D LUT**:
   - Input: `(M1, HAudio)`
   - Output: `(M_end, sumM_rest)` where `sumM_rest = Σ M_i for i=1..N-1`
3. Compute magnetization average:
   - `Mavg = (M1 + sumM_rest) / N`
4. Update states:
   - `MPrev = M_end`
   - `HPrev = HAudio + biasAmp * b_last` (known deterministically)

**Why it’s “max” CPU reduction:**
- Collapses ~66 sequential JA solves → **1** JA solve + cheap interpolation.
- The LUT is built using your **exact current solver**, so sound matches reference very closely.

---

## LUT design notes (practical)
### What to store
For each grid point:
- `M_end` (magnetization after remainder steps)
- `sumM_rest` (sum of magnetizations across remainder steps)

### Suggested grid
- 2D table over `(M_in, HAudio)` with **warped H axis** (more resolution near 0).
- Use **double** storage and math in LUT (your experiments suggest float hurts).

### Table size ballpark
Example:
- `NM = 257`, `NH = 513` → ~132k entries
- 2 doubles per entry → ~2.1 MB per LUT (per mode/bias amplitude set)

### Parameters
- If Drive/Bias/TapeType are “preset-ish”: precompute a small bank of LUTs and interpolate between LUTs.
- If parameters change continuously: rebuild LUT **off the audio thread** and crossfade safely.

---

## Second-best options (if you don’t want LUT yet)
- Replace 66 small steps with fewer **RK4** steps or **implicit/Newton** steps per sample (still physics, less brute force).
- Also possible: **adaptive step count** based on signal change, but LUT gives much bigger wins.

---

## FAUST vs C++ conclusion (for production)
- Your measurements show C++ is ~2× faster than FAUST for identical math in this model.
- Given the strong sequential dependency, the production core should likely be **C++**, with FAUST for prototyping (or calling into a C++ core).

---

## Next implementation tasks in the IDE (action list)
1. Add a `LUTMap` struct/class:
   - axis definitions (M grid, H grid with warp)
   - storage: `M_end`, `sumM_rest`
   - bilinear interpolation
2. Add a LUT build function:
   - for each `(M_in, HAudio)` grid point:
     - set temporary state to `M_in`, `HPrev = HAudio + biasAmp * b0`
     - run substeps 1..N-1 using your existing `executeSubstep`
     - store `M_end` + `sumM_rest`
3. Modify `process()`:
   - compute step 0 real
   - lookup remainder results
   - produce `Mavg` and output sample
4. Keep a compile-time or runtime switch to fall back to full substep simulation for A/B testing.

---

## Optional “absolute max” variant
- Remove even the first substep by extending LUT to 3D:
  - include `dH0` (or `HPrev`) as third dimension
- This is heavier memory/complexity; the **1-step + 2D LUT** is the best first target.

---
