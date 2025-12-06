#!/usr/bin/env python3
"""
Generate 2D Lookup Table for JA Hysteresis Optimization

This script precomputes the JA hysteresis response for substeps 1..N-1,
allowing the audio loop to compute only substep 0 (cross-sample dependency)
and look up the remainder.

The LUT maps (M_in, HAudio) -> (M_end, sumM_rest)

Usage:
    python generate_ja_lut.py [--mode K60] [--output-dir ../faust]
"""

import numpy as np
import argparse
from pathlib import Path
from typing import Tuple, NamedTuple


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
        This changes the phase step size (dphi = phase_span / substeps).
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
# This ensures opposite bias polarity between adjacent samples
MODES = {
    # Low-res group (18 steps/cycle) - lofi character
    'K28': ModeConfig('K28', 1.5, 18),      # 27 substeps (ultra lofi)
    'K45': ModeConfig('K45', 2.5, 18),      # 45 substeps (lofi)
    'K63': ModeConfig('K63', 3.5, 18),      # 63 substeps (vintage)
    # High-res group (22 steps/cycle) - cleaner character
    'K99': ModeConfig('K99', 4.5, 22),      # 99 substeps (warm)
    'K121': ModeConfig('K121', 5.5, 22),    # 121 substeps (standard)
    'K187': ModeConfig('K187', 8.5, 22),    # 187 substeps (high quality)
    'K253': ModeConfig('K253', 11.5, 22),   # 253 substeps (detailed)
    'K495': ModeConfig('K495', 22.5, 22),   # 495 substeps (ultra detailed)
    'K1045': ModeConfig('K1045', 47.5, 22), # 1045 substeps (extreme)
    'K2101': ModeConfig('K2101', 95.5, 22), # 2101 substeps (beyond physical)
}


def fast_tanh(x: np.ndarray) -> np.ndarray:
    """Real tanh (matching upgraded FAUST implementation)"""
    return np.tanh(x)


def generate_bias_lut(phase_span: float, total_substeps: int) -> np.ndarray:
    """Generate bias sin() values for all substeps (midpoint sampling)"""
    n = total_substeps
    dphi = phase_span / n
    # Midpoint sampling: sin((i + 0.5) * dphi)
    indices = np.arange(n)
    return np.sin((indices + 0.5) * dphi)


def ja_substep(
    M_prev: float,
    H_prev: float,
    H_audio: float,
    bias_offset: float,
    bias_amplitude: float,
    physics: PhysicsParams
) -> Tuple[float, float]:
    """
    Execute one JA substep. Returns (M_new, H_new).

    This is the exact same physics as C++ executeSubstep().
    """
    # Derived constants
    Ms_safe = max(physics.Ms, 1e-6)
    alpha_norm = physics.alpha_coupling
    a_norm = physics.a_density / Ms_safe
    inv_a_norm = 1.0 / max(a_norm, 1e-9)
    k_norm = physics.k_pinning / Ms_safe
    c_norm = physics.c_reversibility

    # JA physics
    H_new = H_audio + bias_amplitude * bias_offset
    dH = H_new - H_prev
    He = H_new + alpha_norm * M_prev

    x_man = He * inv_a_norm
    Man_e = float(fast_tanh(np.array([x_man]))[0])
    Man_e2 = Man_e * Man_e
    dMan_dH = (1.0 - Man_e2) * inv_a_norm

    direction = 1.0 if dH >= 0.0 else -1.0
    pin = direction * k_norm - alpha_norm * (Man_e - M_prev)
    inv_pin = 1.0 / (pin + 1e-6)

    denom = 1.0 - c_norm * alpha_norm * dMan_dH
    inv_denom = 1.0 / (denom + 1e-9)
    dMdH = (c_norm * dMan_dH + (Man_e - M_prev) * inv_pin) * inv_denom
    dM_step = dMdH * dH

    M_new = np.clip(M_prev + dM_step, -1.0, 1.0)

    return M_new, H_new


