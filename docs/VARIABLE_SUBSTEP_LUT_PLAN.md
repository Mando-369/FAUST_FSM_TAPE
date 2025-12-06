# Variable Substep Count LUT Implementation Plan

## Status: IMPLEMENTED (Phase 1 Complete)

**Implementation Date**: December 2024

---

## Goal

Replicate C++ scheduler's dynamic substep behavior in FAUST while maintaining low CPU usage.

**Problem**: Current LUT approach is static - same (M1, H_audio) always produces identical output.

**Solution**: Generate multiple LUT variants (N-1, N, N+1 substeps) and use fractional cursor accumulation to alternate between them, exactly matching C++ scheduler behavior.

---

## Implementation Summary

### What Was Built

1. **Python Generator Modifications** (`scripts/generate_ja_lut.py`):
   - Added `--variants` flag to generate N-1, N, N+1 LUT variants
   - All variants share same phase span (11π for K121), different substep counts
   - **Catmull-Rom interpolation** instead of bilinear (16 points vs 4)

2. **FAUST Test Prototype** (`faust/test/test_var_subst_lut.dsp`):
   - Imports K120, K121, K122 LUT variants
   - Fractional cursor accumulation (0.5 increment for K121)
   - **3-way smooth blending** across all 3 LUTs (not hard switching)
   - **Cosine crossfade** for smooth transitions

3. **Generated LUTs** (`faust/test/ja_lut_k{120,121,122}.lib`):
   - 65x129 grid resolution (8,385 points each)
   - Catmull-Rom bicubic interpolation
   - Same phase span (11π), different substep counts

### Key Improvements Over Original Plan

| Original Plan | Actual Implementation |
|---------------|----------------------|
| 2-LUT toggle (K121/K122) | 3-LUT interpolation (K120/K121/K122) |
| Hard switching | Cosine crossfade |
| Bilinear interpolation | Catmull-Rom (separable bicubic) |
| Linear blend | Smooth cosine blend |

### Results

- Null test shows clicks eliminated (cosine crossfade works)
- Sound quality significantly improved ("MUCH BETTER")
- Close to C++ reference in detail and "bite"
- CPU still very low (<1%)

---

## Scope: Phase 1 Only

**Initial target**: K121 mode only (default mode)
- Generate K120, K121, K122 variants
- Test and validate the approach
- Expand to other modes only after success

---

## Phase 1: Python LUT Generator Modifications

### File: `scripts/generate_ja_lut.py`

### 1.1 Add Variant Generation Flag

```python
parser.add_argument('--variants', action='store_true',
                    help='Generate N-1, N, N+1 variants for the mode')
```

### 1.2 Variant Generation Logic

For K121 (5.5 cycles × 22 steps/cycle = 121 substeps):
- **Same phase span** (5.5 cycles = 11π) for all variants
- **Different substep counts**: 120, 121, 122

```python
def generate_variant_luts(mode_name, base_substeps, phase_span, bias_level, bias_scale, output_dir):
    """Generate N-1, N, N+1 LUT variants"""

    for offset in [-1, 0, 1]:
        substeps = base_substeps + offset
        variant_name = f"K{substeps}"
        phase_step = phase_span / substeps

        # Generate LUT with this substep count
        # ... existing LUT generation logic with phase_step

        # Output: ja_lut_k{substeps}.lib
```

### 1.3 Key Insight

Current K121 calculation:
```python
# K121: 5.5 cycles, 22 steps/cycle
phase_step = (5.5 * 2 * pi) / 121  # = 11π / 121
```

Variants:
```python
# K120: same 5.5 cycles, but 120 steps
phase_step_120 = (5.5 * 2 * pi) / 120  # = 11π / 120 (slightly larger step)

# K122: same 5.5 cycles, but 122 steps
phase_step_122 = (5.5 * 2 * pi) / 122  # = 11π / 122 (slightly smaller step)
```

This means:
- K120: Slightly faster phase accumulation per substep, reaches full cycle earlier
- K122: Slightly slower phase accumulation per substep, extends beyond slightly

### 1.4 Output Files (K121 only for now)

```
faust/
├── ja_lut_k120.lib          # N-1 variant (NEW)
├── ja_lut_k121.lib          # Base variant (already exists)
├── ja_lut_k122.lib          # N+1 variant (NEW)
```

