# JA LUT Nonlinearity Enhancement Options

**Created**: 2025-12-02
**Status**: Research / Discussion

---

## Goal & Constraints

**Primary Goal**: Create a high-quality `jahysteresis.lib` FAUST library for Jiles-Atherton magnetic hysteresis.

**Constraint**: All enhancements must be **pure JA physics** — no external effects, post-processing, or heuristics outside the JA model.

**The Challenge**:
- Full FAUST physics computation: ~24% CPU (unusable)
- C++ full physics: ~11% CPU (reference for quality)
- LUT optimization: <1% CPU (usable but static)
- **Target**: Highest quality at affordable CPU (anything below C++ 11% is acceptable)

**What's In Scope**:
- JA model parameters: Ms, a, k, c, α
- Making these parameters signal/state-dependent (physically motivated)
- Computing more real JA substeps before LUT lookup
- Bias oscillator behavior (phase, substep count)
- **Workarounds** that approximate the dynamic behavior of full JA physics

**Priority Order**:
1. Pure JA physics enhancements (Options 1-4)
2. Workarounds that recreate full-physics behavior (e.g., envelope modulation to approximate dynamic response)
3. Heuristics only if needed (index warping, noise, etc.)

---

## The Problem

The 2D LUT optimization reduces CPU from ~24% to <1% by precomputing substeps 1-N. However:

1. **Bilinear interpolation is linear** — The LUT samples are accurate, but interpolation between samples smooths out microvariation
2. **Deterministic output** — Same `(M1, H_audio)` always produces identical output
3. **Fixed physics parameters** — LUT is computed for static Ms, a, k, c, α values

The real JA computation has subtle nonlinear variation at every substep. The LUT captures the *average* behavior but loses micro-dynamics.

---

## Current Architecture

```
Sample N:
├── M_prev (from sample N-1)
├── H_audio (input signal)
│
├── Substep 0: REAL JA PHYSICS ──→ M1, H1
│   └── ja_substep0(bias_lut(0), M_prev, H_prev, H_audio)
│
├── Substeps 1-N: LUT LOOKUP ──→ M_end, sum_M_rest
│   └── bilinear_interp(LUT, M1, H_audio)
│
└── Output: Mavg = (M1 + sum_M_rest) / N
```

**What's already "live":**
- Substep 0 uses full JA physics with real `ma.tanh`
- M_prev feedback creates inter-sample dependency
- Half-integer bias cycles introduce even harmonics

**What's static:**
- Substeps 1-N are pure table lookup
- Physics parameters are constants
- Interpolation is linear

---

## Enhancement Options — Pure JA Physics

These options modify the JA model itself and are physically motivated.

### Option 1: Multiple Real Substeps (Most Direct)

**Concept**: Compute 2-3 real JA substeps before LUT lookup.

**Current**: 1 real substep + LUT for substeps 1-N
**Proposed**: 2-3 real substeps + LUT for remainder

```faust
// Current: only M1 is "live"
M1_H1 = ja_substep0(bias_lut(0), M_prev, H_prev, H_audio);
M1 = ba.selector(0, 2, M1_H1);
M_end = ja_lookup_m_end(M1, H_audio);
sum_M_rest = ja_lookup_sum_m_rest(M1, H_audio);
Mavg = (M1 + sum_M_rest) * inv_N;

// Proposed: M1, M2, M3 are "live"
M1_H1 = ja_substep0(bias_lut(0), M_prev, H_prev, H_audio);
M1 = ba.selector(0, 2, M1_H1);
H1 = ba.selector(1, 2, M1_H1);

M2_H2 = ja_substep0(bias_lut(1), M1, H1, H_audio);
M2 = ba.selector(0, 2, M2_H2);
H2 = ba.selector(1, 2, M2_H2);

M3_H3 = ja_substep0(bias_lut(2), M2, H2, H_audio);
M3 = ba.selector(0, 2, M3_H3);

// LUT for substeps 3-N (needs new LUT generation)
M_end = ja_lookup_m_end_from3(M3, H_audio);
sum_M_rest = ja_lookup_sum_m_rest_from3(M3, H_audio);
Mavg = (M1 + M2 + M3 + sum_M_rest) * inv_N;
```

