# JA Hysteresis Library Restructure Plan

**Date**: 2024-12-06
**Status**: Planning

---

## Overview

Two FAUST libraries:

| Library | Description | CPU Target |
|---------|-------------|------------|
| `jahysteresis.lib` | Full physics, highest quality | ~2% |
| `jahysteresis_lite.lib` | LUT-based, CPU efficient | ~1% (50% reduction) |

---

## Mode Structure (6 Modes)

| Mode | Quality | Cycles | Steps/Cycle | Total | Character |
|------|---------|--------|-------------|-------|-----------|
| K32 | Normal | 2.5 | 18 | 45 | Lo-fi, warm harmonics |
| K32 | High | 2.0 | 20 | 40 | Lo-fi, cleaner |
| K48 | Normal | 3.5 | 18 | 63 | Vintage, rich |
| K48 | High | 3.0 | 19 | 57 | Vintage, transparent |
| K60 | Normal | 3.5 | 22 | 77 | HQ, warm |
| K60 | High | 3.0 | 24 | 72 | HQ, reference |

**Normal modes**: Half-integer cycles → odd substeps → even harmonics → warmer tone
**High modes**: Integer cycles → maximum resolution → cleaner/transparent

---

## Library 1: `jahysteresis.lib` (Full Physics)

### Features

- Full JA physics for all substeps
- Fractional cursor accumulation (C++ scheduler behavior)
- Real `tanh`
- 6 modes with `ondemand` selection

### Implementation

```faust
// Pre-compiled chains for each mode
chain_k32_normal = seq(i, 45, ja_substep(...));  // 2.5 × 18
chain_k32_high   = seq(i, 40, ja_substep(...));  // 2.0 × 20
chain_k48_normal = seq(i, 63, ja_substep(...));  // 3.5 × 18
chain_k48_high   = seq(i, 57, ja_substep(...));  // 3.0 × 19
chain_k60_normal = seq(i, 77, ja_substep(...));  // 3.5 × 22
chain_k60_high   = seq(i, 72, ja_substep(...));  // 3.0 × 24
```

---

## Library 2: `jahysteresis_lite.lib` (LUT-Based)

### Features

- ~15% real substeps + LUT for remainder
- RK4-generated LUTs
- 3-way LUT blending (N-1, N, N+1)
- Catmull-Rom interpolation

### LUT Configuration

| Mode | Total | Real (~15%) | LUT |
|------|-------|-------------|-----|
| K32 Normal | 45 | 7 | 38 |
| K32 High | 40 | 6 | 34 |
| K48 Normal | 63 | 9 | 54 |
| K48 High | 57 | 9 | 48 |
| K60 Normal | 77 | 12 | 65 |
| K60 High | 72 | 11 | 61 |

---

## API

```faust
import("jahysteresis.lib");

// Main function
jah.process(mode, quality) : _ -> _;

// Convenience
jah.k32_normal : _ -> _;
jah.k32_high : _ -> _;
jah.k48_normal : _ -> _;
jah.k48_high : _ -> _;
jah.k60_normal : _ -> _;
jah.k60_high : _ -> _;
```

Same API for `jahysteresis_lite.lib` with `jah_lite` prefix.

---

## File Structure

```
faust/
├── jahysteresis.lib
├── jahysteresis_lite.lib
├── ja_lut/
│   ├── ja_lut_k32_45.lib   # + variants 44, 46
│   ├── ja_lut_k32_40.lib   # + variants 39, 41
│   ├── ja_lut_k48_63.lib   # + variants 62, 64
│   ├── ja_lut_k48_57.lib   # + variants 56, 58
│   ├── ja_lut_k60_77.lib   # + variants 76, 78
│   └── ja_lut_k60_72.lib   # + variants 71, 73
└── examples/
    ├── tape_hq.dsp
    └── tape_lite.dsp
```

---

## Implementation Steps

### Phase 1: jahysteresis.lib
1. Create library structure
2. Implement 6 substep chains
3. Add mode selection
4. Implement fractional cursor
5. Test & benchmark

### Phase 2: jahysteresis_lite.lib
1. Update Python generator
2. Generate 6 mode LUTs with variants
3. Implement real substeps + LUT
4. Add 3-way blending
5. Test & benchmark

### Phase 3: Polish
1. Example DSPs
2. Documentation
3. A/B testing
