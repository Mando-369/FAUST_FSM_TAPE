# CLAUDE.md

## Project Overview

GRAME collaboration repository: Jiles-Atherton magnetic hysteresis for tape saturation.
Goal: optimize algorithm, create `jahysteresis.lib` library for GRAME contribution.

**Parent project**: FSM_TAPE (full plugin)
**This repo**: Extracted JA hysteresis only

**Current status, open problems, and research directions**: See [`docs/CURRENT_STATUS.md`](docs/CURRENT_STATUS.md)

## LUT Optimization (Major Breakthrough!)

The FAUST implementation now uses a **2D LUT optimization** that reduces CPU from ~24% to <1%:

- **Key insight**: Only substep 0 has cross-sample dependency; substeps 1..N-1 are deterministic given (M1, H_audio)
- **Solution**: 1 real JA substep + 2D LUT lookup for the remainder
- **Result**: Can run K2101 (2101 substeps) at same cost as original K63 (63 substeps)

### Available Modes (10-step control)

All modes use **half-integer cycles + odd substeps** for rich harmonic content.
This ensures opposite bias polarity between adjacent samples, introducing even harmonics.

| # | Mode | Cycles | Substeps | Character |
|---|------|--------|----------|-----------|
| 0 | K28 Ultra LoFi | 1.5 | 27 | Maximum grit |
| 1 | K45 LoFi | 2.5 | 45 | Crunchy |
| 2 | K63 Vintage | 3.5 | 63 | Classic tape |
| 3 | K99 Warm | 4.5 | 99 | Smooth warmth |
| 4 | K121 Standard | 5.5 | 121 | **Default** |
| 5 | K187 HQ | 8.5 | 187 | High quality |
| 6 | K253 Detailed | 11.5 | 253 | Very detailed |
| 7 | K495 Ultra | 22.5 | 495 | Ultra detailed |
| 8 | K1045 Extreme | 47.5 | 1045 | Extreme |
| 9 | K2101 Beyond | 95.5 | 2101 | Beyond physical |

### LUT Generation

```bash
cd scripts
python3 generate_ja_lut.py --mode K121 --bias-level 0.41 --output-dir ../faust
```

LUTs are precomputed for `bias_level=0.41, bias_scale=11.0`.

### LUT Responsiveness Challenge

The LUT optimization is CPU-efficient but creates static behavior (same input = same output).
The C++ full-physics version (~11% CPU) feels more "alive" due to variable substep counts and continuous phase.

**Goal**: Highest quality at affordable CPU (anything below C++ 11% is acceptable).

**Options** (see [`docs/JA_LUT_NONLINEARITY.md`](docs/JA_LUT_NONLINEARITY.md)):
1. Multiple real substeps (2-3 instead of 1)
2. Dynamic α(M) — magnetization-dependent coupling
3. Slew-dependent k(dH) — rate-responsive pinning
4. Dynamic c(M) — level-dependent reversibility

## Implementations

| Aspect | FAUST (LUT-optimized) | C++ (original) | C++ (LUT-optimized) |
|--------|----------------------|----------------|---------------------|
| Location | `faust/jahysteresis.lib` | `juce_plugin/Source/JAHysteresisScheduler.*` | `cpp_reference/JAHysteresisSchedulerLUT.*` |
| Prototype | `faust/dev/ja_streaming_bias_proto.dsp` | - | - |
| Substeps | 1 real + LUT lookup | Full loop (66 for K60) | 1 real + LUT lookup |
| tanh | Real `ma.tanh` | `fast_tanh` rational approx | `fast_tanh` rational approx |
| CPU | <1% | ~11% | <1% (expected) |

All implementations use identical physics: Ms=320, a=720, k=280, c=0.18, α=0.015

### C++ LUT Integration

The `cpp_reference/` folder contains a ready-to-integrate LUT-optimized C++ scheduler:

```cpp
#include "JAHysteresisSchedulerLUT.h"
#include "JAHysteresisLUT_K121.h"  // from faust/

scheduler.initialise(sampleRate, Mode::K121, physics);
scheduler.setLUT(JAHysteresisLUT_K121::LUT_M_END.data(),
                 JAHysteresisLUT_K121::LUT_SUM_M_REST.data(),
                 JAHysteresisLUT_K121::M_SIZE,
                 JAHysteresisLUT_K121::H_SIZE);
double output = scheduler.process(input);
```

See `cpp_reference/JAHysteresisSchedulerLUT_README.md` for full integration guide.

**Library prefix**: `jah` (e.g., `jah.tape_channel_ui`)

## Quick Commands

