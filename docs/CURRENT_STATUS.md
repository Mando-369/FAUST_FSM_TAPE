# FAUST JA Hysteresis Library — Current Status

**Last updated**: 2025-12-10
**Collaborators**: Thomas Mandolini (OmegaDSP), GRAME (Stéphane Letz)

---

## Project Goal

Create a reusable **FAUST library (`jahysteresis.lib`)** implementing the Jiles-Atherton model of ferromagnetic hysteresis — a physically-based description relating magnetization (M) to applied field (H) — with phase-locked bias oscillator for analog tape emulation.

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
| 10 bias modes (K28-K2101) | Complete | LoFi to beyond-physical range (integer cycles) |
| FAUST prototype (ba.if) | Complete | `dev/ja_streaming_bias_proto.dsp` |
| FAUST prototype (ondemand) | Complete | `dev/ja_streaming_bias_proto_OD_72.dsp` |
| FAUST full-physics proto | Complete | `dev/lib_latest_proto/jahysteresislib_proto.dsp` (K96, 96 substeps, bias asymmetry) |
| FAUST multi-mode (ondemand) | Complete | `dev/lib_latest_proto/jahysteresislib_proto_OD_3_modes.dsp` (K96/K48/K24, only active computes) |
| FAUST library | In Progress | `jahysteresis.lib` (contribution-ready) |
| C++ reference (original) | Complete | `JAHysteresisScheduler` with ~2% CPU |
| C++ reference (LUT) | Complete | `JAHysteresisSchedulerLUT` with <1% CPU expected |

### Performance (M4 Max, Reaper, AU/VST3)

| Implementation | CPU | Notes |
|----------------|-----|-------|
| FAUST full-physics K96 (96 substeps) | ~2% | Via `seq` unrolling, sounds perfect |
| FAUST full-physics K72 (72 substeps) | ~2% | Previous version |
| C++ scheduler (original) | ~2% | Uses fractional substep accumulation |
| FAUST hybrid 50/50 (48 real + 48 LUT) | ~1.5-1.7% | Not worth it (see findings below) |
| FAUST + LUT (1 real + 95 LUT) | <1% | Fixed bias parameters |
| C++ + LUT | <1% | Ready for integration, see `cpp_reference/` |

### Achieved Breakthrough

**Key insight**: Only substep 0 has cross-sample dependency. Substeps 1..N-1 are deterministic given (M1, H_audio).

**Solution**: Precompute 2D LUT mapping `(M_in, H_audio) → (M_end, sumM_rest)` for the deterministic portion.

**Result**: Collapsed 66 JA physics evaluations to 1 + cheap bilinear interpolation.

### Multi-Mode Prototype with Ondemand (2025-12-09, updated 2025-12-11)

Prototype `dev/lib_latest_proto/jahysteresislib_proto_OD_3_modes.dsp` — 3 quality modes using ondemand:

- **Modes**: K92 (HQ), K44 (Standard), K28 (Eco) — **prime substeps per cycle for stability**
- **CPU optimization**: Only the active mode computes (via `ondemand` primitive)
- **Cycles**: All modes use 4 cycles × prime substeps (23/11/7 per cycle)
- **Build**: `cd faust/dev/lib_latest_proto && ./build_OD_3_modes.sh`
- **Plugin ID**: `e0a3`, Bundle: `com.grame.jahysteresislib_proto_OD_3_modes`

**Prime substep discovery (2025-12-11)**: Using prime numbers for substeps per cycle (7, 11, 23) eliminates noise floor flickering observed with non-prime counts. The non-repeating sampling pattern reduces coherent aliasing artifacts.

| Mode | Cycles | Substeps/Cycle | Total | Phase Step |
|------|--------|----------------|-------|------------|
| K92 (HQ) | 4 | 23 (prime) | 92 | 15.7° |
| K44 (Standard) | 4 | 11 (prime) | 44 | 32.7° |
| K28 (Eco) | 4 | 7 (prime) | 28 | 51.4° |

**Compensation systems** (all mode-dependent):
- **Mode comp**: K92=-0.3dB, K44=+0.8dB, K28=+2.7dB
- **Diff scale**: K92=2.53, K44=3.93, K28=1.81 (tuned for balanced harmonics)
- **Bias comp**: Piecewise linear per mode, scaled by bias_scale/11.0
- **Asym comp**: K92=-1.8dB, K44=-4.9dB, K28=-12.2dB at asym=0.5

**DC blocker**: 5 Hz, Q=0.7071 (Butterworth) — lowered from 10 Hz to reduce LF phase rotation.

**Finding**: K8 (4×2=8 substeps) and K12 (4×3=12 substeps) modes failed at high bias levels — too few samples per cycle causes instability. K24 (4×6=24) showed noise floor flickering. Minimum stable with clean noise floor is K28 (4×7=28).

### Wavelength Saturation (λ Tilt) — (2025-12-10, updated 2025-12-11)

**Major sonic enhancement**: Added frequency-dependent pre-saturation to simulate tape wavelength response.

**Physical basis**: In real tape recording, wavelength λ = tape_speed / frequency. Shorter wavelengths (higher frequencies) experience more saturation due to tape coating thickness loss. This is the dominant source of HF loss in studio-grade machines (per McKnight).

