# JA Hysteresis Optimization Research

## Current Implementation

### The Algorithm
Jiles-Atherton (JA) magnetic hysteresis model with phase-locked bias oscillator for tape saturation.

**Core equation:**
```
dM/dH = (c * dMan/dH + (Man - M) / pin) / (1 - c * α * dMan/dH)
```

**Physics parameters (tuned for tape sound):**
- Ms = 320 (saturation magnetization)
- a = 720 (anhysteretic curve shape)
- k = 280 (coercivity / loop width)
- c = 0.18 (reversibility ratio)
- α = 0.015 (mean field coupling)

### Why It Sounds Good
- Full JA physics model (not simplified)
- Phase-locked bias oscillator (2-3 cycles per audio sample)
- Midpoint sampling for each substep
- Proper magnetization averaging
- This combination produces authentic tape saturation with correct harmonic structure

### The Bottleneck

**Per audio sample, per channel:**
| Operation | Count | Notes |
|-----------|-------|-------|
| Substeps | 66 (K60 mode) | Each substep = full JA physics |
| Divisions | 198 | 3 per substep (fast_tanh, inv_pin, inv_denom) |
| Multiplications | ~600+ | Various physics calculations |

**CPU comparison (M4 Max, K60 mode):**
- C++ version: ~11%
- FAUST version: ~24%

Same algorithm, same physics, 2x performance difference.

## What We've Tried

### 1. Sin() Lookup Table (LUT)
- **Result:** Implemented, ~1% improvement
- **Why:** Sin() wasn't the bottleneck - divisions are

### 2. Polynomial tanh (division-free)
- **Result:** Broke the sound (distortion)
- **Why:** Polynomial approximation not accurate enough for JA physics

### 3. Float instead of double
- **Result:** Sounds bad
- **Why:** Precision loss affects hysteresis state accumulation

### 4. FAUST -vec flag
- **Result:** Added 10% CPU (worse!)
- **Why:** Unknown - vectorization overhead?

## The Core Problem

```
1 audio sample
  → add bias offset (from LUT)
  → compute JA physics (3 divisions, ~10 multiplications)
  → update M, H state
  → repeat 66 times
  → average magnetization
  → 1 output sample
```

The JA model is **stateful** - each substep depends on previous substep's M and H.
This creates a **sequential dependency chain** that can't be parallelized.

### The 3 Divisions Per Substep

```faust
// 1. fast_tanh (anhysteretic magnetization)
fast_tanh(x) = t * (27.0 + x2) / (27.0 + 9.0 * x2)

// 2. inv_pin (pinning field reciprocal)
inv_pin = 1.0 / (pin + sigma)

// 3. inv_denom (JA denominator reciprocal)
inv_denom = 1.0 / (denom + 1e-9)
```

Division is ~15-20 cycles vs ~4 for multiplication.
198 divisions × 20 cycles = ~4000 cycles per sample just for divisions.

## What We're Looking For

### Option A: Mathematical Reformulation
Is there a way to express the **averaged JA response over a bias cycle** without computing each substep individually?

The bias is deterministic (precomputed LUT). The audio input is constant across all 66 substeps within one sample. Only M and H evolve through the substeps.

**Question:** Can the net effect of 66 JA substeps with sinusoidal bias be expressed as a simpler function?

### Option B: Reduced-Order Model
Can we approximate the JA behavior with fewer substeps while maintaining sound quality?

- Currently: 66 substeps (K60), 54 (K48), 36 (K32)
- Could we use 16-22 substeps with a correction factor?

### Option C: Division-Free JA Formulation
Is there an alternative JA formulation that avoids the divisions?

- Newton-Raphson reciprocal? (Still needs initial division or approximation)
- Different anhysteretic function? (Current fast_tanh is already optimized)
- Algebraic rearrangement to combine divisions?

### Option D: FAUST-Specific Optimizations
Why is FAUST 2x slower than C++ for identical math?

- Code generation inefficiency?
- Memory access patterns?
- Lack of SIMD utilization?
- Compiler optimization barriers?

## Reference Files

- FAUST: `faust/ja_streaming_bias_proto.dsp`
- C++: `juce_plugin/Source/JAHysteresisScheduler.cpp`
- FSM Paper: `docs/FSM_doc/fsm - full spectrum magnetization WORD.md`
- Bias Oscillator: `docs/phase-locked_SR-driven Bias/`

## Key Insight

The C++ version proves this algorithm CAN run at 11% CPU. The physics aren't the fundamental problem - the FAUST implementation is. Either we optimize FAUST, or we accept C++ as the production version and use FAUST for prototyping only.