**Pros:**
- Purest approach — just more real physics
- No approximations or heuristics
- Maintains exact JA model fidelity

**Cons:**
- 2-3x substep CPU cost (still negligible vs. full computation)
- Requires LUT regeneration (Python script modification)
- Diminishing returns after 3-4 substeps

**Implementation effort**: Medium (LUT generator update + library code)

---

### Option 2: Dynamic α Coupling (Physically Motivated)

**Concept**: In real ferromagnets, interdomain coupling varies with magnetization level.

**Physics basis**: Higher magnetization → domains more aligned → stronger coupling → harder to change further.

```faust
// In ja_substep0, replace static alpha_norm with dynamic version
ja_substep0_dynamic(bias_val, M_prev, H_prev, H_audio) = M1, H1
with {
    // Dynamic alpha: increases with magnetization squared
    alpha_modulation = 0.2;  // tunable: 0 = static, higher = more dynamic
    alpha_dynamic = alpha_norm * (1.0 + alpha_modulation * M_prev * M_prev);

    H1 = H_audio + bias_amp * bias_val;
    dH = H1 - H_prev;
    He = H1 + alpha_dynamic * M_prev;  // use dynamic alpha

    // ... rest of JA physics unchanged
};
```

**Pros:**
- Physically motivated — real materials exhibit this
- Affects only the "live" substep (no LUT change needed)
- Single tunable parameter

**Cons:**
- Subtle effect — may not be perceptually significant
- Changes the "character" slightly from reference C++

**Implementation effort**: Low (modify `ja_substep0` only)

---

### Option 3: Slew-Dependent Pinning (Rate-Responsive)

**Concept**: Domain wall pinning (k) responds differently to fast vs. slow field changes.

**Physics basis**: Rapid field changes encounter different energy barriers than slow changes. This creates rate-dependent hysteresis width.

```faust
// In ja_substep0, make k respond to input rate of change
ja_substep0_slew(bias_val, M_prev, H_prev, H_audio, H_audio_prev) = M1, H1
with {
    // Detect input slew rate
    dH_audio = abs(H_audio - H_audio_prev);

    // Dynamic pinning: faster changes → more resistance
    k_slew_sensitivity = 0.5;  // tunable
    k_dynamic = k_norm * (1.0 + k_slew_sensitivity * dH_audio);

    // Use k_dynamic in pinning term
    dir = ba.if(dH >= 0.0, 1.0, -1.0);
    pin = dir * k_dynamic - alpha_norm * (Man_e - M_prev);  // dynamic k

    // ... rest of JA physics
};
```

**Pros:**
- Creates input-responsive behavior
- Transients feel different from sustained tones
- Physically plausible

**Cons:**
- Requires passing previous H_audio sample
- Adds complexity to substep function signature
- Effect may be subtle at audio rates

**Implementation effort**: Low-Medium (modify substep + signal routing)

---

### Option 4: Dynamic Reversibility (c Parameter)

**Concept**: The reversibility parameter `c` could vary with signal level.

**Physics basis**: At low magnetization, domain wall motion is more reversible. At high magnetization near saturation, irreversible rotation dominates.

```faust
// Dynamic c: more reversible at low M, less at high M
c_modulation = 0.3;  // tunable
c_dynamic = c_norm * (1.0 - c_modulation * abs(M_prev));

// Use in JA physics
denom = 1.0 - c_dynamic * alpha_norm * dMan_dH;
dMdH = (c_dynamic * dMan_dH + (Man_e - M_prev) * inv_pin) * inv_denom;
```

**Pros:**
- Affects the balance between reversible/irreversible magnetization
- Creates level-dependent harmonic content
- Physically motivated

