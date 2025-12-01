#include "JAHysteresisSchedulerLUT.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace
{
constexpr double kTwoPi = std::numbers::pi * 2.0;
}

void JAHysteresisSchedulerLUT::initialise(double newSampleRate,
                                          Mode mode,
                                          const PhysicsParams& newPhysics)
{
    sampleRate = std::max(1.0, newSampleRate);
    currentMode = mode;
    physics = newPhysics;

    reset();
    updateDerived();
    updateModeDerived();
}

void JAHysteresisSchedulerLUT::reset() noexcept
{
    biasPhase = 0.0;
    substepPhase = 0.0;
    MPrev = 0.0;
    HPrev = 0.0;
}

void JAHysteresisSchedulerLUT::setMode(Mode mode) noexcept
{
    if (currentMode == mode)
        return;

    currentMode = mode;
    updateModeDerived();
}

void JAHysteresisSchedulerLUT::setPhysics(const PhysicsParams& newPhysics) noexcept
{
    physics = newPhysics;
    updateDerived();
}

void JAHysteresisSchedulerLUT::setBiasControls(double level, double scale) noexcept
{
    // Note: LUT is precomputed for fixed bias values (0.41, 11.0)
    // Changing these values will cause mismatch with LUT!
    biasLevel = std::clamp(level, 0.0, 1.0);
    biasScale = std::max(scale, 0.0);
    updateDerived();
}

void JAHysteresisSchedulerLUT::setLUT(const double* lutMEnd,
                                       const double* lutSumMRest,
                                       int mSize,
                                       int hSize) noexcept
{
    lutConfig.lutMEnd = lutMEnd;
    lutConfig.lutSumMRest = lutSumMRest;
    lutConfig.mSize = mSize;
    lutConfig.hSize = hSize;
    // Default grid ranges (can be extended if needed)
    lutConfig.mMin = -1.0;
    lutConfig.mMax = 1.0;
    lutConfig.hMin = -1.0;
    lutConfig.hMax = 1.0;
}

double JAHysteresisSchedulerLUT::process(double HAudio) noexcept
{
    // Phase at start of this sample (before substeps)
    const double phaseStart = biasPhase;

    // Substep phase increment
    const double dphi = kTwoPi / static_cast<double>(lutConfig.totalSubsteps / biasCyclesPerSample);

    // Execute substep 0 with real physics
    const double biasOffset0 = std::sin(phaseStart + dphi * 0.5);
    const double M1 = executeSubstep0(biasOffset0, HAudio);

    // Look up remainder from LUT (substeps 1..N-1)
    const double M_end = bilinearLookup(lutConfig.lutMEnd, M1, HAudio);
    const double sumM_rest = bilinearLookup(lutConfig.lutSumMRest, M1, HAudio);

    // Update state for next sample
    MPrev = M_end;
    HPrev = HAudio + biasAmplitude * std::sin(phaseStart + dphi * (totalSubsteps - 0.5));

    // Advance phase by biasCycles full cycles
    biasPhase = std::fmod(phaseStart + biasCyclesPerSample * kTwoPi, kTwoPi);

    // Return average magnetization: (M1 + sum of M2..MN) / N
    return (M1 + sumM_rest) * invTotalSubsteps;
}

// -----------------------------------------------------------------------------
void JAHysteresisSchedulerLUT::updateDerived() noexcept
{
    MsSafe = std::max(physics.Ms, 1.0e-6);
    alphaNorm = physics.alphaCoupling;
    aNorm = physics.aDensity / MsSafe;
    invANorm = 1.0 / std::max(aNorm, 1.0e-9);
    kNorm = physics.kPinning / MsSafe;
    cNorm = physics.cReversibility;
    biasAmplitude = biasLevel * biasScale;
}

