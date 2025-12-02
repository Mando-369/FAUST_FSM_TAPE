# FAUST JA Hysteresis Library — Current Status

**Last updated**: 2025-12-02
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
| 10 bias modes (K28-K2101) | Complete | LoFi to beyond-physical range (all half-integer cycles) |
| FAUST prototype (ba.if) | Complete | `dev/ja_streaming_bias_proto.dsp` |
| FAUST prototype (ondemand) | Complete | `dev/ja_streaming_bias_proto_OD_72.dsp` |
| FAUST library | In Progress | `jahysteresis.lib` (contribution-ready) |
| C++ reference (original) | Complete | `JAHysteresisScheduler` with ~11% CPU |
| C++ reference (LUT) | Complete | `JAHysteresisSchedulerLUT` with <1% CPU expected |

### Performance (M4 Max, Ableton Live 12.3, AU)

| Implementation | CPU @ K60 | Notes |
|----------------|-----------|-------|
| FAUST (original, 66 substeps) | ~24% | Sequential dependency bottleneck |
| C++ scheduler (original) | ~11% | Uses fractional substep accumulation |
| FAUST + LUT | ~1% | 20x+ improvement |
| FAUST + ondemand (72 substeps) | ~8% | Single mode, compile-time gating |
| C++ + LUT | <1% expected | Ready for integration, see `cpp_reference/` |

### Achieved Breakthrough

**Key insight**: Only substep 0 has cross-sample dependency. Substeps 1..N-1 are deterministic given (M1, H_audio).

**Solution**: Precompute 2D LUT mapping `(M_in, H_audio) → (M_end, sumM_rest)` for the deterministic portion.

**Result**: Collapsed 66 JA physics evaluations to 1 + cheap bilinear interpolation.

---

## Open Problems

### 1. Parallel Computation Overhead (Priority: High) — PARTIALLY SOLVED

**Problem**: FAUST `ba.if` is a signal selector, not a conditional branch. All 10 mode loops are computed every sample; `ba.if` just picks the output.

**Solution**: The **Ondemand primitive** (Yann Orlarey, IFC 24) enables true conditional block execution.

**Limitation discovered**: `ondemand` requires **compile-time determinable clocks** for complex `seq` chains. Runtime mode selection with full substep computation **does not work** — generates invalid C++ with undeclared variables.

| Clock Type | Operations Inside | Result |
|------------|-------------------|--------|
| Runtime (UI mode) | Simple (LUT lookup) | Works |
| Runtime (UI mode) | Complex (`seq(i,60,...)`) | **Bug - invalid C++** |
| Compile-time (`i` from seq) | Complex (`seq(i,72,...)`) | Works |

**Working implementations**:
- `faust/test/ja_streaming_bias_proto_od.dsp` — LUT-based, runtime mode selection
- `faust/dev/ja_streaming_bias_proto_OD_72.dsp` — Full 72 substeps, compile-time gating

**For GRAME/Stéphane**: See `docs/GRAME_ONDEMAND_BUG_REPORT.md` for full details. Question: Is there a way to achieve runtime mode selection with full substep computation using `ondemand`, or is LUT the only viable approach?

```faust
// Works: compile-time i from seq
gated_substep(i, M_prev, H_prev, ...) = ... with {
    clk = (i < steps_this_sample);  // i from seq is compile-time!
    physics_result = clk : ondemand(ja_physics(...));
};
seq(i, MAX_STEPS, gated_substep(i))

// Doesn't work: runtime mode selection + complex seq inside
clk(i) = (int(mode) == i);  // runtime clock from UI
loop(0, H) = clk(0) : ondemand(loopK(H, ja_loop60, ...));  // ja_loop60 = seq(i,60,...)
// -> generates invalid C++ with undeclared fTempXXSE variables
```

### 2. Harmonic Imprint Research (Priority: High) — SOLVED

**Solution**: All modes now use **half-integer cycles + odd substeps**. This ensures opposite bias polarity between adjacent samples, introducing even harmonics for warmer, more musical tone.

| Mode | Cycles | Substeps | Character |
|------|--------|----------|-----------|
| K28 | 1.5 | 27 | Maximum grit |
| K45 | 2.5 | 45 | Crunchy, lo-fi |
| K63 | 3.5 | 63 | Classic tape |
| K99 | 4.5 | 99 | Smooth warmth |
| K121 | 5.5 | 121 | Standard (default) |
| K187 | 8.5 | 187 | High quality |
| K253 | 11.5 | 253 | Very detailed |
| K495 | 22.5 | 495 | Ultra detailed |
| K1045 | 47.5 | 1045 | Extreme |
| K2101 | 95.5 | 2101 | Beyond physical |

