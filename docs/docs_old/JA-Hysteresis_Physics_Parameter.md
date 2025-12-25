# JA Hysteresis Physics Parameters

## Current Default Parameters

```faust
Ms              = 320.0;   // Saturation magnetization
a_density       = 720.0;   // Exchange parameter
k_pinning       = 280.0;   // Pinning coefficient
c_reversibility = 0.18;    // Reversibility
alpha_coupling  = 0.015;   // Domain coupling
```

---

## Parameter Analysis

### Ms (Saturation Magnetization)
- **Value**: 320.0
- Typical range for audio tape: 300-600 mT
- Lower Ms = less pronounced saturation effects

### a_density (Exchange Parameter)
- **Value**: 720.0
- Influences material's response to magnetic field changes
- Higher values = faster transitions (can sound abrupt)
- Lower values = more gradual, natural-sounding saturation

### k_pinning (Pinning Coefficient)
- **Value**: 280.0
- Controls pinning effects (microphonics, resonance)
- Lower k = less pronounced pinning, less "organic" sound
- Higher k = more character, natural tape feel

### c_reversibility
- **Value**: 0.18
- Affects how easily magnetization direction changes
- Lower value = harder reversal, prevents overly aggressive non-linearities
- Current value is reasonable for tape simulation

### alpha_coupling (Domain Coupling)
- **Value**: 0.015
- Controls cross-coupling between magnetic domains
- Low value = minimizes domain interactions (realistic)
- Slightly higher = more non-linearities without instability

---

## Recommended Adjustments

For more realistic and euphonic tape saturation:

| Parameter | Current | Suggested | Effect |
|-----------|---------|-----------|--------|
| Ms | 320.0 | 480.0 | More pronounced saturation |
| a_density | 720.0 | 360.0 | Smoother transitions |
| k_pinning | 280.0 | 420.0 | More organic character |
| c_reversibility | 0.18 | 0.12-0.18 | Harder reversal (optional) |
| alpha_coupling | 0.015 | 0.02-0.03 | More dynamics (optional) |

---

## Alternative Parameter Set

```faust
// ===== Adjusted Physics Parameters =====
Ms              = 480.0;
a_density       = 360.0;
k_pinning       = 420.0;
c_reversibility = 0.15;
alpha_coupling  = 0.02;
```

---

## Additional Considerations

### Integration Method
- Euler: faster but less accurate
- RK4: better numerical stability and accuracy
- Higher-order methods recommended for high-fidelity applications

### Fast-Tanh Approximation
- Ensure smoothness while avoiding clipping artifacts
- Current rational approximation is reasonable
- `ma.tanh` (real tanh) preferred when CPU allows

---

## Conclusion

The default parameters are a solid starting point. Adjustments depend on desired character:
- **Warmer/vintage**: increase k_pinning, reduce a_density
- **More saturation**: increase Ms
- **More dynamics**: slightly increase alpha_coupling

Experiment while monitoring the resulting sound to find optimal values for your application.
