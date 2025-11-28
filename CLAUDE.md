# CLAUDE.md

## Project Overview

GRAME collaboration repository: Jiles-Atherton magnetic hysteresis for tape saturation.
Goal: optimize algorithm, potentially create `ja.lib` library.

**Parent project**: FSM_TAPE (full plugin)
**This repo**: Extracted JA hysteresis only

## Implementations (Must Match!)

| Aspect | FAUST | C++ |
|--------|-------|-----|
| File | `faust/ja_streaming_bias_proto.dsp` | `juce_plugin/Source/JAHysteresisScheduler.*` |
| Sin calls | Direct `sin()` per substep | Direct `std::sin()` per substep |
| Loops | Unrolled chains | For loops |

Both use identical:
- Physics: Ms=320, a=720, k=280, c=0.18, α=0.015
- Substeps: K32=36, K48=54, K60=66
- fast_tanh: `t * (27 + x²) / (27 + 9x²)`
- DC blocker: 10 Hz

## Quick Commands

```bash
# First-time: clone JUCE to repo root
git clone --depth 1 https://github.com/juce-framework/JUCE.git

# Rebuild FAUST (preserves plugin IDs)
cd faust && ./rebuild_faust.sh

# Build C++ (Projucer)
# Open juce_plugin/JA_Hysteresis_CPP.jucer, save, build from Xcode

# Test FAUST syntax only
faust -double -ftz 2 ja_streaming_bias_proto.dsp
```

## File Structure

```
FAUST_FSM_TAPE/
├── JUCE/                           # Shared (gitignored)
├── faust/
│   ├── ja_streaming_bias_proto.dsp # FAUST prototype
│   ├── rebuild_faust.sh            # Rebuild without changing plugin IDs
│   └── ja_streaming_bias_proto/    # Generated (gitignored)
├── juce_plugin/
│   ├── JA_Hysteresis_CPP.jucer
│   ├── CMakeLists.txt
│   └── Source/
└── docs/
```

## Plugin IDs (Don't Change!)

FAUST plugin: `pluginCode="2ec6"`, `bundleIdentifier="com.grame.ja_streaming_bias_proto"`

Use `rebuild_faust.sh` to preserve IDs when regenerating.
