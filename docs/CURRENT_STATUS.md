# FAUST JA Hysteresis Library — Current Status

**Last updated**: 2025-11-29
**Collaborators**: Thomas Mandolini (OmegaDSP), GRAME (Stéphane Letz)

---

## Project Goal

Create a reusable **FAUST library (`jahysteresis.lib`)** for Jiles-Atherton magnetic hysteresis with phase-locked bias oscillator, suitable for tape saturation simulation.

**Target**: Mastering-grade quality at production-viable CPU cost.

**Library prefix**: `jah` (e.g., `jah.tape_channel_ui`)

---

## Current State

### What Works

| Component | Status | Notes |
|-----------|--------|-------|
| JA physics model | Complete | Ms=320, a=720, k=280, c=0.18, α=0.015 |
| Phase-locked bias oscillator | Complete | Fixed cycles/sample, sample-rate invariant |
| 2D LUT optimization | Complete | 1 real substep + LUT lookup |
| 10 bias modes (K28-K1920) | Complete | LoFi to beyond-physical range |
| FAUST prototype | Complete | `dev/ja_streaming_bias_proto.dsp` |
| FAUST library | In Progress | `jahysteresis.lib` (contribution-ready) |
| C++ reference | Complete | `JAHysteresisScheduler` with ~11% CPU |

### Performance (M4 Max, Ableton Live 12.3, AU)

| Implementation | CPU @ K60 | Notes |
|----------------|-----------|-------|
| FAUST (original, 66 substeps) | ~24% | Sequential dependency bottleneck |
| C++ scheduler | ~11% | Uses fractional substep accumulation |
| FAUST + LUT | ~1% | 20x+ improvement |

### Achieved Breakthrough

**Key insight**: Only substep 0 has cross-sample dependency. Substeps 1..N-1 are deterministic given (M1, H_audio).

**Solution**: Precompute 2D LUT mapping `(M_in, H_audio) → (M_end, sumM_rest)` for the deterministic portion.

**Result**: Collapsed 66 JA physics evaluations to 1 + cheap bilinear interpolation.

---

## Open Problems

### 1. Parallel Computation Overhead (Priority: High)

**Problem**: FAUST `ba.if` is a signal selector, not a conditional branch. All 10 mode loops are computed every sample; `ba.if` just picks the output.

**Impact**: ~10x unnecessary CPU overhead for multi-mode support.

**Proposed solution**: Unified LUT with mode-indexed offset (see `docs/LUT_RESTRUCTURE_PLAN.md`).

```faust
// Current: 10 parallel computations
ba.if(mode < 0.5, loopK28, ba.if(mode < 1.5, loopK32, ...))

// Proposed: Single computation with mode index
ja_lookup_m_end(mode, M1, H_audio)  // mode selects offset into unified table
```

**Future solution**: The upcoming **Ondemand primitive** (Yann Orlarey, IFC 24) will allow conditional block execution. This would enable true branching where only the selected mode computes.

### 2. Harmonic Imprint Research (Priority: High)

**Observation**: Each K/bias setting produces a distinct harmonic signature ("character").

**Problem**: Current mode selection (K28-K1920) is somewhat arbitrary. Need systematic analysis to identify:
- Which K/bias combinations produce musically useful imprints
- Optimal subset for the control range (lofi-gritty → highend-beyond)
- Whether intermediate values can be interpolated or need discrete LUTs

**Research needed**:
- Spectra/Harmonic analysis of each mode with test signals (Plugin Doctor, sine sweeps, impulses, music)
- THD+N measurements across drive levels
- Subjective listening tests to identify "sweet spots"
- Correlation between substep count and harmonic distribution

### 3. LUT Parameter Flexibility (Priority: Medium)

**Current limitation**: LUTs are precomputed for fixed bias parameters:
- `bias_level = 0.41`
- `bias_scale = 11.0`