**Cons:**
- Changes the fundamental hysteresis loop shape
- May drift from reference C++ behavior

**Implementation effort**: Low

---

## Workaround & Heuristic Options — Lower Priority

The following options are not pure JA physics but may help approximate the dynamic behavior of full JA computation. Try pure JA options (1-4) first.

### Option 5: Nonlinear LUT Index Warping

**Concept**: Apply nonlinear transformation to LUT indices before lookup.

**Rationale**: The JA response is not uniformly distributed across the (M, H) space. Nonlinear index warping can better match the actual distribution.

```faust
// Current: linear index mapping
M_idx = (M1 - M_MIN) / (M_MAX - M_MIN) * (M_SIZE - 1);
H_idx = (H_audio - H_MIN) / (H_MAX - H_MIN) * (H_SIZE - 1);

// Proposed: nonlinear warping
warp_amount = 0.1;  // tunable

// Sigmoid-like warping concentrates resolution near zero
M_normalized = (M1 - M_MIN) / (M_MAX - M_MIN);  // 0 to 1
M_warped = M_normalized + warp_amount * sin(M_normalized * ma.PI);
M_idx = M_warped * (M_SIZE - 1);

// Or: tanh warping for more resolution near center
M_idx = (ma.tanh(M_normalized * 2 - 1) * 0.5 + 0.5) * (M_SIZE - 1);
```

**Pros:**
- No physics changes — just lookup modification
- Can improve accuracy where it matters most
- Computationally cheap

**Cons:**
- Heuristic, not physics-based
- May introduce artifacts at warp boundaries
- Requires tuning

**Implementation effort**: Low (modify lookup functions only)

---

### Option 6: Interpolation Error Correction

**Concept**: Add a nonlinear correction term to compensate for bilinear interpolation smoothing.

```faust
// After bilinear interpolation
M_end_linear = bilinear_lookup(M1, H_audio);

// Compute correction based on position in LUT cell
M_frac = frac(M_idx);  // fractional part of index
H_frac = frac(H_idx);

// Correction peaks at cell centers, zero at corners
cell_center_dist = (M_frac - 0.5) * (H_frac - 0.5);
correction_strength = 0.02;  // tunable
correction = correction_strength * cell_center_dist * M_end_linear;

M_end = M_end_linear + correction;
```

**Pros:**
- Directly addresses the interpolation linearization issue
- Can be tuned per-mode if needed

**Cons:**
- Heuristic correction, not physics-based
- May introduce its own artifacts
- Requires careful tuning

**Implementation effort**: Low

---

### Option 7: Stochastic Microvariation

**Concept**: Add controlled noise to LUT lookup to simulate computational microvariation.

```faust
// Add tiny noise to lookup indices
noise_amount = 0.002;  // very small — fraction of one LUT cell
M_idx_noisy = M_idx + no.noise * noise_amount * M_SIZE;
H_idx_noisy = H_idx + no.noise * noise_amount * H_SIZE;
```

**Pros:**
- Reintroduces the "aliveness" of real computation
- Breaks up the deterministic feel
- Very cheap

**Cons:**
- Not physics-based — purely perceptual
- Could introduce unwanted noise at low levels
- May not survive A/B testing

**Implementation effort**: Very low

---

### Option 8: Envelope-Modulated Physics

**Concept**: Track signal envelope and modulate physics parameters accordingly.

```faust
// Envelope follower
envelope = abs(H_audio) : si.smooth(ba.tau2pole(0.01));

// Modulate k based on envelope (compression-like behavior)
k_envelope_mod = 0.3;
k_dynamic = k_norm * (1.0 + k_envelope_mod * envelope);

// Or modulate alpha
alpha_envelope_mod = 0.2;
alpha_dynamic = alpha_norm * (1.0 + alpha_envelope_mod * envelope);
```

**Pros:**
- Creates input-level-dependent response
- Louder signals behave differently than quiet ones
- Physically plausible (thermal effects, etc.)

