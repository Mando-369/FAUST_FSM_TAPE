# CLAUDE.md

## Project Overview

GRAME collaboration repository: Jiles-Atherton magnetic hysteresis for tape saturation.
Goal: optimize algorithm, potentially create `ja.lib` library.

**Parent project**: FSM_TAPE (full plugin)
**This repo**: Extracted JA hysteresis only

## LUT Optimization (Major Breakthrough!)

The FAUST implementation now uses a **2D LUT optimization** that reduces CPU from ~24% to <1%:

- **Key insight**: Only substep 0 has cross-sample dependency; substeps 1..N-1 are deterministic given (M1, H_audio)
- **Solution**: 1 real JA substep + 2D LUT lookup for the remainder
- **Result**: Can run K1920 (2112 substeps) at same cost as original K60 (66 substeps)

### Available Modes

| Mode | Cycles | Substeps | Character |
|------|--------|----------|-----------|
| K32 LoFi | 2 | 36 | Gritty harmonics |
| K60 | 3 | 66 | Classic tape |
| K120 | 6 | 132 | Smooth |
| K240 | 12 | 264 | Hi-fi |
| K480 | 24 | 528 | Very clean |
| K960 | 48 | 1056 | Ultra clean |
| K1920 Ultra | 96 | 2112 | Pristine |

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
```

## Plugin IDs (Don't Change!)

FAUST plugin: `pluginCode="2ec6"`, `bundleIdentifier="com.grame.ja_streaming_bias_proto"`

Use `rebuild_faust.sh` to preserve IDs when regenerating.
