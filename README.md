# FAUST_FSM_TAPE - Jiles-Atherton Magnetic Hysteresis

FAUST implementation of the Jiles-Atherton (JA) model of ferromagnetic hysteresis — a physically-based mathematical description relating magnetization (M) to applied field (H). Combined with a phase-locked bias oscillator for analog tape emulation.

**Author**: Thomas Mandolini / OmegaDSP
**Contact**: thomas.mand0369@gmail.com

> **Project Status**: See [`docs/CURRENT_STATUS.md`](docs/CURRENT_STATUS.md) for current state, open problems, and research directions.

## ba.tabulateNd Breakthrough (2025-12-25)

**The ultimate optimization**: 3D LUT via FAUST's built-in `ba.tabulateNd` with runtime bias interpolation.

| Metric | Value |
|--------|-------|
| CPU | **0.19%** (M4 Max) |
| Grid | 33×65×9 (M, H, Bias) |
| Substeps | 4×K45 = 180 total |
| Interpolation | Tricubic |
| Sound | Better than full physics |

```bash
# Build the recommended version
cd faust/dev/lib_latest_proto && ./ja_tabulateNd.sh
```

Key features:
- **Runtime bias**: Continuous 0.1-0.9 (not discrete presets)
- **Lambda tilt**: Spectral shaping for tape character
- **Compile-time physics**: Full JA computed during FAUST compilation
- **Runtime = interpolation**: Just 64-point tricubic lookup at audio rate

## What We're Looking For (GRAME)

1. **Variable-count iteration pattern** - The C++ reference uses fractional substep accumulation (step count varies 35-37 per sample for better phase continuity). FAUST's fixed unrolled chains (exactly 36/54/66) cause subtle frequency response differences. Is there an idiomatic FAUST pattern for variable iteration counts based on runtime accumulator state?

   *Possible workaround:* Unroll to max count and gate each stage with `ba.if(step_index < steps_this_sample, newState, prevState)` to skip inactive steps. CPU stays fixed but physics only advances for active steps. Is there a cleaner pattern?

2. **Potential `ja.lib` library** - Reusable Jiles-Atherton hysteresis module with configurable physics parameters

## The Algorithm

### Jiles-Atherton Model

Core equation for magnetization change:

```
dM/dH = (c * dMan/dH + (Man - M) / pin) / (1 - c * α * dMan/dH)
```

Physics parameters:
- **Ms** (320): Saturation magnetization
- **a** (720): Anhysteretic curve shape
- **k** (280): Coercivity (loop width)
- **c** (0.18): Reversibility ratio
- **α** (0.015): Mean field coupling

### Phase-Locked Bias Oscillator

Fixed bias cycles per audio sample (sample-rate invariant).

**Note**: Integer cycles required to avoid 12kHz bias leakage (half-integer cycles tested, caused audible tone from residual phase accumulation).

| Mode | Cycles/Sample | Substeps | Character |
|------|---------------|----------|-----------|
| K28 | 1 | 28 | Maximum grit |
| K63 | 3 | 63 | Classic tape |
| K121 | 5 | 121 | Standard |
| K253 | 11 | 253 | Detailed |

Each substep uses midpoint sampling: `sin(phi + 0.5 * dphi)`

## Building

### Prerequisites

```bash
git clone --depth 1 https://github.com/juce-framework/JUCE.git
```

### FAUST Plugin

```bash
cd faust
./rebuild_faust.sh
```

First build creates the project with `faust2juce`. Subsequent builds preserve plugin IDs.

Flags: `-double` (64-bit precision)

### C++ Plugin

Open `juce_plugin/JA_Hysteresis_CPP.jucer` in Projucer, save, build from Xcode.

Or CMake:
```bash
cd juce_plugin
cmake -S . -B build -G Xcode && cmake --build build --config Release
```

## A/B Comparison