**Key insight**: Lower substep counts introduce inter-sample "aliasing" that manifests as characteristic harmonics — a feature for lo-fi modes, minimized in HQ modes.

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

### 4. LUT Responsiveness / Static Behavior (Priority: High) — IN PROGRESS

**Problem**: The LUT optimization creates static behavior — same `(M1, H_audio)` always produces identical output. The C++ full-physics version feels more "alive" due to:
- Variable substep count (fractional cursor: 65, 66, 67...)
- Continuous phase across samples
- All substeps computed with real physics

**Challenge**: Full FAUST physics = ~24% CPU (unusable). C++ full physics = ~11% CPU (quality reference).
**Goal**: Highest quality at affordable CPU — anything below C++ 11% is acceptable.

**Research document**: [`docs/JA_LUT_NONLINEARITY.md`](JA_LUT_NONLINEARITY.md)

**Options being explored** (in priority order):

| Option | Type | Description | CPU Impact |
|--------|------|-------------|------------|
| 1. Multiple real substeps | Pure JA | 2-3 substeps before LUT | +2-3 substeps |
| 2. Dynamic α(M) | Pure JA | Coupling varies with magnetization | Negligible |
| 3. Slew-dependent k(dH) | Pure JA | Pinning responds to input rate | Negligible |
| 4. Dynamic c(M) | Pure JA | Reversibility varies with level | Negligible |

**Previous experiment**: `faust/dev/ja_streaming_bias_proto_backup.dsp` implemented midpoint sampling and C++ substep counts, but not fractional cursor or continuous phase.

### 5. Variable Iteration Pattern (Priority: Low)

**C++ reference behavior**: Fractional substep accumulation causes step count to vary (e.g., 35-37 for K60) for better phase continuity.

**FAUST limitation**: Fixed unrolled chains require compile-time constant iteration count.

**Impact**: Subtle high-frequency response differences between FAUST and C++.

**Note**: With LUT optimization, this becomes less critical since only substep 0 is computed in real-time.

---

## Ondemand Implementation Options

With the experimental `ondemand` primitive now available, there are **two approaches** to optimize the full-precision (non-LUT) FAUST implementation.

### Background: C++ Scheduler Superiority

The C++ `JAHysteresisScheduler` (`cpp_reference/JAHysteresisScheduler.cpp`) sounds superior due to its **dynamic substep scheduling**:

```cpp
// C++ scheduler: fractional cursor determines substeps per sample
substepCursor += biasCyclesPerSample * substepsPerCycle;
int stepsTaken = static_cast<int>(std::floor(substepCursor));
substepCursor -= static_cast<double>(stepsTaken);  // carry fraction

for (int i = 0; i < stepsTaken; ++i) {
    executeSubstep(...);
}
if (stepsTaken == 0) stepsTaken = 1;  // guarantee minimum 1
```

Key features:
- **Fractional accumulation**: Cursor carries between samples
- **Variable substep count**: Sometimes N, sometimes N+1 substeps
- **Phase continuity**: Leftover phase advances smoothly
- **Mode × Quality**: K32/K48/K60 × Eco/Normal/Ultra combinations

| Mode | Eco | Normal | Ultra |
|------|-----|--------|-------|
| K32 (2 cycles) | 32 | 36 | 40 |
| K48 (3 cycles) | 48 | 54 | 57 |
| K60 (3 cycles) | 60 | 66 | 72 |

### Option A: Simple Mode-Level Ondemand (Recommended First)

**Concept**: Use `ondemand` to select between K24/K48/K60 modes. Only the active mode computes.

**Prototype**: `faust/dev/ja_streaming_bias_proto_OD_24.dsp`

```faust
// Each mode has fixed substep count via seq
ja_loop24 = seq(i, 24, ja_substep_with_phase);
ja_loop48 = seq(i, 48, ja_substep_with_phase);
ja_loop60 = seq(i, 60, ja_substep_with_phase);

// Ondemand selects which mode computes
ja_hysteresis(H_in) =
    sum(i, 3,
        clk(i) * (clk(i) : ondemand(loop(i, H_in)))
    )
with {
    mode = int(bias_mode + 0.5);
    clk(i) = (mode == i);

    loop(0, H) = loopK(H, ja_loop24, inv_24, phi_k24, dphi_k24);
    loop(1, H) = loopK(H, ja_loop48, inv_48, phi_k48, dphi_k48);
    loop(2, H) = loopK(H, ja_loop60, inv_60, phi_k60, dphi_k60);
};
```

**Pros**:
- Simple, proven pattern (same as LUT prototype)
- Significant CPU savings (only 1 of 3 modes computes)
- Clean code structure