**Implementation**:
```faust
lambda_sat = fi.spectral_tilt(3, 200, 15000, lambda_tilt);
```

- **Filter**: `fi.spectral_tilt` — order 3, band 200-15000 Hz
- **Range**: -0.1 to +0.1 (reduced from ±0.5 — original range was too extreme)
- **Position**: Before JA stage (pre-saturation boost/cut)
- **Effect**: Positive values = HF boost before saturation = more HF saturation = "slower tape" character. Negative = opposite.

**Result**: Subtle but effective tonal shaping. The reduced range provides usable musical variation without extreme artifacts.

### Full-Physics Prototype (2025-12-09)

Prototype `dev/lib_latest_proto/jahysteresislib_proto.dsp` — production-ready full-physics implementation:

- **Mode**: K96 (4 cycles × 24 substeps = 96 total, integer cycles)
- **Physics**: Full JA computation for all 96 substeps via `seq(i, 96, ja_substep_seq)`
- **Phase continuity**: M, H, and phase fed back across samples (3-way feedback loop)
- **tanh**: Real `ma.tanh` (not fast_tanh with ±3 clamp)

**Stabilization** (prevents runaway at high drive):
- `diff_scale`: Soft clamp on (Man_e - M_prev) via `diff / (1 + |diff| * scale)`
- `sigma = 1e-3`: Prevents 1/(pin+σ) from spiking when pin crosses zero

**Bias Asymmetry** (2025-12-09):
- Adds 2nd harmonic to bias oscillator for even harmonic content (warmth)
- Formula: `bias_offset = sin(phase) + bias_asym * sin(2 * phase)`
- Range: 0.0 to 0.5 (beyond 0.5 inverts the waveform)
- Physically-based approach to adding even harmonics

**Gain compensation**:
- Drive compensation: inverse drive + 15.6 dB makeup for JA level drop
- Bias compensation: piecewise linear based on bias_amp (reference: bias_amp=4.4 = 0dB)

**UI groups** (OD_3_modes prototype):
- [00] QUALITY: Mode selection (K92 HQ / K44 Standard / K28 Eco)
- [01] GAIN: Input, Output, Drive, Mix
- [02] BIAS: Level (0.01-1.0), Scale, Asym
- [03] TAPE: λ Tilt (-0.1 to +0.1)
- [04] PHYSICS: Ms, a, k, c, alpha

**Parameters**:
- Input/Output: -24 to +24 dB
- Drive range: -18 to +29 dB
- Bias Level: 0.01 to 1.0 (min raised to prevent instability)
- Bias Asym: 0.0 to 0.5
- DC blocker: 5 Hz, Q=0.7071 (Butterworth)

### Hybrid LUT Experiment (2025-12-09) — NOT RECOMMENDED

Tested a 50/50 hybrid approach: 48 real substeps + 48 LUT substeps.

**Goal**: Reduce CPU while maintaining parameter flexibility for the first half of substeps.

**Implementation**:
- First 48 substeps computed with real JA physics (responds to bias_asym, diff_scale)
- Remaining 48 substeps looked up from 2D LUT (RK4-generated)
- LUT generator updated with diff_scale=1.0 and sigma=1e-3 to match DSP

**Issues discovered**:
1. **H range too narrow**: Initial LUT with H∈[-1,1] caused 3dB gain difference at high drive (22dB). Fixed by expanding to H∈[-30,30].
2. **Catmull-Rom overhead**: 16-point interpolation per lookup (4×4 grid) × 2 LUTs = 32 rdtable reads plus interpolation math.
3. **Minimal CPU savings**: Only 0.3-0.5% reduction vs full physics (~1.5-1.7% vs ~2%), not the expected 50%.

**Why hybrid doesn't pay off**:
- Trading 48 substeps for 2 LUT lookups (with Catmull-Rom) is poor value
- The 1-real-substep LUT approach saves much more (trades 95 substeps for 2 lookups)
- But 1-real-substep loses parameter flexibility

**Conclusion**: For production, use either:
1. **Full physics K96** (~2% CPU) — sounds perfect, full parameter control
2. **1-real + LUT** (<1% CPU) — fixed bias parameters, massive CPU savings

The 50/50 hybrid sits in an awkward middle ground with neither benefit.

**Test files**: `faust/test/test_mode3_bias_asym_hybrid.dsp`, `faust/test/ja_lut_k96.lib`

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
- `faust/GRAME_BUG_ONDEMAND/ja_streaming_bias_proto_od.dsp` — LUT-based, runtime mode selection
- `faust/dev/dev_old/ja_streaming_bias_proto_OD_72.dsp` — Full 72 substeps, compile-time gating

**For GRAME/Stéphane**: See `faust/GRAME_BUG_ONDEMAND/GRAME_ONDEMAND_BUG_REPORT.md` for full details. Question: Is there a way to achieve runtime mode selection with full substep computation using `ondemand`, or is LUT the only viable approach?

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

### 2. Harmonic Imprint Research (Priority: High) — REVISED

