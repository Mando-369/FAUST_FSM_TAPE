# JAHysteresisSchedulerLUT - Integration Guide

LUT-optimized JA hysteresis scheduler for tape saturation.
**CPU reduction: ~11% → ~1%**

## Files Required

Copy these files to your project's `Source/` folder:

```
JAHysteresisSchedulerLUT.h      # Header
JAHysteresisSchedulerLUT.cpp    # Implementation
```

Copy the LUT headers you need from `faust/`:

```
JAHysteresisLUT_K28.h    # 27 substeps (ultra lofi)
JAHysteresisLUT_K45.h    # 45 substeps (lofi)
JAHysteresisLUT_K63.h    # 63 substeps (vintage)
JAHysteresisLUT_K99.h    # 99 substeps (warm)
JAHysteresisLUT_K121.h   # 121 substeps (standard) - RECOMMENDED
JAHysteresisLUT_K187.h   # 187 substeps (high quality)
JAHysteresisLUT_K253.h   # 253 substeps (detailed)
JAHysteresisLUT_K495.h   # 495 substeps (ultra)
JAHysteresisLUT_K1045.h  # 1045 substeps (extreme)
JAHysteresisLUT_K2101.h  # 2101 substeps (beyond)
```

## Quick Start (Single Mode)

```cpp
#include "JAHysteresisSchedulerLUT.h"
#include "JAHysteresisLUT_K121.h"

class TapeProcessor
{
public:
    void prepareToPlay(double sampleRate)
    {
        JAHysteresisSchedulerLUT::PhysicsParams physics;
        // Default physics: Ms=320, a=720, k=280, c=0.18, α=0.015

        scheduler.initialise(sampleRate,
                            JAHysteresisSchedulerLUT::Mode::K121,
                            physics);

        // Load the LUT data
        scheduler.setLUT(
            JAHysteresisLUT_K121::LUT_M_END.data(),
            JAHysteresisLUT_K121::LUT_SUM_M_REST.data(),
            JAHysteresisLUT_K121::M_SIZE,
            JAHysteresisLUT_K121::H_SIZE
        );
    }

    void processBlock(float* buffer, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            double input = buffer[i] * driveGain;
            double output = scheduler.process(input);
            buffer[i] = static_cast<float>(output * outputGain);
        }
    }

private:
    JAHysteresisSchedulerLUT scheduler;
    double driveGain = 1.0;
    double outputGain = 4.0;  // ~12 dB makeup gain
};
```

## Multi-Mode Setup

For runtime mode switching, include all LUT headers:

