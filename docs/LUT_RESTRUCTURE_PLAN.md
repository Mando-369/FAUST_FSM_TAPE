# LUT Restructure Plan

## Problem

Current implementation has two inefficiencies:

1. **10 separate library files** - Each mode (K28-K1920) has its own `.lib` file
2. **Parallel computation** - `ba.if` computes ALL 10 loops every sample, selecting output afterward

Result: ~10x unnecessary CPU overhead.

## Current Structure

```
faust/
├── ja_lut_k28.lib      # 8385×2 values
├── ja_lut_k32.lib      # 8385×2 values
├── ...
├── ja_lut_k1920.lib    # 8385×2 values
└── dev/
    └── ja_streaming_bias_proto.dsp
        ├── imports 10 libs
        ├── 10 bias sin LUTs (tablesize_27..tablesize_2112)
        ├── 10 ja_loop_kXX functions
        └── nested ba.if selecting loop output
```

## Proposed Structure

```
faust/
├── ja_lut_unified.lib   # Single file with all modes
├── jahysteresis.lib     # Contribution-ready library
└── dev/
    └── ja_streaming_bias_proto.dsp
        ├── imports 1 lib
        ├── unified bias LUT with mode offset
        ├── single ja_loop function with mode parameter
        └── single feedback loop
```

---

## Phase 1: Unified LUT Library

### 1.1 Modify `generate_ja_lut.py`

Add `--generate-all` flag:

```python
def generate_unified_faust_lib(output_path: Path):
    """Generate single library with all modes"""

    # Mode order (index 0-9)
    MODE_ORDER = ['K28', 'K32', 'K60', 'K90', 'K120',
                  'K180', 'K240', 'K480', 'K960', 'K1920']

    # Generate all LUTs
    all_m_end = []
    all_sum_m_rest = []

    for mode_name in MODE_ORDER:
        m_grid, h_grid, lut_M_end, lut_sumM_rest = generate_2d_lut(
            mode=MODES[mode_name], ...
        )
        all_m_end.extend(lut_M_end.flatten())
        all_sum_m_rest.extend(lut_sumM_rest.flatten())

    # Write single waveform with 83850 values (10 × 8385)
```

### 1.2 Unified Library Format

```faust
// ja_lut_unified.lib
import("stdfaust.lib");

// Grid constants (same for all modes)
ja_lut_m_size = 65;
ja_lut_h_size = 129;
ja_lut_table_size = 8385;  // 65 × 129
ja_lut_m_min = -1.0;
ja_lut_m_max = 1.0;
ja_lut_h_min = -1.0;
ja_lut_h_max = 1.0;

// Mode metadata: (substeps, inv_substeps, bias_cycles)
// Index: K28=0, K32=1, K60=2, K90=3, K120=4, K180=5, K240=6, K480=7, K960=8, K1920=9
ja_mode_substeps = waveform{27, 36, 66, 99, 132, 198, 264, 528, 1056, 2112};
ja_mode_inv_substeps = waveform{0.037037, 0.027778, 0.015152, 0.010101,
                                 0.007576, 0.005051, 0.003788, 0.001894,
                                 0.000947, 0.000473};
ja_mode_bias_cycles = waveform{1.5, 2.0, 3.0, 4.5, 6.0, 9.0, 12.0, 24.0, 48.0, 96.0};

// Combined M_end LUT (83850 values: 10 modes × 8385)
ja_lut_m_end = waveform{
    // Mode 0 (K28): indices 0..8384
    ...
    // Mode 1 (K32): indices 8385..16769
    ...
    // Mode 9 (K1920): indices 75465..83849
    ...
};

// Combined sumM_rest LUT (83850 values)
ja_lut_sum_m_rest = waveform{ ... };

// Mode offset calculation
ja_mode_offset(mode) = int(mode) * ja_lut_table_size;

// 2D index within mode
ja_lut_idx(m_idx, h_idx) = m_idx * ja_lut_h_size + h_idx;

// Bilinear lookup with mode selection
ja_lookup_m_end(mode, m, h) = result
with {
    m_n = max(0.0, min(1.0, (m - ja_lut_m_min) / (ja_lut_m_max - ja_lut_m_min)));
    h_n = max(0.0, min(1.0, (h - ja_lut_h_min) / (ja_lut_h_max - ja_lut_h_min)));

    m_scaled = m_n * (ja_lut_m_size - 1);
    h_scaled = h_n * (ja_lut_h_size - 1);

    m_idx = int(floor(m_scaled));
    h_idx = int(floor(h_scaled));
    m_frac = m_scaled - float(m_idx);
    h_frac = h_scaled - float(h_idx);

    m_idx_safe = min(m_idx, ja_lut_m_size - 2);
    h_idx_safe = min(h_idx, ja_lut_h_size - 2);

    base = ja_mode_offset(mode);

    v00 = ja_lut_m_end, base + ja_lut_idx(m_idx_safe, h_idx_safe) : rdtable;
    v01 = ja_lut_m_end, base + ja_lut_idx(m_idx_safe, h_idx_safe + 1) : rdtable;
    v10 = ja_lut_m_end, base + ja_lut_idx(m_idx_safe + 1, h_idx_safe) : rdtable;
    v11 = ja_lut_m_end, base + ja_lut_idx(m_idx_safe + 1, h_idx_safe + 1) : rdtable;

    result = v00 * (1.0 - m_frac) * (1.0 - h_frac) +
             v01 * (1.0 - m_frac) * h_frac +
             v10 * m_frac * (1.0 - h_frac) +
             v11 * m_frac * h_frac;
};

ja_lookup_sum_m_rest(mode, m, h) = result
with {
    // Same logic, different table
    ...
};
```

