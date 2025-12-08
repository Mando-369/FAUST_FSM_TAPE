#!/usr/bin/env python3
"""
Generate 2D Lookup Table for JA Hysteresis Optimization

This script precomputes the JA hysteresis response for substeps N..end,
allowing the audio loop to compute substeps 0..N-1 in real-time
and look up the remainder from the LUT.

The LUT maps (M_in, HAudio) -> (M_end, sumM_rest)

Features:
- RK4 integration for improved accuracy
- Configurable number of real substeps before LUT
- Validation metrics comparing LUT vs full computation
- Catmull-Rom interpolation in FAUST output

Usage:
    python generate_ja_lut.py --mode K121 --real-substeps 12 --use-rk4 --validate
"""

import numpy as np
import argparse
from pathlib import Path
from typing import Tuple, NamedTuple, Callable
import time


class PhysicsParams(NamedTuple):
    """JA Hysteresis physics parameters (matching C++ defaults)"""
    Ms: float = 320.0           # Saturation magnetization
    a_density: float = 720.0    # Anhysteretic curve shape
    k_pinning: float = 280.0    # Coercivity (loop width)
    c_reversibility: float = 0.18  # Reversibility ratio
    alpha_coupling: float = 0.015  # Mean field coupling


class ModeConfig(NamedTuple):
    """Bias mode configuration"""
    name: str
    cycles_per_sample: float
    substeps_per_cycle: int

    @property
    def total_substeps(self) -> int:
        return int(self.cycles_per_sample * self.substeps_per_cycle)

    @property
    def phase_span(self) -> float:
        """Total phase span in radians"""
        return 2.0 * np.pi * self.cycles_per_sample

    def get_variants(self) -> list:
        """
        Returns variants with N-1, N, N+1 substeps.
        Each variant has the SAME phase span but different substep count.
        """
        base = self.total_substeps
        return [
            ModeConfigVariant(f"K{base-1}", self.phase_span, base - 1),
            ModeConfigVariant(f"K{base}", self.phase_span, base),
            ModeConfigVariant(f"K{base+1}", self.phase_span, base + 1),
        ]


class ModeConfigVariant(NamedTuple):
    """Variant mode config with fixed phase span and variable substeps"""
    name: str
    phase_span: float
    total_substeps: int


# Mode configurations
# Pattern: half-integer cycles + odd substeps = rich harmonic content
# Integer cycles avoid bias leakage (12kHz tone)
MODES = {
    'K28': ModeConfig('K28', 1.5, 18),
    'K45': ModeConfig('K45', 2.5, 18),
    'K63': ModeConfig('K63', 3.5, 18),
    'K96': ModeConfig('K96', 4.0, 24),   # 4 integer cycles × 24 = 96 (no bias leakage)
    'K99': ModeConfig('K99', 4.5, 22),
    'K121': ModeConfig('K121', 5.5, 22),
    'K187': ModeConfig('K187', 8.5, 22),
    'K253': ModeConfig('K253', 11.5, 22),
    'K495': ModeConfig('K495', 22.5, 22),
    'K1045': ModeConfig('K1045', 47.5, 22),
    'K2101': ModeConfig('K2101', 95.5, 22),
}


def generate_bias_lut(phase_span: float, total_substeps: int) -> np.ndarray:
    """Generate bias sin() values for all substeps (midpoint sampling)"""
    n = total_substeps
    dphi = phase_span / n
    indices = np.arange(n)
    return np.sin((indices + 0.5) * dphi)


def get_derived_constants(physics: PhysicsParams, diff_scale: float = 1.0) -> dict:
    """Compute derived constants from physics parameters"""
    Ms_safe = max(physics.Ms, 1e-6)
    a_norm = physics.a_density / Ms_safe
    return {
        'Ms_safe': Ms_safe,
        'alpha_norm': physics.alpha_coupling,
        'a_norm': a_norm,
        'inv_a_norm': 1.0 / max(a_norm, 1e-9),
        'k_norm': physics.k_pinning / Ms_safe,
        'c_norm': physics.c_reversibility,
        'diff_scale': diff_scale,  # Soft clamp on (Man_e - M) for stabilization
        'sigma': 1e-3,  # Safety margin for pinning term (matches DSP)
    }


