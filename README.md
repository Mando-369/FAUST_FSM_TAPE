# FAUST_FSM_TAPE - Jiles-Atherton Magnetic Hysteresis

FAUST implementation of the Jiles-Atherton (JA) model of ferromagnetic hysteresis — a physically-based mathematical description relating magnetization (M) to applied field (H). Combined with a phase-locked bias oscillator for analog tape emulation.

**Author**: Thomas Mandolini / OmegaDSP
**Contact**: thomas.mand0369@gmail.com

> **Project Status**: See [`docs/CURRENT_STATUS.md`](docs/CURRENT_STATUS.md) for current state, open problems, and research directions.

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
| FAUST full-physics K96 (96 substeps) | ~2% |
| C++ scheduler | ~2% |
| FAUST + LUT optimization | <1% |
| FAUST hybrid 50/50 | ~1.5-1.7% |

**Key difference:** C++ uses fractional substep accumulation (variable 35-37 steps), FAUST uses fixed unrolled chains (exactly 96). This causes subtle high-frequency response differences when bias is active.

**Note:** The LUT optimization trades some flexibility (fixed bias parameters) for massive CPU reduction. Hybrid 50/50 (48 real + 48 LUT) was tested but NOT recommended — Catmull-Rom interpolation overhead makes it only 0.3-0.5% faster than full physics. See `docs/CURRENT_STATUS.md` for details.

### Full-Physics Prototypes (Recommended)

**Single mode**: `faust/dev/lib_latest_proto/jahysteresislib_proto.dsp`
- K96 (96 substeps, 4 cycles × 24) — sounds perfect

**Multi-mode with ondemand**: `faust/dev/lib_latest_proto/jahysteresislib_proto_OD_3_modes.dsp`
- K96/K48/K24 quality modes — only active mode computes
- Build: `cd faust/dev/lib_latest_proto && ./build_OD_3_modes.sh`

Common features:
- Bias Asymmetry: adds 2nd harmonic for warmth (`sin(phase) + asym * sin(2*phase)`)
- **Wavelength Saturation (λ Tilt)**: frequency-dependent pre-saturation via `fi.spectral_tilt(3, 200, 15000, alpha)`. Simulates shorter wavelengths (higher frequencies) hitting tape harder — instant retro vibes. Range: -0.5 to +0.5 (-3 to +3 dB/octave), step 0.001.
- Stabilization: diff_scale soft clamp, sigma=1e-3
- Gain compensation: +15.6 dB makeup + piecewise bias compensation
- UI: Grouped controls (Quality, Gain, Bias, Stab, Tape [λ Tilt], Physics)

## License

Shared for collaboration with GRAME. Contact author for commercial licensing.