**Problem**: Changing these parameters at runtime would require different LUTs.

**Options**:
1. Multiple LUT banks for discrete parameter presets
2. 3D or 4D LUT with parameter dimensions (memory-heavy)
3. Runtime LUT regeneration (background thread, crossfade)
4. Accept fixed bias as "tape formulation" preset

### 4. Variable Iteration Pattern (Priority: Low)

**C++ reference behavior**: Fractional substep accumulation causes step count to vary (e.g., 35-37 for K60) for better phase continuity.

**FAUST limitation**: Fixed unrolled chains require compile-time constant iteration count.

**Impact**: Subtle high-frequency response differences between FAUST and C++.

**Potential FAUST pattern**: Unroll to max count, gate inactive steps:
```faust
ba.if(step_idx < steps_this_sample, computeStep, passThrough)
```

**Note**: With LUT optimization, this becomes less critical since only substep 0 is computed in real-time.

---

## Technical Discoveries

### Why External LUT Generation is Required

Investigated FAUST's `ba.tabulate` and `ba.tabulate_chebychev` functions for potential init-time table computation. 

**Finding**: These functions **cannot** be used for JA hysteresis LUTs.

**Reason**: `ba.tabulate` can only tabulate **pure FAUST functions** — functions with no state, no feedback, no iteration. It evaluates `function(x)` for various x values at init time.

JA physics requires:
- Iterative Newton-Raphson solving (feedback loop)
- 66 sequential substeps, each depending on the previous
- State variables (M_prev carrying across substeps)

This cannot be expressed as a pure function `y = f(x)` that FAUST can evaluate at init time.

**Conclusion**: External Python LUT generation is the correct architecture. The Python computation is a one-time offline cost; the resulting FAUST code has zero table-computation overhead — just memory reads via `rdtable`.

**What `ba.tabulate` IS useful for**:
- Simple functions: `sin(x)`, `tanh(x)`, polynomial approximations
- Any stateless computation expressible as `y = f(x)`
- Could potentially be used for the bilinear interpolation coefficients if needed

### FAUST Optimization Resources (from Stéphane Letz)