---

## Phase 2: FAUST Implementation

### File: `faust/dev/ja_streaming_bias_proto_mod_1.dsp`

### 2.1 Import All Three LUT Variants

```faust
// Import LUT variants for dynamic substep selection
lut_k120 = library("ja_lut_k120.lib");
lut_k121 = library("ja_lut_k121.lib");
lut_k122 = library("ja_lut_k122.lib");
```

### 2.2 Fractional Cursor Accumulation

Replicate C++ scheduler's cursor logic:

```faust
// Fractional cursor accumulation
// For K121: 5.5 cycles per sample = 0.5 fractional part
// cursor wraps 0.0 → 1.0, determines which LUT to use

cycles_per_sample = 5.5;
frac_part = cycles_per_sample - floor(cycles_per_sample);  // 0.5

// Accumulate fractional part each sample
cursor = (cursor' + frac_part) : fmod(_, 1.0);

// Map cursor to LUT selection: 0=K120, 1=K121, 2=K122
// Equal thirds approach
lut_select = int(cursor * 3) : min(2);
```

### 2.3 LUT Selection (Hard Switch)

```faust
// Select which LUT to use based on cursor
ja_process_variable(M_in, H_audio) = M_end, sumM_rest
with {
    // Get results from all three LUTs (or use select3 on tables)
    lut_select = ...; // from cursor logic

    // Hard switch between LUTs
    M_end = select3(lut_select, M_120, M_121, M_122);
    sumM_rest = select3(lut_select, sum_120, sum_121, sum_122);
};
```

### 2.4 Alternative: Just Alternate Between Two

Simpler approach - since K121 has 0.5 fractional:
- Even samples: use K121
- Odd samples: use K122 (or K120)

```faust
// Toggle each sample
toggle = 1 - toggle';

// Alternate between K121 and K122
M_end = select2(toggle, M_121, M_122);
sumM_rest = select2(toggle, sum_121, sum_122);
```

This is closer to C++ behavior where samples alternate between 121 and 122 substeps.

---

## Phase 3: Implementation Steps

### Step 1: Modify Python Generator

1. Read current `generate_ja_lut.py` structure
2. Add `--variants` flag
3. Modify to generate N-1, N, N+1 when flag is set
4. Keep same phase span, change substep count only

### Step 2: Generate K120, K121, K122

```bash
cd scripts
python3 generate_ja_lut.py --mode K121 --variants --bias-level 0.41 --output-dir ../faust
```

### Step 3: Implement FAUST Changes

1. Copy `ja_streaming_bias_proto.dsp` to `ja_streaming_bias_proto_mod_1.dsp`
2. Import all three LUT variants
3. Add cursor/toggle logic
4. Modify LUT lookup to use selection

### Step 4: Build and Test

```bash
cd faust/dev/ja_streaming_bias_proto_mod_1
# faust2juce + xcodebuild as before
```

### Step 5: Null Test

- Two tracks, same settings, one phase-inverted
- Should NOT cancel perfectly (variation expected!)

---

## Decision: Hard Switch vs Toggle

**Recommendation for K121**: Use simple toggle (alternating samples)

Since K121 = 5.5 cycles = 121 substeps:
- C++ does 121 substeps one sample, 122 the next (alternating)
- FAUST toggle matches this behavior exactly

```faust
// Simple alternating approach for 0.5 fractional
use_plus_one = 1 - use_plus_one';  // Toggles 0,1,0,1,...

M_end = select2(use_plus_one,
                lut_k121.lookup_M_end(M_in, H_audio),
                lut_k122.lookup_M_end(M_in, H_audio));
```

---

## Success Criteria

1. **Variation check**: Null test shows non-zero difference
2. **Sound quality**: Similar "alive" feel to C++ reference
3. **CPU**: Still below 11% (C++ reference point)
4. **No artifacts**: No clicks/pops from LUT switching

---

## Next Steps (After Phase 1 Success)

1. **Try K253 mode**: Generate K252, K253, K254 variants for higher quality
2. Expand to other modes (K63, K187, etc.) following same pattern
3. Consider integrating into main `jahysteresis.lib` once validated