**Cons**:
- Fixed substep count per mode (no fractional accumulation)
- Doesn't match C++ scheduler's variable iteration
- May have subtle sound differences from C++

**Status**: Implementation in progress.

### Option B: Dynamic Substep Gating (Experimental, Future)

**Concept**: Replicate C++ scheduler behavior by gating individual substeps with `ondemand`.

**Goal**: Variable substep count per sample based on fractional cursor accumulation.

```faust
// Maximum possible substeps (K60 Ultra = 72)
MAX_STEPS = 72;

// Cursor accumulator - determines how many substeps THIS sample
cursor_target = biasCycles * substepsPerCycle;  // e.g., 3.0 * 22 = 66
cursor_acc = cursor_target : (+ ~ _);           // accumulates
cursor_prev = cursor_acc @ 1;
steps_this_sample = int(floor(cursor_acc)) - int(floor(cursor_prev)) : max(1);

// Each substep gated by runtime comparison
gated_substep(i)(M_prev, H_prev, H_audio, M_sum, phi, D) =
    M_sum_out, M_out, H_out, H_audio, phi_out, D
with {
    clk = (i < steps_this_sample);  // runtime: 1 if should run, 0 otherwise

    // Expensive JA physics only computed when clk=1
    computed = clk : ondemand(ja_substep_with_phase(M_prev, H_prev, H_audio, M_sum, phi, D));

    // Extract computed values (0 when clk=0 due to ondemand)
    M_sum_computed = ba.selector(0, 6, computed);
    M_computed     = ba.selector(1, 6, computed);
    H_computed     = ba.selector(2, 6, computed);
    phi_computed   = ba.selector(4, 6, computed);

    // Select: computed result when clk=1, pass-through when clk=0
    // ba.if evaluates both, but ondemand already saved the expensive computation
    M_sum_out = ba.if(clk, M_sum_computed, M_sum);
    M_out     = ba.if(clk, M_computed, M_prev);
    H_out     = ba.if(clk, H_computed, H_prev);
    phi_out   = ba.if(clk, phi_computed, phi);
};

// Chain all potential substeps - inactive ones pass through
process_chain =
    M_prev, H_prev, H_audio, 0.0, phi0, D
    : seq(i, MAX_STEPS, gated_substep(i))
    <: ba.selector(0, 6), ba.selector(1, 6), ba.selector(2, 6);

// Divide by actual steps taken (not fixed count)
output = process_chain : /(float(steps_this_sample));
```

**Key Challenges**:

1. **Ondemand syntax with inputs**: Unclear if `clk : ondemand(f(inputs))` works when `f` needs signal inputs from the seq chain.

2. **Pass-through logic**: When `clk=0`, ondemand outputs 0. Need `ba.if` to select pass-through values. The `ba.if` evaluates both branches, but ondemand already saved the expensive computation.

3. **Graph size**: Creates MAX_STEPS (72) substep instances. CPU savings come from ondemand not computing inactive ones, but memory/graph fixed.

4. **Division by variable**: `steps_this_sample` varies per sample. Need to divide accumulated M_sum by actual count, not fixed.

**Pros**:
- Matches C++ scheduler behavior exactly
- Variable substep count with fractional accumulation
- Could achieve C++ sound quality

**Cons**:
- Experimental - may not work with current ondemand semantics
- Complex implementation
- Larger signal graph (72 substep instances regardless of mode)
- Needs testing to verify CPU savings

**Status**: PARTIALLY WORKING (2025-12-01)

**Breakthrough**: The gating pattern works when `i` is compile-time from seq:
```faust
// Working pattern - compile-time i
gated_substep(i, M_prev, H_prev, ...) = M_out, H_out, ...
with {
    clk = (i < steps_this_sample);  // i from seq is compile-time!
    physics_result = clk : ondemand(ja_physics(...));
    ...
};
seq(i, MAX_STEPS, gated_substep(i))  // pass i to function
```

**Working prototype**: `faust/dev/test_gated_substeps.dsp` compiles and runs with 72 substeps (K60 Ultra).

**Remaining question**: Does the dynamic cursor variation work? The current prototype uses `steps_this_sample = 72` as a compile-time constant. The full C++ scheduler behavior requires runtime `steps_this_sample` from cursor accumulation - this may trigger the same ondemand bug seen with runtime clocks + complex seq.

### Comparison

| Aspect | Option A (Simple) | Option B (Dynamic) |
|--------|-------------------|-------------------|
| Implementation | Straightforward | Complex |
| Ondemand usage | Mode selection (proven) | Substep gating (experimental) |
| Substeps/sample | Fixed (24/48/60) | Variable (cursor-based) |
| C++ parity | Partial | Full |
| Sound quality | Good | Potentially matches C++ |
| Risk | Low | High |