Key documentation reviewed:
- [Init-time computation](https://faustdoc.grame.fr/manual/optimizing/#computations-done-at-init-time)
- [ba.tabulate functions](https://faustlibraries.grame.fr/libs/basics/#batabulate)
- [General optimization guide](https://faustdoc.grame.fr/manual/optimizing/)

**Note**: Same LUT optimization approach could be applied to the C++ version for even lower CPU usage.

---

## Challenges to Overcome

### Technical

1. **FAUST `rdtable` constraints**
   - 1D only (solved with flattened 2D indexing)
   - Compile-time table definition (solved with external generation)
   - All tables loaded into memory regardless of mode selection

2. **State accumulation sensitivity**
   - JA hysteresis is highly sensitive to floating-point precision
   - `float` precision degraded quality; `double` required
   - Polynomial `tanh` approximations changed tone

3. **Memory footprint**
   - 10 modes × 8385 values × 2 tables × 8 bytes = ~1.3 MB
   - Acceptable for plugin, may need reduction for embedded

### Architectural

1. **Library API design**
   - What parameters should be exposed vs. fixed?
   - How to handle mode selection without parallel overhead?
   - Should physics parameters (Ms, a, k, c, α) be runtime-adjustable?

2. **Integration with parent FSM_TAPE project**
   - This repo contains extracted JA hysteresis only
   - Need clean interface for reintegration

---

## Research Directions

### Harmonic Imprint Characterization

Goal: Map the K/bias parameter space to musical descriptors.

| Mode | Substeps | Expected Character | Status |
|------|----------|-------------------|--------|
| K28 | 27 | Maximum grit, aliasing artifacts | Needs testing |
| K32 | 36 | Crunchy, lo-fi | Needs testing |
| K60 | 66 | Classic tape saturation | Baseline |
| K120 | 132 | Clean, detailed | Needs testing |
| K1920 | 2112 | Beyond physical, ultra-smooth | Needs testing |

**Hypothesis**: Lower substep counts introduce inter-sample aliasing that manifests as characteristic harmonics. This is a "feature" for lo-fi modes but should be minimized for high-quality modes.

### Perceptual Mode Reduction

**Question**: Do we need 10 discrete modes, or can we interpolate between fewer anchor points?

If K60 and K240 cover the perceptually distinct territory, we could:
- Use only 2-3 LUTs
- Interpolate between them for intermediate settings
- Reduce memory and complexity

### Bias Waveform Variations

Current: Pure sine bias oscillator.

**Future exploration**:
- Asymmetric bias (different positive/negative excursions)
- Harmonic-rich bias (triangle, modified sine)
- These would require new LUT sets but could expand tonal palette

---

## File Structure

```
FAUST_FSM_TAPE/
├── faust/
│   ├── jahysteresis.lib              # Contribution-ready FAUST library (jah prefix)
│   ├── ja_lut_k*.lib                 # 10 mode-specific LUT libraries (K28-K1920)
│   ├── rebuild_faust.sh              # Build script preserving plugin IDs
│   ├── dev/
│   │   └── ja_streaming_bias_proto.dsp   # Working prototype (reference)
│   └── examples/
│       └── jah_tape_demo.dsp         # Demo importing jahysteresis.lib
├── juce_plugin/
│   └── Source/
│       ├── JAHysteresisScheduler.h   # C++ reference implementation
│       └── JAHysteresisScheduler.cpp
├── scripts/
│   └── generate_ja_lut.py            # LUT generator
└── docs/
    ├── CURRENT_STATUS.md             # This file
    ├── LUT_RESTRUCTURE_PLAN.md       # Unified LUT proposal
    ├── JA_LUT_IMPLEMENTATION_PLAN.md # Original LUT design
    └── JA_Hysteresis_Optimization_Summary.md
```

**Note**: `jahysteresis.lib` is the library-ready version for GRAME contribution.
`dev/ja_streaming_bias_proto.dsp` is the working prototype kept as reference.

---

## Next Steps

### Immediate (Code)

1. Implement unified LUT structure (`LUT_RESTRUCTURE_PLAN.md`)
2. Benchmark CPU reduction from eliminating parallel computation
3. Validate sound quality against per-mode LUT version

### Research

1. Conduct harmonic imprint analysis for all 10 modes
2. Identify musically distinct anchor points
3. Determine if mode interpolation is viable

### Documentation

1. Define `jahysteresis.lib` public API
2. Write usage examples for GRAME review
3. Document integration path back to FSM_TAPE

### IFC 2026 Preparation

Invited by Stéphane Letz to present at **International Faust Conference 2026**:
- **Date**: June 28-29, 2026
- **Location**: Cannes, France
- **Topic**: AI-assisted DSP development workflow, JA hysteresis optimization journey

---

## Questions for GRAME

1. Any recommendations for managing multiple LUT variants (mode × parameter combinations)?
2. Timeline for the **Ondemand primitive**? (Would solve parallel computation overhead)
3. Best practices for contributing optimized libraries to faustlibraries?

---

## Commit History Summary

| Commit | Description |
|--------|-------------|
| `946d4e2` | Expand to 10 bias modes (K28-K1920) with corrected LUTs |
| `55474e8` | 2D LUT optimization - 20x+ CPU reduction |
| `652ae5a` | Add FSM paper and phase-locked bias research |
| `c361a6d` | Simplify FAUST code using `seq(i,N,exp)` form |
| `76a5087` | Initial JA hysteresis FAUST/C++ comparison |

---

## Contact

- **Thomas Mandolini** — thomas.mand0369@gmail.com
- **Repository** — https://github.com/Mando-369/FAUST_FSM_TAPE
