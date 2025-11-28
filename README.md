# FAUST_FSM_TAPE - Jiles-Atherton Magnetic Hysteresis

FAUST implementation of the Jiles-Atherton (JA) magnetic hysteresis model with phase-locked bias oscillator for tape saturation simulation.

**Author**: Thomas Mandolini / OmegaDSP
**Contact**: thomas.mand0369@gmail.com

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

Fixed bias cycles per audio sample (sample-rate invariant):

| Mode | Cycles/Sample | Substeps | Points/Cycle |
|------|---------------|----------|--------------|
| K32 | 2 | 36 | 18 |
| K48 | 3 | 54 | 18 |
| K60 | 3 | 66 | 22 |

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

Flags: `-double` (64-bit precision), `-ftz 2` (flush denormals)

### C++ Plugin

Open `juce_plugin/JA_Hysteresis_CPP.jucer` in Projucer, save, build from Xcode.

Or CMake:
```bash
cd juce_plugin
cmake -S . -B build -G Xcode && cmake --build build --config Release
```

## A/B Comparison

Both plugins use identical physics, DC blocker (SVF TPT 10 Hz), and parameter ranges.

**CPU load at K60 (M4 Max):** FAUST ~24% vs C++ ~11%

**Key difference:** C++ uses fractional substep accumulation (variable 35-37 steps), FAUST uses fixed unrolled chains (exactly 36/54/66). This causes subtle high-frequency response differences when bias is active.

## License

Shared for collaboration with GRAME. Contact author for commercial licensing.
