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

**Note**: LUT modes use integer cycles to avoid bias leakage. Half-integer cycles were tested but cause audible 12kHz tone from residual phase accumulation.

| # | Mode | Cycles | Substeps | Character |
|---|------|--------|----------|-----------|
| 0 | K28 Ultra LoFi | 1 | 28 | Maximum grit |
| 1 | K45 LoFi | 2 | 45 | Crunchy |
| 2 | K63 Vintage | 3 | 63 | Classic tape |
| 3 | K99 Warm | 4 | 99 | Smooth warmth |
| 4 | K121 Standard | 5 | 121 | **Default** |
| 5 | K187 HQ | 8 | 187 | High quality |
| 6 | K253 Detailed | 11 | 253 | Very detailed |
| 7 | K495 Ultra | 22 | 495 | Ultra detailed |
| 8 | K1045 Extreme | 47 | 1045 | Extreme |
| 9 | K2101 Beyond | 95 | 2101 | Beyond physical |

### LUT Generation

```bash
cd scripts
python3 generate_ja_lut.py --mode K121 --bias-level 0.41 --output-dir ../faust
```

LUTs are precomputed for `bias_level=0.41, bias_scale=11.0`.

### Variable Substep LUT (Major Breakthrough!)

The original LUT was static - same input always produced identical output. The C++ version sounds more "alive" due to variable substep counts from fractional cursor accumulation.

**Solution implemented** (see [`docs/VARIABLE_SUBSTEP_LUT_PLAN.md`](docs/VARIABLE_SUBSTEP_LUT_PLAN.md)):

1. **Multiple LUT variants**: Generate K120, K121, K122 (N-1, N, N+1) with same phase span but different substep counts
2. **Fractional cursor**: Accumulates 0.5 each sample, smoothly blends across all 3 LUTs
3. **Cosine crossfade**: Smooth transitions between LUTs (not hard switching)
4. **Catmull-Rom interpolation**: 16-point bicubic instead of 4-point bilinear within each LUT

**Result**: Sound quality now very close to C++ reference! Much more detail and "bite".

```bash
# Generate variant LUTs
cd scripts && python3 generate_ja_lut.py --mode K121 --variants --output-dir ../faust/test
```

**Test prototype**: `faust/test/test_var_subst_lut.dsp`

## Implementations

| Aspect | FAUST (LUT-optimized) | FAUST (full-physics) | C++ (original) |
|--------|----------------------|----------------------|----------------|
| Location | `faust/jahysteresis.lib` | `faust/dev/lib_latest_proto/jahysteresislib_proto.dsp` | `cpp_reference/JAHysteresisScheduler.*` |
| Mode | 10 modes (K28-K2101) | K60 Ultra (72 substeps) | K32/K48/K60 × Eco/Normal/Ultra |
| Substeps | 1 real + LUT lookup | Full 72 via `seq` | Full loop |
| tanh | Real `ma.tanh` | Real `ma.tanh` | `fast_tanh` (±3 clamp) |
| CPU | <1% | TBD | ~11% |

All implementations use identical physics: Ms=320, a=720, k=280, c=0.18, α=0.015

### Full-Physics Prototype

`faust/dev/lib_latest_proto/jahysteresislib_proto.dsp` - K60 Ultra single mode:
- 72 substeps via `seq(i, 72, ja_substep_seq)` (3 cycles × 24)
- Integer cycles to avoid bias leakage (half-integer causes 12kHz tone)
- Phase continuity: M, H, phase fed back across samples
- **Stabilization**: diff_scale soft clamp on (Man_e - M_prev), sigma=1e-3
- **Gain compensation**: +15.6 dB makeup + piecewise bias compensation
- **UI**: Grouped controls (Gain, Bias, Stab, Physics)
- Drive range: -18 to +29 dB

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

**Bug report and test files**: `faust/GRAME_BUG_ONDEMAND/`

**Note**: The dev fork is **not in git** (too large). Install manually:
```bash
git clone -b master-dev-ocpp-od-fir-2-FIR13 https://github.com/grame-cncm/faust.git tools/faust-ondemand
cd tools/faust-ondemand/build && make
```

### Build with Ondemand

```bash
# Compile DSP to C++
./tools/faust-ondemand/build/bin/faust faust/GRAME_BUG_ONDEMAND/ja_streaming_bias_proto_od.dsp -o test.cpp

# Build AU plugin (from project root)
bash -c 'export PATH="$(pwd)/tools/faust-ondemand/build/bin:$PATH" && \
export FAUSTARCH="$(pwd)/tools/faust-ondemand/architecture" && \
export FAUSTLIB="$(pwd)/tools/faust-ondemand/share/faust" && \
export FAUSTINC="$(pwd)/tools/faust-ondemand/architecture" && \
$(pwd)/tools/faust-ondemand/tools/faust2appls/faust2juce \
  -jucemodulesdir $(pwd)/JUCE/modules \
  faust/GRAME_BUG_ONDEMAND/ja_streaming_bias_proto_od.dsp'
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
│   │   ├── lib_latest_proto/              # Latest full-physics prototype
│   │   │   └── jahysteresislib_proto.dsp  # K60 Ultra (C++ match)
│   │   └── dev_old/                       # Archived prototypes
│   ├── test/
│   │   ├── test_var_subst_lut.dsp      # Variable substep LUT test
│   │   ├── ja_lut_k*.lib               # LUTs for test builds
│   │   └── test_old/                   # Archived tests
│   ├── GRAME_BUG_ONDEMAND/             # Ondemand bug reproduction files
│   │   ├── GRAME_ONDEMAND_BUG_REPORT.md
│   │   ├── ja_streaming_bias_proto_od.dsp      # Working (LUT-based)
│   │   └── jaStreamingBiasProtoOD24.dsp        # Failing (complex seq)
│   └── examples/
│       └── jah_tape_demo.dsp       # Demo importing jahysteresis.lib
├── cpp_reference/
│   ├── JAHysteresisScheduler.*     # Original C++ scheduler (~2% CPU)
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
    ├── JA_LUT_NONLINEARITY.md      # LUT enhancement options
    └── VARIABLE_SUBSTEP_LUT_PLAN.md # Variable substep LUT design
```

## Plugin IDs (Don't Change!)

FAUST plugin: `pluginCode="2ec6"`, `bundleIdentifier="com.grame.ja_streaming_bias_proto"`

Use `rebuild_faust.sh` to preserve IDs when regenerating.