```cpp
#include "JAHysteresisSchedulerLUT.h"
#include "JAHysteresisLUT_K28.h"
#include "JAHysteresisLUT_K45.h"
#include "JAHysteresisLUT_K63.h"
#include "JAHysteresisLUT_K99.h"
#include "JAHysteresisLUT_K121.h"
#include "JAHysteresisLUT_K187.h"
#include "JAHysteresisLUT_K253.h"
#include "JAHysteresisLUT_K495.h"
#include "JAHysteresisLUT_K1045.h"
#include "JAHysteresisLUT_K2101.h"

void setMode(int modeIndex)
{
    using Mode = JAHysteresisSchedulerLUT::Mode;

    static const struct {
        Mode mode;
        const double* lutMEnd;
        const double* lutSumMRest;
        int mSize;
        int hSize;
    } modes[] = {
        { Mode::K28,   JAHysteresisLUT_K28::LUT_M_END.data(),   JAHysteresisLUT_K28::LUT_SUM_M_REST.data(),   65, 129 },
        { Mode::K45,   JAHysteresisLUT_K45::LUT_M_END.data(),   JAHysteresisLUT_K45::LUT_SUM_M_REST.data(),   65, 129 },
        { Mode::K63,   JAHysteresisLUT_K63::LUT_M_END.data(),   JAHysteresisLUT_K63::LUT_SUM_M_REST.data(),   65, 129 },
        { Mode::K99,   JAHysteresisLUT_K99::LUT_M_END.data(),   JAHysteresisLUT_K99::LUT_SUM_M_REST.data(),   65, 129 },
        { Mode::K121,  JAHysteresisLUT_K121::LUT_M_END.data(),  JAHysteresisLUT_K121::LUT_SUM_M_REST.data(),  65, 129 },
        { Mode::K187,  JAHysteresisLUT_K187::LUT_M_END.data(),  JAHysteresisLUT_K187::LUT_SUM_M_REST.data(),  65, 129 },
        { Mode::K253,  JAHysteresisLUT_K253::LUT_M_END.data(),  JAHysteresisLUT_K253::LUT_SUM_M_REST.data(),  65, 129 },
        { Mode::K495,  JAHysteresisLUT_K495::LUT_M_END.data(),  JAHysteresisLUT_K495::LUT_SUM_M_REST.data(),  65, 129 },
        { Mode::K1045, JAHysteresisLUT_K1045::LUT_M_END.data(), JAHysteresisLUT_K1045::LUT_SUM_M_REST.data(), 65, 129 },
        { Mode::K2101, JAHysteresisLUT_K2101::LUT_M_END.data(), JAHysteresisLUT_K2101::LUT_SUM_M_REST.data(), 65, 129 },
    };

    modeIndex = std::clamp(modeIndex, 0, 9);
    scheduler.setMode(modes[modeIndex].mode);
    scheduler.setLUT(modes[modeIndex].lutMEnd,
                     modes[modeIndex].lutSumMRest,
                     modes[modeIndex].mSize,
                     modes[modeIndex].hSize);
}
```

## Important Notes

### Fixed Bias Parameters
LUTs are precomputed for:
- `bias_level = 0.41`
- `bias_scale = 11.0`

These values are baked into the LUT. Changing them via `setBiasControls()` will cause incorrect results.

### Physics Parameters
Default physics (matching LUT generation):
```cpp
Ms = 320.0;           // Saturation magnetization
aDensity = 720.0;     // Anhysteretic curve shape
kPinning = 280.0;     // Coercivity (loop width)
cReversibility = 0.18; // Reversibility ratio
alphaCoupling = 0.015; // Mean field coupling
```

### Signal Range
- Input: Normalized audio (-1.0 to 1.0), scaled by drive
- Output: Magnetization (-1.0 to 1.0), needs makeup gain

### Recommended Gain Structure
```cpp
// Drive: 0 dB default, -18 to +18 dB range
driveGain = pow(10.0, driveDb / 20.0);

// Output: +12 dB default makeup gain
outputGain = pow(10.0, outputDb / 20.0);

// Process
input = audioSample * driveGain;
output = scheduler.process(input);
audioSample = output * outputGain;
```

## Mode Characteristics

| Mode | Substeps | Cycles | Character |
|------|----------|--------|-----------|
| K28  | 27       | 1.5    | Ultra lofi, maximum grit |
| K45  | 45       | 2.5    | Lofi, crunchy |
| K63  | 63       | 3.5    | Vintage, classic tape |
| K99  | 99       | 4.5    | Warm, smooth |
| K121 | 121      | 5.5    | **Standard** (recommended) |
| K187 | 187      | 8.5    | High quality |
| K253 | 253      | 11.5   | Very detailed |
| K495 | 495      | 22.5   | Ultra detailed |
| K1045| 1045     | 47.5   | Extreme |
| K2101| 2101     | 95.5   | Beyond physical |

## How It Works

Original scheduler: loops through 66-121 substeps per sample (~11% CPU)

LUT-optimized:
1. Execute **substep 0** with real JA physics (cross-sample dependency)
2. Look up **M_end** and **sumM_rest** from 2D LUT using (M1, H_audio)
3. Return `(M1 + sumM_rest) / totalSubsteps`

Result: ~1% CPU regardless of substep count.

## Memory Usage

Each LUT: 65 × 129 × 2 arrays × 8 bytes = ~134 KB per mode

All 10 modes loaded: ~1.3 MB total
