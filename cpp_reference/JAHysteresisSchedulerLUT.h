#pragma once

#include <array>
#include <cmath>
#include <cstddef>

/**
 * JAHysteresisSchedulerLUT
 *
 * LUT-optimized version of the JA hysteresis scheduler.
 * Only computes substep 0 (cross-sample dependency), then uses
 * precomputed 2D LUT for the remainder.
 *
 * Expected CPU reduction: ~11% → ~1%
 */
class JAHysteresisSchedulerLUT
{
public:
    enum class Mode
    {
        K28 = 0,  ///< 1.5 cycles, 27 substeps (ultra lofi)
        K45,      ///< 2.5 cycles, 45 substeps (lofi)
        K63,      ///< 3.5 cycles, 63 substeps (vintage)
        K99,      ///< 4.5 cycles, 99 substeps (warm)
        K121,     ///< 5.5 cycles, 121 substeps (standard)
        K187,     ///< 8.5 cycles, 187 substeps (high quality)
        K253,     ///< 11.5 cycles, 253 substeps (detailed)
        K495,     ///< 22.5 cycles, 495 substeps (ultra)
        K1045,    ///< 47.5 cycles, 1045 substeps (extreme)
        K2101     ///< 95.5 cycles, 2101 substeps (beyond)
    };

    struct PhysicsParams
    {
        double Ms = 320.0;
        double aDensity = 720.0;
        double kPinning = 280.0;
        double cReversibility = 0.18;
        double alphaCoupling = 0.015;
    };

    // LUT configuration (must match generated LUT)
    struct LUTConfig
    {
        int mSize = 65;
        int hSize = 129;
        double mMin = -1.0;
        double mMax = 1.0;
        double hMin = -1.0;
        double hMax = 1.0;
        const double* lutMEnd = nullptr;
        const double* lutSumMRest = nullptr;
        int totalSubsteps = 121;
        double biasCycles = 5.5;
    };

    void initialise(double sampleRate, Mode mode, const PhysicsParams& physics);
    void reset() noexcept;

    void setMode(Mode mode) noexcept;
    void setPhysics(const PhysicsParams& physics) noexcept;
    void setBiasControls(double biasLevel, double biasScale) noexcept;

    /** Set the LUT data pointers for the current mode.
     *  Must be called after setMode() with matching LUT data.
     *  @param lutMEnd Pointer to M_end LUT array (mSize * hSize elements)
     *  @param lutSumMRest Pointer to sumM_rest LUT array (mSize * hSize elements)
     *  @param mSize M grid size (default 65)
     *  @param hSize H grid size (default 129)
     */
    void setLUT(const double* lutMEnd, const double* lutSumMRest,
                int mSize = 65, int hSize = 129) noexcept;

    /** Process one host sample worth of audio field and return averaged magnetisation. */
    double process(double HAudio) noexcept;

private:
    // --- configuration -----------------------------------------------------
    double sampleRate { 48000.0 };
    Mode currentMode { Mode::K121 };
    PhysicsParams physics {};
    double biasLevel { 0.41 };  // Fixed for LUT compatibility
    double biasScale { 11.0 };  // Fixed for LUT compatibility
    LUTConfig lutConfig {};

    // --- derived constants -------------------------------------------------
    double MsSafe { 1.0 };
    double alphaNorm { 0.0 };
    double aNorm { 1.0 };
    double invANorm { 1.0 };
    double kNorm { 0.0 };
    double cNorm { 0.0 };
    double biasAmplitude { 0.0 };

    // Bias oscillator
    double biasCyclesPerSample { 5.5 };
    int totalSubsteps { 121 };
    double biasPhase { 0.0 };
    double substepPhase { 0.0 };
    double invTotalSubsteps { 1.0 / 121.0 };

    // JA state
    double MPrev { 0.0 };
    double HPrev { 0.0 };

    // --- helpers -----------------------------------------------------------
    void updateDerived() noexcept;
    void updateModeDerived() noexcept;
    double fastTanh(double x) const noexcept;

    /** Execute substep 0 and return M1 */
    double executeSubstep0(double biasOffset, double HAudio) noexcept;

    /** Bilinear interpolation lookup */
    double bilinearLookup(const double* lut, double m, double h) const noexcept;
};