### Recommendation

1. **Implement Option A first** - proven pattern, immediate CPU savings
2. **Test Option B later** - once Option A is stable, experiment with dynamic gating
3. **Compare sound quality** - A/B test FAUST vs C++ to quantify differences

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

**Note**: The LUT optimization has now been applied to the C++ version — see `cpp_reference/JAHysteresisSchedulerLUT.*` for the implementation and `JAHysteresisSchedulerLUT_README.md` for integration instructions.

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

### LUT Responsiveness Enhancement (Active)

See [`docs/JA_LUT_NONLINEARITY.md`](JA_LUT_NONLINEARITY.md) for full analysis.

**Goal**: Achieve highest quality at affordable CPU (below C++ 11%), matching the dynamic feel of full-physics C++.

**Priority order**:
1. Pure JA physics: Dynamic α(M), k(dH), c(M) in substep 0
2. Pure JA physics: Multiple real substeps (2-3 instead of 1)
3. Workarounds: Envelope modulation of JA parameters
4. Heuristics: Index warping, interpolation correction (last resort)

### Bias Waveform Variations (Future)

Current: Pure sine bias oscillator.

**Potential exploration**:
- Asymmetric bias (different positive/negative excursions)
- Harmonic-rich bias (triangle, modified sine)
- These would require new LUT sets but could expand tonal palette

---

## File Structure

```
FAUST_FSM_TAPE/
├── faust/
│   ├── jahysteresis.lib              # Contribution-ready FAUST library (jah prefix)
│   ├── ja_lut_k*.lib                 # 10 mode-specific LUT libraries (K28-K2101)
│   ├── JAHysteresisLUT_K*.h          # C++ LUT headers (all 10 modes)
│   ├── rebuild_faust.sh              # Build script preserving plugin IDs
│   ├── dev/
│   │   ├── ja_streaming_bias_proto.dsp       # Working prototype (ba.if version)
│   │   ├── ja_streaming_bias_proto_OD_72.dsp # 72-substep ondemand prototype
│   │   └── test_gated_substeps.dsp           # Gated substeps experiment
│   ├── test/
│   │   ├── test_gated_substeps.dsp           # Ondemand gating tests
│   │   └── ja_lut_k*.lib                     # LUTs for test builds
│   └── examples/
│       └── jah_tape_demo.dsp         # Demo importing jahysteresis.lib
├── cpp_reference/
│   ├── JAHysteresisScheduler.*       # Original C++ scheduler (~11% CPU)
│   ├── JAHysteresisSchedulerLUT.*    # LUT-optimized C++ scheduler (<1% CPU)
│   └── JAHysteresisSchedulerLUT_README.md  # Integration guide
├── juce_plugin/
│   └── Source/
│       ├── JAHysteresisScheduler.h   # C++ reference implementation
│       └── JAHysteresisScheduler.cpp
├── scripts/
│   └── generate_ja_lut.py            # LUT generator (outputs .lib and .h)
├── tools/                            # Gitignored - clone separately
│   └── faust-ondemand/               # Dev fork with ondemand primitive
└── docs/
    ├── CURRENT_STATUS.md             # This file
    ├── JA_LUT_NONLINEARITY.md        # LUT responsiveness enhancement options
    ├── GRAME_ONDEMAND_BUG_REPORT.md  # Ondemand primitive bug report
    ├── LUT_RESTRUCTURE_PLAN.md       # Unified LUT proposal
    ├── JA_LUT_IMPLEMENTATION_PLAN.md # Original LUT design
    └── JA_Hysteresis_Optimization_Summary.md
```

**Note**: `jahysteresis.lib` is the library-ready version for GRAME contribution.
`dev/ja_streaming_bias_proto.dsp` is the working prototype kept as reference.

---

## Next Steps

### Immediate (Code)

1. **LUT Responsiveness**: Prototype dynamic α(M) + k(dH) in `ja_substep0` (see `JA_LUT_NONLINEARITY.md`)
2. If insufficient: Implement 2 real substeps + regenerate LUTs
3. Benchmark CPU and A/B test against full-physics C++

### Research

1. Validate responsiveness enhancements against C++ reference
2. Conduct harmonic imprint analysis for all 10 modes
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
2. ~~Timeline for the **Ondemand primitive**?~~ — Working! Dev fork in `tools/faust-ondemand/`
3. **Ondemand + runtime mode selection**: Is there a way to use `ondemand` for runtime mode switching with complex `seq` chains inside (full JA substeps), or does `ondemand` fundamentally require compile-time clocks for complex operations? See `docs/GRAME_ONDEMAND_BUG_REPORT.md` for details.
4. Best practices for contributing optimized libraries to faustlibraries?

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