void JAHysteresisSchedulerLUT::updateModeDerived() noexcept
{
    // Set mode-specific parameters
    // These must match the LUT generator configuration!
    switch (currentMode)
    {
        case Mode::K28:
            biasCyclesPerSample = 1.5;
            totalSubsteps = 27;
            break;
        case Mode::K45:
            biasCyclesPerSample = 2.5;
            totalSubsteps = 45;
            break;
        case Mode::K63:
            biasCyclesPerSample = 3.5;
            totalSubsteps = 63;
            break;
        case Mode::K99:
            biasCyclesPerSample = 4.5;
            totalSubsteps = 99;
            break;
        case Mode::K121:
            biasCyclesPerSample = 5.5;
            totalSubsteps = 121;
            break;
        case Mode::K187:
            biasCyclesPerSample = 8.5;
            totalSubsteps = 187;
            break;
        case Mode::K253:
            biasCyclesPerSample = 11.5;
            totalSubsteps = 253;
            break;
        case Mode::K495:
            biasCyclesPerSample = 22.5;
            totalSubsteps = 495;
            break;
        case Mode::K1045:
            biasCyclesPerSample = 47.5;
            totalSubsteps = 1045;
            break;
        case Mode::K2101:
            biasCyclesPerSample = 95.5;
            totalSubsteps = 2101;
            break;
    }

    invTotalSubsteps = 1.0 / static_cast<double>(totalSubsteps);

    // Update LUT config - this should point to the appropriate LUT data
    lutConfig.totalSubsteps = totalSubsteps;
    lutConfig.biasCycles = biasCyclesPerSample;
}

double JAHysteresisSchedulerLUT::fastTanh(double x) const noexcept
{
    const double clamped = std::clamp(x, -3.0, 3.0);
    const double x2 = clamped * clamped;
    return clamped * (27.0 + x2) / (27.0 + 9.0 * x2);
}

double JAHysteresisSchedulerLUT::executeSubstep0(double biasOffset,
                                                  double HAudio) noexcept
{
    // Execute one JA substep (identical to original executeSubstep)
    const double HNew = HAudio + biasAmplitude * biasOffset;
    const double dH = HNew - HPrev;
    const double He = HNew + alphaNorm * MPrev;

    const double xMan = He * invANorm;
    const double ManE = fastTanh(xMan);
    const double ManE2 = ManE * ManE;
    const double dMan_dH = (1.0 - ManE2) * invANorm;

    const double dir = (dH >= 0.0) ? 1.0 : -1.0;
    const double pin = dir * kNorm - alphaNorm * (ManE - MPrev);
    const double invPin = 1.0 / (pin + 1.0e-6);

    const double denom = 1.0 - cNorm * alphaNorm * dMan_dH;
    const double invDenom = 1.0 / (denom + 1.0e-9);
    const double dMdH = (cNorm * dMan_dH + (ManE - MPrev) * invPin) * invDenom;
    const double dMStep = dMdH * dH;

    const double MNew = std::clamp(MPrev + dMStep, -1.0, 1.0);

    // Note: We don't update MPrev/HPrev here - that's done in process() after LUT lookup
    return MNew;
}

double JAHysteresisSchedulerLUT::bilinearLookup(const double* lut,
                                                 double m,
                                                 double h) const noexcept
{
    if (lut == nullptr)
        return 0.0;

    // Normalize coordinates to [0, 1]
    const double mNorm = std::clamp(
        (m - lutConfig.mMin) / (lutConfig.mMax - lutConfig.mMin), 0.0, 1.0);
    const double hNorm = std::clamp(
        (h - lutConfig.hMin) / (lutConfig.hMax - lutConfig.hMin), 0.0, 1.0);

    // Scale to grid indices
    const double mScaled = mNorm * static_cast<double>(lutConfig.mSize - 1);
    const double hScaled = hNorm * static_cast<double>(lutConfig.hSize - 1);

    // Get integer indices (clamped to safe range)
    const int mIdx = std::min(static_cast<int>(std::floor(mScaled)), lutConfig.mSize - 2);
    const int hIdx = std::min(static_cast<int>(std::floor(hScaled)), lutConfig.hSize - 2);

    // Fractional parts for interpolation
    const double mFrac = mScaled - static_cast<double>(mIdx);
    const double hFrac = hScaled - static_cast<double>(hIdx);

    // 2D index computation: row-major order
    const int idx00 = mIdx * lutConfig.hSize + hIdx;
    const int idx01 = mIdx * lutConfig.hSize + (hIdx + 1);
    const int idx10 = (mIdx + 1) * lutConfig.hSize + hIdx;
    const int idx11 = (mIdx + 1) * lutConfig.hSize + (hIdx + 1);

    // Fetch corner values
    const double v00 = lut[idx00];
    const double v01 = lut[idx01];
    const double v10 = lut[idx10];
    const double v11 = lut[idx11];

    // Bilinear interpolation
    return v00 * (1.0 - mFrac) * (1.0 - hFrac)
         + v01 * (1.0 - mFrac) * hFrac
         + v10 * mFrac * (1.0 - hFrac)
         + v11 * mFrac * hFrac;
}