def ja_dMdH(M: float, H_new: float, H_prev: float, consts: dict) -> float:
    """
    Compute dM/dH for JA hysteresis (the derivative).
    Used by both Euler and RK4 methods.
    """
    dH = H_new - H_prev
    He = H_new + consts['alpha_norm'] * M

    x_man = He * consts['inv_a_norm']
    Man_e = np.tanh(x_man)
    Man_e2 = Man_e * Man_e
    dMan_dH = (1.0 - Man_e2) * consts['inv_a_norm']

    # Soft clamp on (Man_e - M) for stabilization (matches DSP diff_scale)
    diff = Man_e - M
    diff_scale = consts.get('diff_scale', 1.0)
    diff_clamped = diff / (1.0 + abs(diff) * diff_scale)

    direction = 1.0 if dH >= 0.0 else -1.0
    pin = direction * consts['k_norm'] - consts['alpha_norm'] * diff_clamped
    sigma = consts.get('sigma', 1e-3)
    inv_pin = 1.0 / (pin + sigma)

    denom = 1.0 - consts['c_norm'] * consts['alpha_norm'] * dMan_dH
    inv_denom = 1.0 / (denom + 1e-9)

    return (consts['c_norm'] * dMan_dH + diff_clamped * inv_pin) * inv_denom


def ja_substep_euler(
    M_prev: float,
    H_prev: float,
    H_audio: float,
    bias_offset: float,
    bias_amplitude: float,
    consts: dict
) -> Tuple[float, float]:
    """
    Execute one JA substep using Euler integration.
    Returns (M_new, H_new).
    """
    H_new = H_audio + bias_amplitude * bias_offset
    dH = H_new - H_prev

    dMdH = ja_dMdH(M_prev, H_new, H_prev, consts)
    dM_step = dMdH * dH

    M_new = np.clip(M_prev + dM_step, -1.0, 1.0)
    return M_new, H_new


def ja_substep_rk4(
    M_prev: float,
    H_prev: float,
    H_audio: float,
    bias_offset: float,
    bias_amplitude: float,
    consts: dict
) -> Tuple[float, float]:
    """
    Execute one JA substep using RK4 integration.
    Returns (M_new, H_new).

    RK4 samples the derivative at 4 points for better accuracy:
    - k1: slope at start
    - k2: slope at midpoint using k1
    - k3: slope at midpoint using k2
    - k4: slope at end using k3
    """
    H_new = H_audio + bias_amplitude * bias_offset
    dH = H_new - H_prev

    if abs(dH) < 1e-12:
        return M_prev, H_new

    # RK4 stages - integrate M from H_prev to H_new
    # Stage 1: slope at start
    k1 = ja_dMdH(M_prev, H_prev, H_prev, consts) * dH

    # Stage 2: slope at midpoint using k1
    H_mid = H_prev + 0.5 * dH
    k2 = ja_dMdH(M_prev + 0.5 * k1, H_mid, H_prev, consts) * dH

    # Stage 3: slope at midpoint using k2
    k3 = ja_dMdH(M_prev + 0.5 * k2, H_mid, H_prev, consts) * dH

    # Stage 4: slope at end using k3
    k4 = ja_dMdH(M_prev + k3, H_new, H_prev, consts) * dH

    # Weighted average
    M_new = M_prev + (k1 + 2.0*k2 + 2.0*k3 + k4) / 6.0
    M_new = np.clip(M_new, -1.0, 1.0)

    return M_new, H_new


def compute_full_response(
    M_init: float,
    H_audio: float,
    bias_lut: np.ndarray,
    bias_amplitude: float,
    consts: dict,
    substep_func: Callable
) -> Tuple[float, float]:
    """
    Compute ALL substeps with real physics.
    Returns (M_end, sum_M_all).
    Used for validation.
    """
    n = len(bias_lut)
    M = M_init
    H = H_audio  # Initial H
    sum_M = 0.0

    for i in range(n):
        M, H = substep_func(M, H, H_audio, bias_lut[i], bias_amplitude, consts)
        sum_M += M

    return M, sum_M


def compute_real_substeps(
    M_init: float,
    H_init: float,
    H_audio: float,
    bias_lut: np.ndarray,
    bias_amplitude: float,
    consts: dict,
    num_real: int,
    substep_func: Callable
) -> Tuple[float, float, float]:
    """
    Compute the first num_real substeps with real physics.
    Returns (M_after_real, H_after_real, sum_M_real).
    """
    M = M_init
    H = H_init
    sum_M = 0.0

    for i in range(num_real):
        M, H = substep_func(M, H, H_audio, bias_lut[i], bias_amplitude, consts)
        sum_M += M

    return M, H, sum_M