**Finding**: Half-integer cycles cause audible 12kHz bias tone from residual phase accumulation. Reverted to integer cycles.

**Current approach**: Integer cycles prevent bias leakage. Harmonics come from JA physics itself, stabilized with diff_scale soft clamp.

| Mode | Cycles | Substeps | Character |
|------|--------|----------|-----------|
| K28 | 1 | 28 | Maximum grit |
| K45 | 2 | 45 | Crunchy, lo-fi |
| K63 | 3 | 63 | Classic tape |
| K99 | 4 | 99 | Smooth warmth |
| K121 | 5 | 121 | Standard (default) |
| K187 | 8 | 187 | High quality |
| K253 | 11 | 253 | Very detailed |
| K495 | 22 | 495 | Ultra detailed |
| K1045 | 47 | 1045 | Extreme |
| K2101 | 95 | 2101 | Beyond physical |

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

**Status**: Both FAUST and C++ full-physics implementations run at ~2% CPU.

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

**Prototype**: `faust/dev/dev_old/ja_streaming_bias_proto_OD_24.dsp`

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
│   │   ├── lib_latest_proto/              # Latest full-physics prototype
│   │   │   └── jahysteresislib_proto.dsp  # K60 Ultra (C++ match)
│   │   └── dev_old/                       # Archived prototypes
│   ├── test/
│   │   ├── test_var_subst_lut.dsp      # Variable substep LUT test
│   │   ├── ja_lut_k*.lib               # LUTs for test builds
│   │   └── test_old/                   # Archived tests
│   ├── GRAME_BUG_ONDEMAND/             # Ondemand bug reproduction files
│   │   ├── GRAME_ONDEMAND_BUG_REPORT.md
│   │   ├── ja_streaming_bias_proto_od.dsp      # Working (LUT-based)
│   │   └── jaStreamingBiasProtoOD24.dsp        # Failing (complex seq)
│   └── examples/
│       └── jah_tape_demo.dsp         # Demo importing jahysteresis.lib
├── cpp_reference/
│   ├── JAHysteresisScheduler.*       # Original C++ scheduler (~2% CPU)
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
    └── VARIABLE_SUBSTEP_LUT_PLAN.md  # Variable substep LUT design
```

**Note**: `jahysteresis.lib` is the library-ready version for GRAME contribution.

---

## Next Steps

### Immediate (Code)


### Documentation

1. Define `jahysteresis.lib` public API
2. Write usage examples for GRAME review
3. Document integration path back to FSM_TAPE

### Future Enhancements (from FSM Analysis)

#### Tape Type Presets
Implement JA parameter presets for different tape formulations:
- **AMPEX 456** — warm, thick, forgiving (Ms=250-330, a=650-850, k=260-340)
- **AMPEX 499** — high-output, cleaner (Ms=330-420, a=800-1000, k=200-280)
- **Quantegy GP9** — super high-output, near-digital dynamics (Ms=400-500, a=900-1400, k=160-240)
- **BASF/EMTEC 900** — European mastering, refined HF (Ms=350-440, a=900-1200, k=180-260)
- **Sony Metal Particle** — aggressive, punchy (Ms=450-600, a=600-900, k=300-450)
- **Consumer Ferric (Type I)** — crunchy, lo-fi (Ms=180-260, a=400-600, k=260-400)
- **Consumer Chrome (Type II)** — nostalgic cassette (Ms=240-320, a=450-700, k=220-330)

#### Machine Presets
Implement JA parameter presets for specific tape machines:
| Machine | Ms | a | k | c | α | biasLvl | biasAsym |
|---------|-----|-----|-----|------|-------|---------|----------|
| Studer A800 | 320 | 750 | 300 | 0.18 | 0.015 | 0.42 | 0.08 |
| Studer A810 | 360 | 900 | 240 | 0.24 | 0.012 | 0.50 | 0.05 |
| Ampex ATR-102 | 380 | 950 | 220 | 0.26 | 0.011 | 0.52 | 0.04 |
| Otari MX5050 | 300 | 700 | 320 | 0.20 | 0.017 | 0.38 | 0.12 |

#### Macro Controls (UI Simplification)
Create a FAUST macro block mapping `Drive / Color / Bias / Character` → JA parameters:
- **DRIVE (0-100%)** — controls magnetic intensity (drive_db, k, c, α)
- **COLOR (Warm↔Bright)** — controls spectral tilt via a_density and bias_asym
- **BIAS (Under↔Over)** — classic tape calibration control
- **CHARACTER (0-1)** — morphs between machine presets (A800→A810→ATR-102→MX5050)

#### Additional Tape Emulation Features
For T805-like behavior:
1. **Head bump simulation** — low-frequency resonance characteristic of tape heads
2. **Reproduce EQ integrator** — playback head equalization curve
3. **Tape speed modes** — 3.75, 7.5, 15, 30 IPS with different frequency responses
4. **FM-style bias head leakage** — subtle bias carrier bleed-through
5. ~~**Dynamic wavelength saturation (λ response)**~~ — ✅ DONE (2025-12-10) — implemented as λ Tilt using `fi.spectral_tilt(3, 200, 15000, alpha)`



## Questions for GRAME




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