def compute_remainder_response(
    M1: float,
    H_audio: float,
    bias_lut: np.ndarray,
    bias_amplitude: float,
    physics: PhysicsParams
) -> Tuple[float, float]:
    """
    Compute substeps 1..N-1 given the state after substep 0.

    Returns (M_end, sumM_rest) where:
    - M_end: final magnetization after all substeps
    - sumM_rest: sum of magnetizations from substeps 1..N-1
    """
    n = len(bias_lut)

    # Initialize with post-substep-0 state
    M = M1
    H = H_audio + bias_amplitude * bias_lut[0]  # H after substep 0

    sum_M = 0.0

    # Run substeps 1 to N-1
    for i in range(1, n):
        M, H = ja_substep(M, H, H_audio, bias_lut[i], bias_amplitude, physics)
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
    h_range: Tuple[float, float] = (-1.0, 1.0)
) -> Tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """
    Generate the 2D LUT for (M_in, HAudio) -> (M_end, sumM_rest).

    Returns:
        m_grid: M axis values
        h_grid: H axis values
        lut_M_end: 2D array of M_end values
        lut_sumM_rest: 2D array of sumM_rest values
    """
    bias_amplitude = bias_level * bias_scale
    bias_lut = generate_bias_lut(phase_span, total_substeps)

    # Create grids
    m_grid = np.linspace(-1.0, 1.0, m_size)
    h_grid = np.linspace(h_range[0], h_range[1], h_size)

    # Initialize output arrays
    lut_M_end = np.zeros((m_size, h_size))
    lut_sumM_rest = np.zeros((m_size, h_size))

    total_points = m_size * h_size
    count = 0

    print(f"Generating LUT for {name}: {m_size}x{h_size} = {total_points} points")
    print(f"Phase span: {phase_span:.4f} rad ({phase_span/np.pi:.2f}π)")
    print(f"Bias amplitude: {bias_amplitude:.3f}")
    print(f"Substeps: {total_substeps} (computing 1..{total_substeps-1})")

    for i, M_in in enumerate(m_grid):
        for j, H_audio in enumerate(h_grid):
            # M_in represents M1 (magnetization after substep 0)
            M_end, sumM_rest = compute_remainder_response(
                M_in, H_audio, bias_lut, bias_amplitude, physics
            )
            lut_M_end[i, j] = M_end
            lut_sumM_rest[i, j] = sumM_rest

            count += 1
            if count % 1000 == 0:
                print(f"  Progress: {count}/{total_points} ({100*count/total_points:.1f}%)")

    print(f"Done! LUT shape: {lut_M_end.shape}")

    return m_grid, h_grid, lut_M_end, lut_sumM_rest


def export_cpp_header(
    m_grid: np.ndarray,
    h_grid: np.ndarray,
    lut_M_end: np.ndarray,
    lut_sumM_rest: np.ndarray,
    name: str,
    total_substeps: int,
    output_path: Path
):
    """Export LUT as C++ header file"""
    m_size, h_size = lut_M_end.shape

    with open(output_path, 'w') as f:
        f.write(f"// Auto-generated JA Hysteresis LUT for {name}\n")
        f.write(f"// Grid: {m_size} x {h_size} = {m_size * h_size} points\n")
        f.write(f"// Substeps covered: 1..{total_substeps - 1}\n\n")

        f.write("#pragma once\n\n")
        f.write("#include <array>\n\n")
        f.write(f"namespace JAHysteresisLUT_{name} {{\n\n")

        f.write(f"constexpr int M_SIZE = {m_size};\n")
        f.write(f"constexpr int H_SIZE = {h_size};\n")
        f.write(f"constexpr double M_MIN = {m_grid[0]:.6f};\n")
        f.write(f"constexpr double M_MAX = {m_grid[-1]:.6f};\n")
        f.write(f"constexpr double H_MIN = {h_grid[0]:.6f};\n")
        f.write(f"constexpr double H_MAX = {h_grid[-1]:.6f};\n\n")

        # Flatten for 1D array storage
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

    print(f"Exported C++ header: {output_path}")