def compute_remainder_response(
    M_start: float,
    H_audio: float,
    bias_lut: np.ndarray,
    bias_amplitude: float,
    consts: dict,
    start_substep: int,
    substep_func: Callable
) -> Tuple[float, float]:
    """
    Compute substeps start_substep..N-1 given the state after real substeps.

    Returns (M_end, sumM_rest) where:
    - M_end: final magnetization after all substeps
    - sumM_rest: sum of magnetizations from substeps start_substep..N-1

    Args:
        M_start: Magnetization after the last real substep
        H_audio: Audio input signal
        bias_lut: Precomputed bias oscillator values
        bias_amplitude: Bias amplitude (level * scale)
        consts: Derived physics constants
        start_substep: First substep to compute (0-indexed)
        substep_func: Integration function (Euler or RK4)
    """
    n = len(bias_lut)

    if start_substep >= n:
        return M_start, 0.0

    # Initialize with state after real substeps
    M = M_start
    # H after previous substep
    H = H_audio + bias_amplitude * bias_lut[start_substep - 1] if start_substep > 0 else H_audio

    sum_M = 0.0

    # Run substeps from start_substep to N-1
    for i in range(start_substep, n):
        M, H = substep_func(M, H, H_audio, bias_lut[i], bias_amplitude, consts)
        sum_M += M

    return M, sum_M


def generate_2d_lut(
    name: str,
    phase_span: float,
    total_substeps: int,
    physics: PhysicsParams,
    bias_level: float = 0.41,
    bias_scale: float = 11.0,
    m_size: int = 65,
    h_size: int = 129,
    h_range: Tuple[float, float] = (-1.0, 1.0),
    real_substeps: int = 1,
    use_rk4: bool = False
) -> Tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """
    Generate the 2D LUT for (M_in, HAudio) -> (M_end, sumM_rest).

    Args:
        real_substeps: Number of substeps computed in real-time (LUT starts after this)
        use_rk4: Use RK4 integration instead of Euler

    Returns:
        m_grid: M axis values
        h_grid: H axis values
        lut_M_end: 2D array of M_end values
        lut_sumM_rest: 2D array of sumM_rest values
    """
    bias_amplitude = bias_level * bias_scale
    bias_lut = generate_bias_lut(phase_span, total_substeps)
    consts = get_derived_constants(physics)

    substep_func = ja_substep_rk4 if use_rk4 else ja_substep_euler
    method_name = "RK4" if use_rk4 else "Euler"

    # Create grids
    m_grid = np.linspace(-1.0, 1.0, m_size)
    h_grid = np.linspace(h_range[0], h_range[1], h_size)

    # Initialize output arrays
    lut_M_end = np.zeros((m_size, h_size))
    lut_sumM_rest = np.zeros((m_size, h_size))

    total_points = m_size * h_size
    count = 0

    print(f"\nGenerating LUT for {name}:")
    print(f"  Phase span: {phase_span:.4f} rad ({phase_span/np.pi:.2f}π)")
    print(f"  Total substeps: {total_substeps}")
    print(f"  Real substeps (FAUST): {real_substeps} ({100*real_substeps/total_substeps:.1f}%)")
    print(f"  LUT substeps: {real_substeps}..{total_substeps-1} ({total_substeps - real_substeps} steps)")
    print(f"  Integration method: {method_name}")
    print(f"  Grid: {m_size}x{h_size} = {total_points} points")
    print(f"  Bias amplitude: {bias_amplitude:.3f}")

    start_time = time.time()

    for i, M_in in enumerate(m_grid):
        for j, H_audio in enumerate(h_grid):
            # M_in represents M after real substeps (M_real_substeps)
            # LUT computes the remainder
            M_end, sumM_rest = compute_remainder_response(
                M_in, H_audio, bias_lut, bias_amplitude, consts,
                start_substep=real_substeps,
                substep_func=substep_func
            )
            lut_M_end[i, j] = M_end
            lut_sumM_rest[i, j] = sumM_rest

            count += 1
            if count % 2000 == 0:
                elapsed = time.time() - start_time
                rate = count / elapsed
                remaining = (total_points - count) / rate
                print(f"  Progress: {count}/{total_points} ({100*count/total_points:.1f}%) - ETA: {remaining:.1f}s")

    elapsed = time.time() - start_time
    print(f"  Done in {elapsed:.1f}s")

    return m_grid, h_grid, lut_M_end, lut_sumM_rest


