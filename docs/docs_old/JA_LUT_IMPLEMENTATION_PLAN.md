# JA Hysteresis 2D LUT Implementation Plan

## The Core Insight

From your research, the key observation is:

```
Substep 0: depends on (M_prev, H_prev) from PREVIOUS sample
Substeps 1-65: DETERMINISTIC given (M1, HAudio)
```

Since bias is phase-locked and HAudio is constant within a sample, substeps 1-65 always produce the same result for the same (M1, HAudio) input.

## The Optimization

**Before:** 66 full JA physics evaluations per sample
**After:** 1 real JA substep + 1 LUT lookup with interpolation

### Per-sample flow:

```
1. Compute substep 0 with full physics → get M1
2. LUT lookup: (M1, HAudio) → (M_end, sumM_rest)
3. Mavg = (M1 + sumM_rest) / N
4. Update: MPrev = M_end, HPrev = HAudio + biasAmp * b_last
```

## FAUST Implementation Challenges

### Challenge 1: FAUST rdtable is 1D
Solution: Flatten 2D table, compute linear index manually

```faust
// 2D to 1D index
idx_2d(m_idx, h_idx, h_size) = m_idx * h_size + h_idx;
```

### Challenge 2: Bilinear interpolation
Solution: 4 lookups + weighted blend

```faust
bilinear(table, m, h, m_size, h_size) = result
with {
    // Quantize to grid indices
    m_idx = int(floor(m * (m_size - 1)));
    h_idx = int(floor(h * (h_size - 1)));

    // Fractional parts for interpolation
    m_frac = m * (m_size - 1) - m_idx;
    h_frac = h * (h_size - 1) - h_idx;

    // 4 corner lookups
    v00 = table(idx_2d(m_idx, h_idx, h_size));
    v01 = table(idx_2d(m_idx, h_idx + 1, h_size));
    v10 = table(idx_2d(m_idx + 1, h_idx, h_size));
    v11 = table(idx_2d(m_idx + 1, h_idx + 1, h_size));

    // Bilinear blend
    result = v00 * (1-m_frac) * (1-h_frac) +
             v01 * (1-m_frac) * h_frac +
             v10 * m_frac * (1-h_frac) +
             v11 * m_frac * h_frac;
};
```

### Challenge 3: LUT Generation at Init Time
FAUST can generate tables at init using `rdtable` with a generator function.
But generating a 2D table that runs the full JA physics is complex.

**Option A: Precompute externally**
- Generate LUT values in Python/C++
- Embed as `waveform{}` literals in FAUST

**Option B: Generate in FAUST init**
- Use nested `ba.period` counters
- Run substeps 1-65 for each (M, H) grid point
- This is complex but keeps everything in one file

## Recommended Approach: Option A (External Precomputation)

### Step 1: Python script to generate LUT
```python
# generate_ja_lut.py
import numpy as np

# Grid parameters
M_SIZE = 65   # -1 to +1
H_SIZE = 129  # reasonable audio range

# For each (M_in, H_audio) grid point:
#   - Run substeps 1..65 with JA physics
#   - Store (M_end, sumM_rest)

# Output as FAUST waveform literals
```

### Step 2: FAUST file with embedded LUT
```faust
// Generated LUT for K60 mode
lut_M_end = waveform{...8385 values...};
lut_sumM_rest = waveform{...8385 values...};

// Lookup with bilinear interpolation
lookup_remainder(M1, HAudio) = M_end, sumM_rest
with {
    M_end = bilinear(lut_M_end, ...);
    sumM_rest = bilinear(lut_sumM_rest, ...);
};
```

### Step 3: Modified main loop
```faust
ja_hysteresis_lut(H_in) = Mavg
with {
    // Substep 0: full physics (cross-sample dependency)
    M1, H1 = ja_substep_full(MPrev, HPrev, H_in, bias_lut(0));

    // Substeps 1-65: LUT lookup
    M_end, sumM_rest = lookup_remainder(M1, H_in);

    // Average
    Mavg = (M1 + sumM_rest) / 66.0;

    // Update state for next sample
    MPrev = M_end;
    HPrev = H_in + biasAmp * bias_lut(65);  // last bias value
};
```

## LUT Size Estimates

| Grid | Entries | Values | Size (doubles) |
|------|---------|--------|----------------|
| 65×129 | 8,385 | 16,770 | 134 KB |
| 129×257 | 33,153 | 66,306 | 530 KB |
| 257×513 | 131,841 | 263,682 | 2.1 MB |

Smaller grids with bilinear interpolation should be sufficient.

## Expected CPU Reduction

| Before | After | Reduction |
|--------|-------|-----------|
| 66 substeps | 1 substep + 4 lookups + interpolation | ~90-95% |
| 198 divisions | 3 divisions + ~20 multiplies | ~85% |
| ~24% CPU | ~3-5% CPU (estimated) | 5-8x faster |

## Implementation Order

1. **First:** Test the concept in C++ (easier to debug)
2. **Then:** Port to FAUST once proven
3. **Finally:** Generate LUTs for all 3 K modes (K32, K48, K60)

## Questions to Resolve

1. What H_audio range to cover? (based on typical input levels)
2. Should we warp the H axis for more resolution near 0?
3. How to handle parameter changes (Drive, Bias Level)?
   - Option: Multiple LUTs + interpolation
   - Option: Regenerate LUT on parameter change (background thread)

## Next Steps

1. Write Python script to generate test LUT for K60
2. Implement in C++ JAHysteresisScheduler first
3. A/B test sound quality vs original
4. If good, port to FAUST