**Cons:**
- Adds state (envelope follower)
- May be considered "effect-like" rather than pure physics

**Implementation effort**: Low-Medium

---

### Option 9: Continuous Phase + Variable Substep Count (C++ Parity)

**Concept**: Replicate the C++ scheduler's fractional cursor and continuous phase behavior.

**C++ Reference** (`JAHysteresisScheduler.cpp`):
```cpp
// Fractional cursor accumulation - substep count VARIES per sample
substepCursor += biasCyclesPerSample * static_cast<double>(substepsPerCycle);
int stepsTaken = static_cast<int>(std::floor(substepCursor));
substepCursor -= static_cast<double>(stepsTaken);  // carry fraction forward

// Phase is CONTINUOUS across samples
double phase = biasPhase;
for (int i = 0; i < stepsTaken; ++i) {
    const double midpoint = std::fmod(phase + substepPhase * 0.5, kTwoPi);
    const double biasOffset = std::sin(midpoint);  // real-time sin()
    executeSubstep(biasOffset, HAudio, magnetisationSum);
    phase += substepPhase;
}

// Advance phase by leftover fractional substep
phase += substepCursor * substepPhase;
biasPhase = phase;  // persist for next sample

// Guarantee minimum 1 substep
if (stepsTaken == 0) {
    executeSubstep(...);
    stepsTaken = 1;
}
return magnetisationSum / static_cast<double>(stepsTaken);
```

**Key C++ behaviors missing from FAUST LUT:**

| Aspect | C++ Scheduler | FAUST LUT |
|--------|---------------|-----------|
| Substeps per sample | Variable (65, 66, 67...) | Fixed (e.g., 121) |
| Bias phase | Continuous across samples | Resets each sample |
| Fractional carry | Yes, cursor persists | No |
| Bias sampling | Midpoint of interval | Start of interval* |
| sin() computation | Real-time `std::sin()` | Table lookup |

*Note: The backup prototype `ja_streaming_bias_proto_backup.dsp` already implements midpoint sampling via `(n + 0.5) * dphi`.

**FAUST Implementation Approach**:
```faust
// State variables (via feedback)
// - biasPhase: continuous oscillator phase
// - substepCursor: fractional accumulator

ja_hysteresis_continuous(H_audio) = output
letrec {
    // Accumulate cursor (e.g., 3.0 cycles * 22 points = 66.0 per sample)
    target = biasCyclesPerSample * substepsPerCycle;

    // How many substeps THIS sample? (varies: 65, 66, 67, 66...)
    'cursor_new = cursor + target;
    steps_this_sample = int(floor(cursor_new)) - int(floor(cursor));
    steps_clamped = max(1, steps_this_sample);  // minimum 1

    // Carry fraction to next sample
    'cursor = cursor_new - floor(cursor_new);

    // Advance phase by fractional leftover
    'phase = fmod(phase + steps_clamped * substepPhase +
                  frac(cursor_new) * substepPhase, 2*PI);
}
with {
    cursor = cursor';
    phase = phase';

    // Execute steps_clamped substeps with continuous phase
    // ... (complex: need dynamic loop count)
};
```

**Challenge**: FAUST's `seq(i, N, ...)` requires compile-time constant N. Variable substep count requires either:
1. Fixed maximum with conditional gating (via `ondemand`)
2. Multiple pre-compiled paths for different step counts

**Pros:**
- Exact C++ behavior replication
- Creates the micro-variation that makes C++ feel "alive"
- Physically accurate phase continuity

**Cons:**
- Complex FAUST implementation
- May require `ondemand` primitive for efficiency
- Difficult to combine with LUT optimization

**Implementation effort**: High

**Status**: Partially explored in `faust/dev/ja_streaming_bias_proto_backup.dsp` which implemented midpoint sampling and C++ substep counts, but not the fractional cursor or continuous phase.

---

## Comparison Matrix