def validate_lut(
    m_grid: np.ndarray,
    h_grid: np.ndarray,
    lut_M_end: np.ndarray,
    lut_sumM_rest: np.ndarray,
    phase_span: float,
    total_substeps: int,
    physics: PhysicsParams,
    bias_level: float,
    bias_scale: float,
    real_substeps: int,
    use_rk4: bool,
    sample_every: int = 4
) -> dict:
    """
    Validate LUT accuracy by comparing against full physics computation.

    Tests at sampled grid points:
    1. Compute real substeps 0..real_substeps-1
    2. Look up remainder from LUT (with bilinear interpolation)
    3. Compare against full physics computation

    Returns dict with error statistics.
    """
    print(f"\nValidating LUT accuracy (sampling every {sample_every} points)...")

    bias_amplitude = bias_level * bias_scale
    bias_lut = generate_bias_lut(phase_span, total_substeps)
    consts = get_derived_constants(physics)
    substep_func = ja_substep_rk4 if use_rk4 else ja_substep_euler

    m_size, h_size = lut_M_end.shape

    errors_M_end = []
    errors_sum_M = []
    errors_M_avg = []

    test_points = 0

    # Test at sampled grid points
    for i in range(0, m_size, sample_every):
        for j in range(0, h_size, sample_every):
            M_init = m_grid[i]
            H_audio = h_grid[j]

            # Full physics: compute ALL substeps from M_init
            M_full_end, sum_M_full = compute_full_response(
                M_init, H_audio, bias_lut, bias_amplitude, consts, substep_func
            )

            # LUT approach: compute real substeps, then lookup
            # For validation, we simulate what FAUST would do:
            # 1. Start from M_init (representing initial state)
            # 2. Compute real_substeps with physics
            # 3. Look up remainder from LUT

            # Compute real substeps
            M_after_real, H_after_real, sum_M_real = compute_real_substeps(
                M_init, H_audio, H_audio, bias_lut, bias_amplitude, consts,
                real_substeps, substep_func
            )

            # LUT lookup (bilinear interpolation for validation)
            # Normalize M_after_real to grid index
            m_norm = (M_after_real - m_grid[0]) / (m_grid[-1] - m_grid[0])
            h_norm = (H_audio - h_grid[0]) / (h_grid[-1] - h_grid[0])

            m_idx = m_norm * (m_size - 1)
            h_idx = h_norm * (h_size - 1)

            # Bilinear interpolation
            m_idx = np.clip(m_idx, 0, m_size - 1.001)
            h_idx = np.clip(h_idx, 0, h_size - 1.001)

            m0, m1 = int(m_idx), min(int(m_idx) + 1, m_size - 1)
            h0, h1 = int(h_idx), min(int(h_idx) + 1, h_size - 1)

            m_frac = m_idx - m0
            h_frac = h_idx - h0

            # Interpolate M_end
            v00 = lut_M_end[m0, h0]
            v01 = lut_M_end[m0, h1]
            v10 = lut_M_end[m1, h0]
            v11 = lut_M_end[m1, h1]

            M_end_lut = (v00 * (1-m_frac) * (1-h_frac) +
                         v01 * (1-m_frac) * h_frac +
                         v10 * m_frac * (1-h_frac) +
                         v11 * m_frac * h_frac)

            # Interpolate sumM_rest
            v00 = lut_sumM_rest[m0, h0]
            v01 = lut_sumM_rest[m0, h1]
            v10 = lut_sumM_rest[m1, h0]
            v11 = lut_sumM_rest[m1, h1]

            sum_M_rest_lut = (v00 * (1-m_frac) * (1-h_frac) +
                              v01 * (1-m_frac) * h_frac +
                              v10 * m_frac * (1-h_frac) +
                              v11 * m_frac * h_frac)

            # Total from LUT approach
            sum_M_lut = sum_M_real + sum_M_rest_lut

            # Compute errors
            errors_M_end.append(abs(M_full_end - M_end_lut))
            errors_sum_M.append(abs(sum_M_full - sum_M_lut))

            # Average magnetization error (what actually matters for audio)
            M_avg_full = sum_M_full / total_substeps
            M_avg_lut = sum_M_lut / total_substeps
            errors_M_avg.append(abs(M_avg_full - M_avg_lut))

            test_points += 1

    results = {
        'test_points': test_points,
        'M_end_mean_error': np.mean(errors_M_end),
        'M_end_max_error': np.max(errors_M_end),
        'M_end_std_error': np.std(errors_M_end),
        'sum_M_mean_error': np.mean(errors_sum_M),
        'sum_M_max_error': np.max(errors_sum_M),
        'M_avg_mean_error': np.mean(errors_M_avg),
        'M_avg_max_error': np.max(errors_M_avg),
        'M_avg_std_error': np.std(errors_M_avg),
    }

    print(f"\nValidation Results ({test_points} test points):")
    print(f"  M_end error:  mean={results['M_end_mean_error']:.2e}, max={results['M_end_max_error']:.2e}")
    print(f"  sum_M error:  mean={results['sum_M_mean_error']:.2e}, max={results['sum_M_max_error']:.2e}")
    print(f"  M_avg error:  mean={results['M_avg_mean_error']:.2e}, max={results['M_avg_max_error']:.2e}")
    print(f"  (M_avg is what matters for audio output)")

    return results