```bash
# First-time: clone JUCE to repo root
git clone --depth 1 https://github.com/juce-framework/JUCE.git

# Rebuild FAUST (preserves plugin IDs)
cd faust && ./rebuild_faust.sh

# Generate new LUT
cd scripts && python3 generate_ja_lut.py --mode K121 --bias-level 0.41 --output-dir ../faust

# Build C++ (Projucer)
# Open juce_plugin/JA_Hysteresis_CPP.jucer, save, build from Xcode
```

## Ondemand Primitive (Experimental)

The `ondemand` primitive eliminates parallel computation overhead by only computing the active mode branch.

**Prototype**: `faust/test/ja_streaming_bias_proto_od.dsp`

**Note**: The dev fork is **not in git** (too large). Install manually:
```bash
git clone -b master-dev-ocpp-od-fir-2-FIR13 https://github.com/grame-cncm/faust.git tools/faust-ondemand
cd tools/faust-ondemand/build && make
```

### Build with Ondemand

```bash
# Compile DSP to C++
./tools/faust-ondemand/build/bin/faust faust/test/ja_streaming_bias_proto_od.dsp -o faust/test/TestOD.cpp

# Build AU plugin (from project root)
bash -c 'export PATH="$(pwd)/tools/faust-ondemand/build/bin:$PATH" && \
export FAUSTARCH="$(pwd)/tools/faust-ondemand/architecture" && \
export FAUSTLIB="$(pwd)/tools/faust-ondemand/share/faust" && \
export FAUSTINC="$(pwd)/tools/faust-ondemand/architecture" && \
$(pwd)/tools/faust-ondemand/tools/faust2appls/faust2juce \
  -jucemodulesdir $(pwd)/JUCE/modules \
  faust/test/ja_streaming_bias_proto_od.dsp'

# Generate Xcode project and build
JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer \
  --resave faust/test/ja_streaming_bias_proto_od/ja_streaming_bias_proto_od.jucer

xcodebuild -project faust/test/ja_streaming_bias_proto_od/Builds/MacOSX/ja_streaming_bias_proto_od.xcodeproj \
  -scheme "ja_streaming_bias_proto_od - AU" -configuration Release build
```

Plugin installs to `~/Library/Audio/Plug-Ins/Components/`.

## File Structure

```
FAUST_FSM_TAPE/
├── JUCE/                           # Shared (gitignored)
├── faust/
│   ├── jahysteresis.lib            # Contribution-ready FAUST library
│   ├── ja_lut_k*.lib               # Precomputed 2D LUTs (K28-K2101)
│   ├── JAHysteresisLUT_K*.h        # C++ LUT headers (all 10 modes)
│   ├── rebuild_faust.sh            # Rebuild without changing plugin IDs
│   ├── dev/
│   │   ├── ja_streaming_bias_proto.dsp       # Working prototype (ba.if version)
│   │   ├── ja_streaming_bias_proto_OD_72.dsp # 72-substep ondemand prototype
│   │   └── test_gated_substeps.dsp           # Gated substeps test
│   ├── test/
│   │   ├── test_gated_substeps.dsp           # Ondemand gating tests
│   │   └── ja_lut_k*.lib                     # LUTs for test builds
│   └── examples/
│       └── jah_tape_demo.dsp       # Demo importing jahysteresis.lib
├── cpp_reference/
│   ├── JAHysteresisScheduler.*     # Original C++ scheduler (~11% CPU)
│   ├── JAHysteresisSchedulerLUT.*  # LUT-optimized C++ scheduler (<1% CPU)
│   └── JAHysteresisSchedulerLUT_README.md  # Integration guide
├── juce_plugin/
│   ├── JA_Hysteresis_CPP.jucer
│   ├── CMakeLists.txt
│   └── Source/
├── scripts/
│   └── generate_ja_lut.py          # LUT generator (outputs .lib and .h)
├── tools/                          # Gitignored - clone separately
│   └── faust-ondemand/             # Dev fork with ondemand primitive
└── docs/
    ├── CURRENT_STATUS.md           # Project status and open problems
    ├── JA_LUT_NONLINEARITY.md      # LUT enhancement options for responsiveness
    ├── GRAME_ONDEMAND_BUG_REPORT.md # Ondemand primitive bug report
    └── LUT_RESTRUCTURE_PLAN.md     # Unified LUT optimization plan
```

## Plugin IDs (Don't Change!)

FAUST plugin: `pluginCode="2ec6"`, `bundleIdentifier="com.grame.ja_streaming_bias_proto"`

Use `rebuild_faust.sh` to preserve IDs when regenerating.
