# CLAUDE.md

## Project Overview

GRAME collaboration repository: Jiles-Atherton magnetic hysteresis for tape saturation.
Goal: optimize algorithm, potentially create `ja.lib` library.

**Parent project**: FSM_TAPE (full plugin)
**This repo**: Extracted JA hysteresis only

**Current status, open problems, and research directions**: See [`docs/CURRENT_STATUS.md`](docs/CURRENT_STATUS.md)

## LUT Optimization (Major Breakthrough!)

The FAUST implementation now uses a **2D LUT optimization** that reduces CPU from ~24% to <1%:

- **Key insight**: Only substep 0 has cross-sample dependency; substeps 1..N-1 are deterministic given (M1, H_audio)
- **Solution**: 1 real JA substep + 2D LUT lookup for the remainder
- **Result**: Can run K1920 (2112 substeps) at same cost as original K60 (66 substeps)

### Available Modes (10-step control)

| # | Mode | Cycles | Substeps | Character |
|---|------|--------|----------|-----------|
| 0 | K28 Ultra LoFi | 1.5 | 27 | Maximum grit |
| 1 | K32 LoFi | 2 | 36 | Crunchy |
| 2 | K60 Vintage | 3 | 66 | Classic tape |
| 3 | K90 Warm | 4.5 | 99 | Smooth warmth |
| 4 | K120 Standard | 6 | 132 | **Default** |
| 5 | K180 HQ | 9 | 198 | High quality |
| 6 | K240 | 12 | 264 | Very detailed |
| 7 | K480 | 24 | 528 | Ultra detailed |
| 8 | K960 | 48 | 1056 | Extreme |
| 9 | K1920 Beyond | 96 | 2112 | Beyond physical |

### LUT Generation

```bash
cd scripts
python3 generate_ja_lut.py --mode K960 --bias-level 0.41 --output-dir ../faust
```

LUTs are precomputed for `bias_level=0.41, bias_scale=11.0`.

## Implementations

| Aspect | FAUST (LUT-optimized) | C++ (original) |
|--------|----------------------|----------------|
| File | `faust/ja_streaming_bias_proto.dsp` | `juce_plugin/Source/JAHysteresisScheduler.*` |
| Substeps | 1 real + LUT lookup | Full loop (66 for K60) |
| tanh | Real `ma.tanh` | `fast_tanh` rational approx |
| CPU | <1% | ~11% |

Both use identical physics: Ms=320, a=720, k=280, c=0.18, α=0.015

## Quick Commands

```bash
# First-time: clone JUCE to repo root
git clone --depth 1 https://github.com/juce-framework/JUCE.git

# Rebuild FAUST (preserves plugin IDs)
cd faust && ./rebuild_faust.sh

# Generate new LUT
cd scripts && python3 generate_ja_lut.py --mode K960 --bias-level 0.41 --output-dir ../faust

# Build C++ (Projucer)
# Open juce_plugin/JA_Hysteresis_CPP.jucer, save, build from Xcode
```

## File Structure

```
FAUST_FSM_TAPE/
├── JUCE/                           # Shared (gitignored)
├── faust/
│   ├── ja_streaming_bias_proto.dsp # FAUST prototype (LUT-optimized)
│   ├── ja_lut_k*.lib               # Precomputed 2D LUTs
│   ├── rebuild_faust.sh            # Rebuild without changing plugin IDs
│   └── ja_streaming_bias_proto/    # Generated (gitignored)
├── juce_plugin/
│   ├── JA_Hysteresis_CPP.jucer
│   ├── CMakeLists.txt
│   └── Source/
├── scripts/
│   └── generate_ja_lut.py          # LUT generator
└── docs/
    ├── CURRENT_STATUS.md           # Project status and open problems
    └── LUT_RESTRUCTURE_PLAN.md     # Unified LUT optimization plan
```

## Plugin IDs (Don't Change!)

FAUST plugin: `pluginCode="2ec6"`, `bundleIdentifier="com.grame.ja_streaming_bias_proto"`

Use `rebuild_faust.sh` to preserve IDs when regenerating.