def export_cpp_header(
    m_grid: np.ndarray,
    h_grid: np.ndarray,
    lut_M_end: np.ndarray,
    lut_sumM_rest: np.ndarray,
    name: str,
    total_substeps: int,
    real_substeps: int,
    output_path: Path
):
    """Export LUT as C++ header file"""
    m_size, h_size = lut_M_end.shape

    with open(output_path, 'w') as f:
        f.write(f"// Auto-generated JA Hysteresis LUT for {name}\n")
        f.write(f"// Grid: {m_size} x {h_size} = {m_size * h_size} points\n")
        f.write(f"// Total substeps: {total_substeps}\n")
        f.write(f"// Real substeps (compute in audio loop): 0..{real_substeps-1}\n")
        f.write(f"// LUT substeps: {real_substeps}..{total_substeps - 1}\n\n")

        f.write("#pragma once\n\n")
        f.write("#include <array>\n\n")
        f.write(f"namespace JAHysteresisLUT_{name} {{\n\n")

        f.write(f"constexpr int M_SIZE = {m_size};\n")
        f.write(f"constexpr int H_SIZE = {h_size};\n")
        f.write(f"constexpr int TOTAL_SUBSTEPS = {total_substeps};\n")
        f.write(f"constexpr int REAL_SUBSTEPS = {real_substeps};\n")
        f.write(f"constexpr double M_MIN = {m_grid[0]:.6f};\n")
        f.write(f"constexpr double M_MAX = {m_grid[-1]:.6f};\n")
        f.write(f"constexpr double H_MIN = {h_grid[0]:.6f};\n")
        f.write(f"constexpr double H_MAX = {h_grid[-1]:.6f};\n\n")

        flat_M_end = lut_M_end.flatten()
        flat_sumM_rest = lut_sumM_rest.flatten()

        f.write(f"constexpr std::array<double, {len(flat_M_end)}> LUT_M_END = {{\n")
        for i, val in enumerate(flat_M_end):
            f.write(f"    {val:.10e}")
            if i < len(flat_M_end) - 1:
                f.write(",")
            if (i + 1) % 4 == 0:
                f.write("\n")
        f.write("};\n\n")

        f.write(f"constexpr std::array<double, {len(flat_sumM_rest)}> LUT_SUM_M_REST = {{\n")
        for i, val in enumerate(flat_sumM_rest):
            f.write(f"    {val:.10e}")
            if i < len(flat_sumM_rest) - 1:
                f.write(",")
            if (i + 1) % 4 == 0:
                f.write("\n")
        f.write("};\n\n")

        f.write("} // namespace\n")

    print(f"  Exported C++ header: {output_path}")