---

## Phase 2: Unified Bias Oscillator

### 2.1 Problem

Current: 10 separate bias LUTs with different sizes (27, 36, 66, ..., 2112)

Cannot easily unify because:
- Different table sizes
- `rdtable` requires compile-time size

### 2.2 Solution: Real-time sin() with mode parameters

Replace precomputed bias tables with direct computation:

```faust
// Bias phase increment per substep
// dphi = 2π × cycles / substeps
ja_bias_dphi(mode) = 2.0 * ma.PI * cycles / substeps
with {
    cycles = ja_mode_bias_cycles, int(mode) : rdtable;
    substeps = ja_mode_substeps, int(mode) : rdtable;
};

// Bias value at substep i
ja_bias_val(mode, i) = sin((float(i) + 0.5) * ja_bias_dphi(mode));
```

Cost: One `sin()` call for substep 0 only (LUT handles rest).

---

## Phase 3: Unified Loop Function

### 3.1 Current (10 parallel loops)

```faust
ja_hysteresis(H_in) =
  ba.if(bias_mode < 0.5, loopK28(H_in),
  ba.if(bias_mode < 1.5, loopK32(H_in),
  ...));  // ALL 10 computed
```

### 3.2 Proposed (single loop)

```faust
ja_loop(mode, M_prev, H_prev, H_audio) = M_end, H_end, Mavg
with {
    // Substep 0 with real-time bias
    bias0 = ja_bias_val(mode, 0);
    M1_H1 = ja_substep0(bias0, M_prev, H_prev, H_audio);
    M1 = ba.selector(0, 2, M1_H1);

    // LUT lookup for remainder (mode-indexed)
    M_end = ja_lookup_m_end(mode, M1, H_audio);
    sumM_rest = ja_lookup_sum_m_rest(mode, M1, H_audio);

    // Average magnetization
    substeps = ja_mode_substeps, int(mode) : rdtable;
    inv_substeps = ja_mode_inv_substeps, int(mode) : rdtable;
    Mavg = (M1 + sumM_rest) * inv_substeps;

    // Final H (last substep bias)
    bias_last = ja_bias_val(mode, substeps - 1);
    H_end = H_audio + bias_amp * bias_last;
};

ja_hysteresis(H_in) = (loop ~ (mem, mem)) : ba.selector(2, 3)
with {
    loop(recM, recH) = recM, recH, H_in : ja_loop(bias_mode);
};
```

---

## Phase 4: File Cleanup

After successful migration:

1. Delete individual lib files:
   - `ja_lut_k28.lib` through `ja_lut_k1920.lib`

2. Update `CLAUDE.md`:
   - Document new unified structure
   - Update LUT generation command

3. Update `rebuild_faust.sh` if needed

---

## Implementation Order

| Step | Task | Risk |
|------|------|------|
| 1 | Modify `generate_ja_lut.py` to add `--generate-all` | Low |
| 2 | Generate `ja_lut_unified.lib` | Low |
| 3 | Create `dev/ja_streaming_bias_proto_v2.dsp` (new file) | Low |
| 4 | Test v2 against original for correctness | Medium |
| 5 | Benchmark CPU usage | Low |
| 6 | Replace original if tests pass | Medium |
| 7 | Update `jahysteresis.lib` with unified LUT | Medium |
| 8 | Clean up old lib files | Low |

---

## Expected Results

| Metric | Current | After |
|--------|---------|-------|
| Library files | 10 | 1 |
| Loops computed | 10 (parallel) | 1 |
| LUT lookups/sample | 20 (10×2 tables) | 2 |
| CPU overhead | ~10x | ~1x |
| Memory | ~1.3 MB | ~1.3 MB (same) |

---

## Risks

1. **FAUST rdtable index limits** - Verify 83850 index works (should be fine)
2. **Bilinear interpolation edge cases** - Test at M/H boundaries
3. **Mode switching glitches** - May need smoothing on mode change
4. **sin() cost** - Minimal (2 calls per sample vs 0, but saves 10 loop computations)

---

## Rollback Plan

Keep original files until v2 is validated:
- `dev/ja_streaming_bias_proto.dsp` (original)
- `dev/ja_streaming_bias_proto_v2.dsp` (new unified)
- All `ja_lut_kXX.lib` files

Only delete after production validation.