def export_faust_lib(
    m_grid: np.ndarray,
    h_grid: np.ndarray,
    lut_M_end: np.ndarray,
    lut_sumM_rest: np.ndarray,
    name: str,
    total_substeps: int,
    output_path: Path
):
    """Export LUT as FAUST library file"""
    m_size, h_size = lut_M_end.shape

    # Flatten for 1D waveform storage
    flat_M_end = lut_M_end.flatten()
    flat_sumM_rest = lut_sumM_rest.flatten()

    with open(output_path, 'w') as f:
        f.write(f"// Auto-generated JA Hysteresis LUT for {name}\n")
        f.write(f"// Grid: {m_size} x {h_size} = {m_size * h_size} points\n")
        f.write(f"// Substeps covered: 1..{total_substeps - 1}\n\n")

        f.write("import(\"stdfaust.lib\");\n\n")

        prefix = name.lower()
        f.write(f"// Grid parameters for {name}\n")
        f.write(f"ja_lut_{prefix}_m_size = {m_size};\n")
        f.write(f"ja_lut_{prefix}_h_size = {h_size};\n")
        f.write(f"ja_lut_{prefix}_m_min = {m_grid[0]:.6f};\n")
        f.write(f"ja_lut_{prefix}_m_max = {m_grid[-1]:.6f};\n")
        f.write(f"ja_lut_{prefix}_h_min = {h_grid[0]:.6f};\n")
        f.write(f"ja_lut_{prefix}_h_max = {h_grid[-1]:.6f};\n\n")

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

        # Write lookup helper functions
        f.write("// 2D index computation\n")
        f.write(f"ja_lut_{prefix}_idx(m_idx, h_idx) = m_idx * ja_lut_{prefix}_h_size + h_idx;\n\n")

        f.write("// Normalize M to [0, 1] range\n")
        f.write(f"ja_lut_{prefix}_m_norm(m) = (m - ja_lut_{prefix}_m_min) / (ja_lut_{prefix}_m_max - ja_lut_{prefix}_m_min);\n\n")

        f.write("// Normalize H to [0, 1] range\n")
        f.write(f"ja_lut_{prefix}_h_norm(h) = (h - ja_lut_{prefix}_h_min) / (ja_lut_{prefix}_h_max - ja_lut_{prefix}_h_min);\n\n")

        # Write 1D Catmull-Rom helper function
        f.write("// 1D Catmull-Rom interpolation: p0,p1,p2,p3 are 4 consecutive points, t in [0,1]\n")
        f.write(f"ja_catmull_rom_{prefix}(p0, p1, p2, p3, t) = 0.5 * (\n")
        f.write("    2.0*p1 +\n")
        f.write("    (-p0 + p2) * t +\n")
        f.write("    (2.0*p0 - 5.0*p1 + 4.0*p2 - p3) * t * t +\n")
        f.write("    (-p0 + 3.0*p1 - 3.0*p2 + p3) * t * t * t\n")
        f.write(");\n\n")

        # Write Catmull-Rom interpolation for M_end table
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
        f.write("    // Clamp indices for 4x4 Catmull-Rom (need p-1, p, p+1, p+2)\n")
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

        # Write Catmull-Rom interpolation for sumM_rest table
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

    print(f"Exported FAUST library: {output_path}")


def generate_single_lut(name: str, phase_span: float, total_substeps: int,
                        physics: PhysicsParams, args, output_dir: Path):
    """Generate a single LUT with given parameters"""
    print(f"\n--- Generating {name} ({total_substeps} substeps, phase span {phase_span/np.pi:.2f}π) ---")

    m_grid, h_grid, lut_M_end, lut_sumM_rest = generate_2d_lut(
        name=name,
        phase_span=phase_span,
        total_substeps=total_substeps,
        physics=physics,
        bias_level=args.bias_level,
        bias_scale=args.bias_scale,
        m_size=args.m_size,
        h_size=args.h_size,
        h_range=tuple(args.h_range)
    )

    cpp_path = output_dir / f"JAHysteresisLUT_{name}.h"
    faust_path = output_dir / f"ja_lut_{name.lower()}.lib"

    export_cpp_header(m_grid, h_grid, lut_M_end, lut_sumM_rest, name, total_substeps, cpp_path)
    export_faust_lib(m_grid, h_grid, lut_M_end, lut_sumM_rest, name, total_substeps, faust_path)

    print(f"  M_end range: [{lut_M_end.min():.6f}, {lut_M_end.max():.6f}]")
    print(f"  sumM_rest range: [{lut_sumM_rest.min():.6f}, {lut_sumM_rest.max():.6f}]")
    print(f"  Memory: {lut_M_end.nbytes * 2 / 1024:.1f} KB")

    return m_grid, h_grid, lut_M_end, lut_sumM_rest


def main():
    parser = argparse.ArgumentParser(description='Generate JA Hysteresis 2D LUT')
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

    args = parser.parse_args()

    mode = MODES[args.mode]
    physics = PhysicsParams()

    print(f"\n=== JA Hysteresis LUT Generator ===")
    print(f"Mode: {mode.name} (base: {mode.total_substeps} substeps)")
    print(f"Phase span: {mode.phase_span:.4f} rad ({mode.phase_span/np.pi:.2f}π)")
    print(f"Physics: Ms={physics.Ms}, a={physics.a_density}, k={physics.k_pinning}, c={physics.c_reversibility}, α={physics.alpha_coupling}")
    print(f"Grid: M[{args.m_size}] x H[{args.h_size}]")
    print(f"H range: [{args.h_range[0]}, {args.h_range[1]}]")
    print(f"Bias: level={args.bias_level}, scale={args.bias_scale}")

    if args.variants:
        print(f"\n=== VARIANT MODE: Generating N-1, N, N+1 ===")

    # Create output directory
    args.output_dir.mkdir(parents=True, exist_ok=True)

    if args.variants:
        # Generate N-1, N, N+1 variants with SAME phase span
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
        print(f"\n=== Generated {len(variants)} variants ===")
        for v in variants:
            print(f"  {v.name}: {v.total_substeps} substeps")
    else:
        # Generate single LUT (original behavior)
        generate_single_lut(
            name=mode.name,
            phase_span=mode.phase_span,
            total_substeps=mode.total_substeps,
            physics=physics,
            args=args,
            output_dir=args.output_dir
        )

    print("\nDone!")


if __name__ == '__main__':
    main()