def export_faust_lib(
    m_grid: np.ndarray,
    h_grid: np.ndarray,
    lut_M_end: np.ndarray,
    lut_sumM_rest: np.ndarray,
    name: str,
    total_substeps: int,
    real_substeps: int,
    phase_span: float,
    output_path: Path
):
    """Export LUT as FAUST library file with Catmull-Rom interpolation"""
    m_size, h_size = lut_M_end.shape

    flat_M_end = lut_M_end.flatten()
    flat_sumM_rest = lut_sumM_rest.flatten()

    with open(output_path, 'w') as f:
        f.write(f"// Auto-generated JA Hysteresis LUT for {name}\n")
        f.write(f"// Grid: {m_size} x {h_size} = {m_size * h_size} points\n")
        f.write(f"// Total substeps: {total_substeps}\n")
        f.write(f"// Real substeps (compute in audio loop): 0..{real_substeps-1}\n")
        f.write(f"// LUT substeps: {real_substeps}..{total_substeps - 1}\n")
        f.write(f"// Phase span: {phase_span:.6f} rad ({phase_span/np.pi:.2f}π)\n\n")

        f.write("import(\"stdfaust.lib\");\n\n")

        prefix = name.lower()
        f.write(f"// Grid parameters for {name}\n")
        f.write(f"ja_lut_{prefix}_m_size = {m_size};\n")
        f.write(f"ja_lut_{prefix}_h_size = {h_size};\n")
        f.write(f"ja_lut_{prefix}_total_substeps = {total_substeps};\n")
        f.write(f"ja_lut_{prefix}_real_substeps = {real_substeps};\n")
        f.write(f"ja_lut_{prefix}_m_min = {m_grid[0]:.6f};\n")
        f.write(f"ja_lut_{prefix}_m_max = {m_grid[-1]:.6f};\n")
        f.write(f"ja_lut_{prefix}_h_min = {h_grid[0]:.6f};\n")
        f.write(f"ja_lut_{prefix}_h_max = {h_grid[-1]:.6f};\n")
        f.write(f"ja_lut_{prefix}_phase_span = {phase_span:.6f};\n\n")

        # Write waveform for M_end
        f.write(f"// M_end LUT ({len(flat_M_end)} values)\n")
        f.write(f"ja_lut_{prefix}_m_end = waveform{{\n")
        for i, val in enumerate(flat_M_end):
            f.write(f"    {val:.10e}")
            if i < len(flat_M_end) - 1:
                f.write(",")
            if (i + 1) % 4 == 0:
                f.write("\n")
        f.write("};\n\n")

        # Write waveform for sumM_rest
        f.write(f"// sumM_rest LUT ({len(flat_sumM_rest)} values)\n")
        f.write(f"ja_lut_{prefix}_sum_m_rest = waveform{{\n")
        for i, val in enumerate(flat_sumM_rest):
            f.write(f"    {val:.10e}")
            if i < len(flat_sumM_rest) - 1:
                f.write(",")
            if (i + 1) % 4 == 0:
                f.write("\n")
        f.write("};\n\n")

        # Helper functions
        f.write("// 2D index computation\n")
        f.write(f"ja_lut_{prefix}_idx(m_idx, h_idx) = m_idx * ja_lut_{prefix}_h_size + h_idx;\n\n")

        f.write("// Normalize M to [0, 1] range\n")
        f.write(f"ja_lut_{prefix}_m_norm(m) = (m - ja_lut_{prefix}_m_min) / (ja_lut_{prefix}_m_max - ja_lut_{prefix}_m_min);\n\n")

        f.write("// Normalize H to [0, 1] range\n")
        f.write(f"ja_lut_{prefix}_h_norm(h) = (h - ja_lut_{prefix}_h_min) / (ja_lut_{prefix}_h_max - ja_lut_{prefix}_h_min);\n\n")

        # Catmull-Rom helper
        f.write("// 1D Catmull-Rom interpolation: p0,p1,p2,p3 are 4 consecutive points, t in [0,1]\n")
        f.write(f"ja_catmull_rom_{prefix}(p0, p1, p2, p3, t) = 0.5 * (\n")
        f.write("    2.0*p1 +\n")
        f.write("    (-p0 + p2) * t +\n")
        f.write("    (2.0*p0 - 5.0*p1 + 4.0*p2 - p3) * t * t +\n")
        f.write("    (-p0 + 3.0*p1 - 3.0*p2 + p3) * t * t * t\n")
        f.write(");\n\n")

        # Catmull-Rom interpolation for M_end
        f.write("// Separable Catmull-Rom interpolation lookup for M_end\n")
        f.write(f"ja_lookup_m_end_{prefix}(m, h) = result\n")
        f.write("with {\n")
        f.write(f"    m_n = max(0.0, min(1.0, ja_lut_{prefix}_m_norm(m)));\n")
        f.write(f"    h_n = max(0.0, min(1.0, ja_lut_{prefix}_h_norm(h)));\n")
        f.write("    \n")
        f.write(f"    m_scaled = m_n * (ja_lut_{prefix}_m_size - 1);\n")
        f.write(f"    h_scaled = h_n * (ja_lut_{prefix}_h_size - 1);\n")
        f.write("    \n")
        f.write("    m_idx = int(floor(m_scaled));\n")
        f.write("    h_idx = int(floor(h_scaled));\n")
        f.write("    \n")
        f.write("    m_frac = m_scaled - float(m_idx);\n")
        f.write("    h_frac = h_scaled - float(h_idx);\n")
        f.write("    \n")
        f.write("    // Clamp indices for 4x4 Catmull-Rom\n")
        f.write(f"    m0 = max(0, m_idx - 1);\n")
        f.write(f"    m1 = max(0, min(m_idx, ja_lut_{prefix}_m_size - 1));\n")
        f.write(f"    m2 = max(0, min(m_idx + 1, ja_lut_{prefix}_m_size - 1));\n")
        f.write(f"    m3 = min(m_idx + 2, ja_lut_{prefix}_m_size - 1);\n")
        f.write("    \n")
        f.write(f"    h0 = max(0, h_idx - 1);\n")
        f.write(f"    h1 = max(0, min(h_idx, ja_lut_{prefix}_h_size - 1));\n")
        f.write(f"    h2 = max(0, min(h_idx + 1, ja_lut_{prefix}_h_size - 1));\n")
        f.write(f"    h3 = min(h_idx + 2, ja_lut_{prefix}_h_size - 1);\n")
        f.write("    \n")
        f.write("    // Fetch 16 points (4x4 grid)\n")
        for mi in range(4):
            for hi in range(4):
                f.write(f"    v{mi}{hi} = ja_lut_{prefix}_m_end, ja_lut_{prefix}_idx(m{mi}, h{hi}) : rdtable;\n")
        f.write("    \n")
        f.write("    // Interpolate 4 columns along H axis\n")
        for mi in range(4):
            f.write(f"    col{mi} = ja_catmull_rom_{prefix}(v{mi}0, v{mi}1, v{mi}2, v{mi}3, h_frac);\n")
        f.write("    \n")
        f.write("    // Interpolate along M axis\n")
        f.write(f"    result = ja_catmull_rom_{prefix}(col0, col1, col2, col3, m_frac);\n")
        f.write("};\n\n")

        # Catmull-Rom interpolation for sumM_rest
        f.write("// Separable Catmull-Rom interpolation lookup for sumM_rest\n")
        f.write(f"ja_lookup_sum_m_rest_{prefix}(m, h) = result\n")
        f.write("with {\n")
        f.write(f"    m_n = max(0.0, min(1.0, ja_lut_{prefix}_m_norm(m)));\n")
        f.write(f"    h_n = max(0.0, min(1.0, ja_lut_{prefix}_h_norm(h)));\n")
        f.write("    \n")
        f.write(f"    m_scaled = m_n * (ja_lut_{prefix}_m_size - 1);\n")
        f.write(f"    h_scaled = h_n * (ja_lut_{prefix}_h_size - 1);\n")
        f.write("    \n")
        f.write("    m_idx = int(floor(m_scaled));\n")
        f.write("    h_idx = int(floor(h_scaled));\n")
        f.write("    \n")
        f.write("    m_frac = m_scaled - float(m_idx);\n")
        f.write("    h_frac = h_scaled - float(h_idx);\n")
        f.write("    \n")
        f.write("    // Clamp indices for 4x4 Catmull-Rom\n")
        f.write(f"    m0 = max(0, m_idx - 1);\n")
        f.write(f"    m1 = max(0, min(m_idx, ja_lut_{prefix}_m_size - 1));\n")
        f.write(f"    m2 = max(0, min(m_idx + 1, ja_lut_{prefix}_m_size - 1));\n")
        f.write(f"    m3 = min(m_idx + 2, ja_lut_{prefix}_m_size - 1);\n")
        f.write("    \n")
        f.write(f"    h0 = max(0, h_idx - 1);\n")
        f.write(f"    h1 = max(0, min(h_idx, ja_lut_{prefix}_h_size - 1));\n")
        f.write(f"    h2 = max(0, min(h_idx + 1, ja_lut_{prefix}_h_size - 1));\n")
        f.write(f"    h3 = min(h_idx + 2, ja_lut_{prefix}_h_size - 1);\n")
        f.write("    \n")
        f.write("    // Fetch 16 points (4x4 grid)\n")
        for mi in range(4):
            for hi in range(4):
                f.write(f"    v{mi}{hi} = ja_lut_{prefix}_sum_m_rest, ja_lut_{prefix}_idx(m{mi}, h{hi}) : rdtable;\n")
        f.write("    \n")
        f.write("    // Interpolate 4 columns along H axis\n")
        for mi in range(4):
            f.write(f"    col{mi} = ja_catmull_rom_{prefix}(v{mi}0, v{mi}1, v{mi}2, v{mi}3, h_frac);\n")
        f.write("    \n")
        f.write("    // Interpolate along M axis\n")
        f.write(f"    result = ja_catmull_rom_{prefix}(col0, col1, col2, col3, m_frac);\n")
        f.write("};\n")

    print(f"  Exported FAUST library: {output_path}")