Both plugins use identical physics, DC blocker (SVF TPT 10 Hz), and parameter ranges.

**CPU load (M4 Max, Reaper, AU/VST3):**
| Implementation | CPU |
|----------------|-----|
| **ba.tabulateNd 3D LUT** | **0.19%** |
| FAUST full-physics K96 (96 substeps) | ~2% |
| C++ scheduler | ~2% |
| FAUST + 2D LUT optimization | <1% |

**Key difference:** C++ uses fractional substep accumulation (variable 35-37 steps), FAUST uses fixed unrolled chains (exactly 96). This causes subtle high-frequency response differences when bias is active.

**Note:** The LUT optimization trades some flexibility (fixed bias parameters) for massive CPU reduction. Hybrid 50/50 (48 real + 48 LUT) was tested but NOT recommended — Catmull-Rom interpolation overhead makes it only 0.3-0.5% faster than full physics. See `docs/CURRENT_STATUS.md` for details.

### Recommended: ba.tabulateNd 3D LUT

**Location**: `faust/dev/lib_latest_proto/ja_tabulateNd.dsp`
- 3D LUT (M × H × Bias) with 4×K45 cascaded lookups
- Runtime bias 0.1-0.9 with tricubic interpolation
- **0.19% CPU** — sounds better than full physics
- Build: `cd faust/dev/lib_latest_proto && ./ja_tabulateNd.sh`

### Full-Physics Alternatives

**Single mode**: `faust/dev/lib_latest_proto/jahysteresislib_proto.dsp`
- K96 (96 substeps, 4 cycles × 24) — ~2% CPU

**Multi-mode with ondemand**: `faust/dev/lib_latest_proto/jahysteresislib_proto_OD_3_modes.dsp`
- K96/K48/K24 quality modes — only active mode computes
- Build: `cd faust/dev/lib_latest_proto && ./build_OD_3_modes.sh`

Common features:
- Bias Asymmetry: adds 2nd harmonic for warmth (`sin(phase) + asym * sin(2*phase)`)
- **Wavelength Saturation (λ Tilt)**: frequency-dependent pre-saturation via `fi.spectral_tilt(3, 200, 15000, alpha)`. Simulates shorter wavelengths (higher frequencies) hitting tape harder — instant retro vibes. Range: -0.1 to +0.1, step 0.001.
- Stabilization: diff_scale soft clamp, sigma=1e-3
- Gain compensation: +15.6 dB makeup + piecewise bias compensation
- UI: Grouped controls (Quality, Gain, Bias, Stab, Tape [λ Tilt], Physics)

### Lite Version (3×K29 Cascaded LUT)

Ultra-low CPU version with discrete bias presets.

**Location**: `faust/test/test_fixed_bias_cascaded_LUT/`

- **Architecture**: 3 sequential K29 LUT lookups (87 total substeps, 3 course-corrections/sample)
- **9 bias presets**: 0.1 to 0.9 with measured gain compensation
- **CPU**: <1%

**Build** (requires extended timeout):
```bash
/opt/homebrew/bin/faust -t 600 -a /opt/homebrew/share/faust/juce/juce-plugin.cpp \
  -scn base_dsp -uim -i jahysteresis_lite_3xK29_9bias.dsp \
  -o jahysteresis_lite_3xK29_9bias/FaustPluginProcessor.cpp
```

**Important**: Default faust timeout is 120s. Use `-t 600` for complex DSP.

**Critical**: LUTs must use H range [-40, 40] (not [-1, 1]) to cover full +29dB drive range.

### Production Libraries

Located in `faust/dev/lib_final/`:

| Library | Description | CPU |
|---------|-------------|-----|
| `jahysteresis.lib` | Full-physics K72, runtime params | ~2% |
| `jahysteresis_lite.lib` | LUT-optimized, 10 modes (K28-K2101) | <1% |

## License

Shared for collaboration with GRAME. Contact author for commercial licensing.