| Option | Type | Responsiveness | CPU Cost | Implementation | Risk |
|--------|------|----------------|----------|----------------|------|
| 1. Multiple Real Substeps | Pure JA | High | +2-3 substeps | Medium | Low |
| 2. Dynamic α(M) | Pure JA | Medium | Negligible | Low | Low |
| 3. Slew-Dependent k(dH) | Pure JA | High | Negligible | Low-Medium | Low |
| 4. Dynamic c(M) | Pure JA | Medium | Negligible | Low | Medium |
| 9. Continuous Phase | Pure JA | Excellent | High (no LUT) | High | Medium |
| 8. Envelope Modulation | Workaround | High | Low | Low-Medium | Medium |
| 5. Nonlinear Index Warp | Heuristic | Medium | Negligible | Low | Medium |
| 6. Interpolation Correction | Heuristic | Low | Negligible | Low | Medium |
| 7. Stochastic Noise | Heuristic | Low | Negligible | Very Low | Medium |

---

## Recommendations

### Recommended Implementation Order

**Phase 1 — Pure JA Physics (try first):**

1. **Option 2 + 3**: Dynamic α(M) and k(dH) in `ja_substep0`
   - Zero CPU cost, physically motivated
   - No LUT regeneration needed
   - Test if this adds sufficient responsiveness

2. **Option 1**: 2-3 real substeps instead of 1
   - Small CPU increase (+2-3 substeps)
   - Requires LUT regeneration (from M2 or M3)
   - More "live" JA physics = more responsive

3. **Option 4**: Dynamic c(M) if needed
   - Combine with Options 2+3
   - Changes reversibility based on magnetization

**Phase 2 — Workarounds (if Phase 1 insufficient):**

4. **Option 8**: Envelope modulation of JA parameters
   - Track signal envelope, modulate α/k/c
   - Approximates how full physics responds to dynamics

**Phase 3 — Heuristics (last resort):**

5. **Options 5-7**: Index warping, interpolation correction, noise
   - Only if pure JA + workarounds don't achieve goal

---

## Test Methodology

To evaluate improvements:

1. **A/B Comparison**: Compare LUT version vs. full computation (all real substeps)
2. **Harmonic Analysis**: Measure THD and harmonic distribution
3. **Transient Response**: Analyze attack/release behavior with impulses
4. **Frequency Sweep**: Check for artifacts across the spectrum
5. **Listening Tests**: Subjective evaluation on musical material

---

## Next Steps

1. [ ] Prototype Option 2 + 3 in `ja_substep0`
2. [ ] Benchmark against current implementation
3. [ ] If needed, update LUT generator for Option 1
4. [ ] Conduct A/B listening tests
5. [ ] Document chosen approach in `jahysteresis.lib`

---

## C++ Reference Implementation Details

The C++ `JAHysteresisScheduler` (`cpp_reference/JAHysteresisScheduler.cpp`) serves as the reference for "alive" behavior.

### Core Algorithm

```cpp
double JAHysteresisScheduler::process(double HAudio) noexcept
{
    double magnetisationSum = 0.0;

    // FRACTIONAL CURSOR: accumulates across samples
    substepCursor += biasCyclesPerSample * static_cast<double>(substepsPerCycle);
    int stepsTaken = static_cast<int>(std::floor(substepCursor));
    substepCursor -= static_cast<double>(stepsTaken);  // keep fraction

    double phase = biasPhase;  // CONTINUOUS phase

    for (int i = 0; i < stepsTaken; ++i)
    {
        // MIDPOINT sampling within substep interval
        const double midpoint = std::fmod(phase + substepPhase * 0.5, kTwoPi);
        const double biasOffset = std::sin(midpoint);  // real-time sin()
        executeSubstep(biasOffset, HAudio, magnetisationSum);
        phase += substepPhase;
    }

    // Advance by leftover FRACTIONAL substep
    phase += substepCursor * substepPhase;
    biasPhase = phase;  // persist for next sample

    // MINIMUM 1 substep guarantee
    if (stepsTaken == 0) {
        executeSubstep(...);
        stepsTaken = 1;
    }

    return magnetisationSum / static_cast<double>(stepsTaken);
}
```