def generate_single_lut(name: str, phase_span: float, total_substeps: int,
                        physics: PhysicsParams, args, output_dir: Path):
    """Generate a single LUT with given parameters"""

    m_grid, h_grid, lut_M_end, lut_sumM_rest = generate_2d_lut(
        name=name,
        phase_span=phase_span,
        total_substeps=total_substeps,
        physics=physics,
        bias_level=args.bias_level,
        bias_scale=args.bias_scale,
        m_size=args.m_size,
        h_size=args.h_size,
        h_range=tuple(args.h_range),
        real_substeps=args.real_substeps,
        use_rk4=args.use_rk4
    )

    # Export files
    cpp_path = output_dir / f"JAHysteresisLUT_{name}.h"
    faust_path = output_dir / f"ja_lut_{name.lower()}.lib"

    export_cpp_header(m_grid, h_grid, lut_M_end, lut_sumM_rest, name,
                      total_substeps, args.real_substeps, cpp_path)
    export_faust_lib(m_grid, h_grid, lut_M_end, lut_sumM_rest, name,
                     total_substeps, args.real_substeps, phase_span, faust_path)

    print(f"  M_end range: [{lut_M_end.min():.6f}, {lut_M_end.max():.6f}]")
    print(f"  sumM_rest range: [{lut_sumM_rest.min():.6f}, {lut_sumM_rest.max():.6f}]")
    print(f"  Memory: {lut_M_end.nbytes * 2 / 1024:.1f} KB")

    # Validate if requested
    if args.validate:
        validate_lut(
            m_grid, h_grid, lut_M_end, lut_sumM_rest,
            phase_span, total_substeps, physics,
            args.bias_level, args.bias_scale,
            args.real_substeps, args.use_rk4
        )

    return m_grid, h_grid, lut_M_end, lut_sumM_rest


