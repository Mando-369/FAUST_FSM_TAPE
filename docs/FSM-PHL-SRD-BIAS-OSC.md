# Phase-Locked SR-Driven Bias Oscillator

## Concept

Real tape machines use ~100kHz bias to linearize magnetic recording. This implementation uses a **phase-locked oscillator** running fixed cycles per audio sample, making it sample-rate invariant.

## How It Works

1. **Phase accumulation**: `phi += 2π * bias_freq / SR` per sample
2. **Substep integration**: Each substep samples bias at midpoint: `sin(phi + 0.5 * dphi)`
3. **Magnetization averaging**: Output = sum of substep magnetizations / substep count

The averaging removes bias frequency while preserving audio-rate saturation.

## Mode Matrix

| Mode | Cycles | Substeps | Character |
|------|--------|----------|-----------|
| K32 | 2 | 36 | Smooth, gentle glue |
| K48 | 3 | 54 | Balanced, tape slam |
| K60 | 3 | 66 | Highest fidelity |

## Controls

- **Bias Level**: Amplitude (0-1)
- **Bias Scale**: Multiplier (1-100)
- **Resolution**: K32/K48/K60 mode selector