### JA Physics (executeSubstep)

```cpp
void JAHysteresisScheduler::executeSubstep(double biasOffset,
                                           double HAudio,
                                           double& magnetisationSum) noexcept
{
    const double HNew = HAudio + biasAmplitude * biasOffset;
    const double dH = HNew - HPrev;
    const double He = HNew + alphaNorm * MPrev;

    const double xMan = He * invANorm;
    const double ManE = fastTanh(xMan);  // Padé approximant
    const double ManE2 = ManE * ManE;
    const double dMan_dH = (1.0 - ManE2) * invANorm;

    const double dir = (dH >= 0.0) ? 1.0 : -1.0;
    const double pin = dir * kNorm - alphaNorm * (ManE - MPrev);
    const double invPin = 1.0 / (pin + 1.0e-6);

    const double denom = 1.0 - cNorm * alphaNorm * dMan_dH;
    const double invDenom = 1.0 / (denom + 1.0e-9);
    const double dMdH = (cNorm * dMan_dH + (ManE - MPrev) * invPin) * invDenom;
    const double dMStep = dMdH * dH;

    const double MNew = std::clamp(MPrev + dMStep, -1.0, 1.0);

    magnetisationSum += MNew;
    MPrev = MNew;
    HPrev = HNew;
}
```

### fastTanh Approximation

```cpp
double JAHysteresisScheduler::fastTanh(double x) const noexcept
{
    const double clamped = std::clamp(x, -3.0, 3.0);
    const double x2 = clamped * clamped;
    return clamped * (27.0 + x2) / (27.0 + 9.0 * x2);  // Padé [3,3]
}
```

### Mode Configuration

```cpp
// Mode determines cycles per sample and substeps per cycle
switch (currentMode) {
    case Mode::K32:
        biasCyclesPerSample = 2.0;
        // Eco: 16, Normal: 18, Ultra: 20 points/cycle
        break;
    case Mode::K48:
        biasCyclesPerSample = 3.0;
        // Eco: 16, Normal: 18, Ultra: 19 points/cycle
        break;
    case Mode::K60:
        biasCyclesPerSample = 3.0;
        // Eco: 20, Normal: 22, Ultra: 24 points/cycle
        break;
}
```

### Physics Parameters

```cpp
struct PhysicsParams {
    double Ms = 320.0;           // Saturation magnetization
    double aDensity = 720.0;     // Domain density
    double kPinning = 280.0;     // Pinning strength
    double cReversibility = 0.18; // Reversibility factor
    double alphaCoupling = 0.015; // Interdomain coupling
};
```

---

## Previous FAUST Experiments

### ja_streaming_bias_proto_backup.dsp

Located at `faust/dev/ja_streaming_bias_proto_backup.dsp`, this prototype attempted to match C++ behavior:

**What was implemented:**
- Midpoint bias sampling: `sin((float(ba.period(n)) + 0.5) * dphi)`
- C++ substep counts: K32=36, K48=54, K60=66 (Normal quality)
- C++ fastTanh approximation: `t * (27.0 + x2) / (27.0 + 9.0 * x2)`
- Multiple modes via `ba.if` selection

**What was NOT implemented:**
- Fractional cursor accumulation (variable substep count)
- Continuous phase across samples
- Real-time sin() (uses rdtable instead)

**Result:** Improved but still not matching C++ "aliveness" — the deterministic fixed-count nature remains.

---

## References

- Jiles, D.C. and Atherton, D.L. (1986) "Theory of ferromagnetic hysteresis"
- Current implementation: `faust/jahysteresis.lib`
- LUT generator: `scripts/generate_ja_lut.py`
- C++ reference: `cpp_reference/JAHysteresisScheduler.cpp`
- Previous experiment: `faust/dev/ja_streaming_bias_proto_backup.dsp`