def main():
    parser = argparse.ArgumentParser(
        description='Generate JA Hysteresis 2D LUT with RK4 integration and configurable real substeps',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic generation with defaults (1 real substep, Euler)
  python generate_ja_lut.py --mode K121

  # 10% real substeps with RK4 (recommended)
  python generate_ja_lut.py --mode K121 --real-substeps 12 --use-rk4

  # Generate variants (K120, K121, K122) with validation
  python generate_ja_lut.py --mode K121 --real-substeps 12 --use-rk4 --variants --validate

  # High quality: 20% real substeps
  python generate_ja_lut.py --mode K121 --real-substeps 24 --use-rk4 --validate
"""
    )

    parser.add_argument('--mode', choices=list(MODES.keys()), default='K121',
                        help='Bias mode (default: K121)')
    parser.add_argument('--variants', action='store_true',
                        help='Generate N-1, N, N+1 variants (same phase span, different substeps)')
    parser.add_argument('--m-size', type=int, default=65,
                        help='M grid size (default: 65)')
    parser.add_argument('--h-size', type=int, default=129,
                        help='H grid size (default: 129)')
    parser.add_argument('--h-range', type=float, nargs=2, default=[-1.0, 1.0],
                        help='H audio range (default: -1.0 1.0)')
    parser.add_argument('--bias-level', type=float, default=0.41,
                        help='Bias level (default: 0.41)')
    parser.add_argument('--bias-scale', type=float, default=11.0,
                        help='Bias scale (default: 11.0)')
    parser.add_argument('--output-dir', type=Path, default=Path('.'),
                        help='Output directory (default: current)')

    # New options
    parser.add_argument('--real-substeps', type=int, default=1,
                        help='Number of substeps computed in real-time before LUT lookup (default: 1). '
                             'Higher = more "alive" sound but more CPU. 10%% of total is a good starting point.')
    parser.add_argument('--use-rk4', action='store_true',
                        help='Use RK4 integration instead of Euler (more accurate, recommended)')
    parser.add_argument('--validate', action='store_true',
                        help='Run validation comparing LUT against full physics computation')

    args = parser.parse_args()

    mode = MODES[args.mode]
    physics = PhysicsParams()

    # Validate real_substeps
    if args.real_substeps < 1:
        args.real_substeps = 1
        print("Warning: real-substeps must be >= 1, setting to 1")
    if args.real_substeps >= mode.total_substeps:
        args.real_substeps = mode.total_substeps - 1
        print(f"Warning: real-substeps must be < total substeps, setting to {args.real_substeps}")

    print(f"\n{'='*60}")
    print(f"JA Hysteresis LUT Generator")
    print(f"{'='*60}")
    print(f"Mode: {mode.name} ({mode.total_substeps} total substeps)")
    print(f"Phase span: {mode.phase_span:.4f} rad ({mode.phase_span/np.pi:.2f}π)")
    print(f"Real substeps: {args.real_substeps} ({100*args.real_substeps/mode.total_substeps:.1f}% of total)")
    print(f"Integration: {'RK4' if args.use_rk4 else 'Euler'}")
    print(f"Physics: Ms={physics.Ms}, a={physics.a_density}, k={physics.k_pinning}, c={physics.c_reversibility}, α={physics.alpha_coupling}")
    print(f"Grid: M[{args.m_size}] x H[{args.h_size}]")
    print(f"H range: [{args.h_range[0]}, {args.h_range[1]}]")
    print(f"Bias: level={args.bias_level}, scale={args.bias_scale}")
    print(f"Validation: {'Yes' if args.validate else 'No'}")

    if args.variants:
        print(f"\nVariant mode: Generating N-1, N, N+1")

    # Create output directory
    args.output_dir.mkdir(parents=True, exist_ok=True)

    if args.variants:
        variants = mode.get_variants()
        for variant in variants:
            generate_single_lut(
                name=variant.name,
                phase_span=variant.phase_span,
                total_substeps=variant.total_substeps,
                physics=physics,
                args=args,
                output_dir=args.output_dir
            )
        print(f"\n{'='*60}")
        print(f"Generated {len(variants)} variants:")
        for v in variants:
            pct = 100 * args.real_substeps / v.total_substeps
            print(f"  {v.name}: {v.total_substeps} substeps, {args.real_substeps} real ({pct:.1f}%)")
    else:
        generate_single_lut(
            name=mode.name,
            phase_span=mode.phase_span,
            total_substeps=mode.total_substeps,
            physics=physics,
            args=args,
            output_dir=args.output_dir
        )

    print(f"\n{'='*60}")
    print("Done!")
    print(f"{'='*60}\n")


if __name__ == '__main__':
    main()
