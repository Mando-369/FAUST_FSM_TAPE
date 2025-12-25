# // ===== Core JA substep (parameterized by substep_phase) =====

A 100 kHz bias carrier “inside” a 48 kHz simulation is expensive because the only way to make the hysteresis state *see* that fast excursion is exactly what you’re doing: local substeps (i.e., local oversampling).  WDF can help you embed the hysteresis into a larger circuit in a stable way, but it won’t magically give you a free high-rate magnetic integrator at 48 kHz.[^15_1][^15_2]

## Cheaper than substepping: average the bias

If the audible goal is “bias linearizes the magnetization / reduces distortion” rather than “phase-accurate 100 kHz waveform,” then it’s often enough to replace the explicit carrier with an **averaged (cycle-integrated) anhysteretic curve** inside your JA update.[^15_1]

Concretely, instead of stepping
$H(t)=H_\text{audio}+B\sin(\phi(t))$
many times, compute your anhysteretic magnetization using a small quadrature over one bias cycle:

- Replace `Man_e = ja_tanh(He * inv_a_norm)` with
`Man_e = mean_k( ja_tanh((He + bias_amp * bias_offset_k) * inv_a_norm) )`
- Use 4 or 8 fixed phases for `bias_offset_k` (e.g., 4-point: $\sin(\pi/4),\sin(3\pi/4),\sin(5\pi/4),\sin(7\pi/4)$).
- Then run **one** JA update per audio sample using that “bias-averaged” `Man_e`.

This removes the whole `phase/substep_phase_92/wrap_2pi` loop and turns “N substeps per sample” into “K tanh evaluations per sample,” where K can be as low as 4.

If you want your K92 asymmetry term, just bake it into the offsets:
`bias_offset_k = sin(theta_k) + bias_asym_s * sin(2*theta_k)` (same K points).

## If substepping must stay: make it adaptive

If the model really needs explicit substeps (e.g., you’re intentionally chasing bias/audio intermod sidebands), then the best CPU win is to vary the number of internal steps based on how fast the state is changing (|dH|, proximity to saturation/pinning blow-up, etc.). Work exists on variable step-size / step-size control for WDF systems specifically aimed at changing the effective step while preserving the network variables/energy consistency. [^15_2] Even if you don’t implement the full method, the guiding idea applies: spend substeps only when the internal state demands it.

## Where WDF can still help your JA library

WDF won’t replace JA, but it can make JA **meaningfully “loadable”** by putting it in the magnetizing branch of a transformer/inductor equivalent circuit so the rest of the circuit (source impedance, secondary load, feedback paths) changes the hysteresis operating point naturally.  That’s also the direction taken in published “inductor/transformer with Jiles–Atherton magnetization” audio work, i.e., JA as the magnetic law, embedded in a circuit formulation.[^15_3][^15_4]

If the target is tape/head rather than transformer: is the objective mainly bias “linearization” (THD drop/softening) or explicitly reproducing bias-related intermod components? That one choice determines whether the bias-averaging trick is acceptable.
<span style="display:none">[^15_10][^15_11][^15_12][^15_13][^15_14][^15_15][^15_16][^15_17][^15_18][^15_19][^15_20][^15_21][^15_22][^15_5][^15_6][^15_7][^15_8][^15_9]</span>

<div align="center">⁂</div>

[^15_1]: https://www.dafx.de/paper-archive/2019/DAFx2019_paper_3.pdf

[^15_2]: https://dafx.de/paper-archive/2017/papers/DAFx17_paper_74.pdf

[^15_3]: https://www.hsu-hh.de/ant/wp-content/uploads/sites/699/2017/10/Holters_jamodel_DAFx16.pdf

[^15_4]: https://faustlibraries.grame.fr/libs/wdmodels/

[^15_5]: https://dafx.de/paper-archive/details/Z5Ow_E5YHVxvndTAyeEuzA

[^15_6]: https://www.reddit.com/r/audioengineering/comments/p5p6j3/psa_information_is_not_lost_when_converting_an/

[^15_7]: https://ntrs.nasa.gov/api/citations/19710001859/downloads/19710001859.pdf

[^15_8]: https://www.analog.com/media/en/technical-documentation/user-guides/ug-311.pdf

[^15_9]: https://aimspress.com/aimspress-data/math/2024/11/PDF/math-09-11-1517.pdf

[^15_10]: https://www.diyaudio.com/community/threads/tapedeck-bias-frequency-circuitry.227093/

[^15_11]: https://www.facebook.com/TheMusicTelegraph/posts/in-analog-tape-recording-bias-frequency-is-a-high-frequency-signal-added-to-the-/1418314880296620/

[^15_12]: https://www.mixonline.com/recording/analog-tape-101-part-3-bias-magic-373029

[^15_13]: https://gearspace.com/board/electronic-music-instruments-and-electronic-music-production/1420742-analog-drum-machines-vs-roland-acb-emulations.html

[^15_14]: https://ieeexplore.ieee.org/iel8/6287639/10820123/11224892.pdf

[^15_15]: https://www.facebook.com/groups/2262752904/posts/10158352513747905/

[^15_16]: https://github.com/jatinchowdhury18/AnalogTapeModel/wiki/User-Manual

[^15_17]: https://www.sciencedirect.com/science/article/abs/pii/S0304885302014634

[^15_18]: https://hydrogenaudio.org/index.php/topic,96560.0.html

[^15_19]: https://en.wikipedia.org/wiki/Jiles–Atherton_model

[^15_20]: https://www.tapeheads.net/threads/bias-signal-quality-bias-freq-vs-record-quality.16153/

[^15_21]: https://opus4.kobv.de/opus4-fhws/frontdoor/deliver/index/docId/257/file/Biondic_Comparison_Ferromagnetic_Coil_Hysteresis_Models.pdf

[^15_22]: https://gearspace.com/board/mastering-forum/390532-tape-flutter-compensation.html


---

# Why not create a LUT and and then call table at sample rate ?

Yes—using a LUT is a very reasonable way to cut CPU, but it only helps for the parts of your JA update that are “pure function of one (or few) variables,” not for the stateful recursion itself.[^16_1][^16_2]

## What a LUT can accelerate

In your substep, the expensive bit is usually the anhysteretic curve eval (`ja_tanh(x_man)`) and possibly anything that looks like “nonlinear mapping of x.”  That part is perfect for a LUT: precompute `tanh(x)` (or your `ja_tanh`) over a bounded range of `x_man`, then do linear/cubic interpolation per sample.[^16_2]

In Faust you can do this cleanly with the `ba.tabulateNd` helpers which explicitly exist to tabulate an expensive function and then evaluate it with `.val`, `.lin`, or `.cub` interpolation.  For plain wavetable-style reads, Faust also has `rdtable`/`frdtable` style mechanisms used in its oscillators library, which is the same general idea (table + fractional index + interpolation).[^16_3][^16_1][^16_2]

## What a LUT won’t solve

A LUT won’t remove the need for substeps if you truly need the hysteresis state to evolve at bias timescale, because the costly part then becomes “doing the whole update N times,” not just the tanh call.  If you keep 32–128 substeps per sample, even a very cheap LUT still multiplies cost by that factor.[^16_4]

## A practical hybrid

If the goal is “keep substeps but make each one cheaper,” LUT the `ja_tanh` (and optionally `d/dx tanh` too, since you compute `dMan_dH` from `1 - tanh^2`).  If the goal is “stop paying for substeps,” pair the LUT with a bias-averaged/implicit update so you do one update per sample rather than N updates.[^16_2][^16_4]

If you tell the typical range of `x_man = He * inv_a_norm` you see in practice (min/max), it’s possible to suggest a good table size and whether `.lin` is enough or you need `.cub`.
<span style="display:none">[^16_10][^16_11][^16_12][^16_13][^16_14][^16_15][^16_16][^16_17][^16_18][^16_19][^16_20][^16_21][^16_5][^16_6][^16_7][^16_8][^16_9]</span>

<div align="center">⁂</div>

[^16_1]: https://faustlibraries.grame.fr/libs/oscillators/

[^16_2]: https://faustlibraries.grame.fr/libs/basics/

[^16_3]: https://faustlibraries.grame.fr/libs/interpolators/

[^16_4]: https://www.dafx.de/paper-archive/2019/DAFx2019_paper_3.pdf

[^16_5]: https://faustdoc.grame.fr/manual/syntax/

[^16_6]: https://www.dafx.de/paper-archive/2006/papers/p_169.pdf

[^16_7]: http://msp.ucsd.edu/techniques/v0.04/book-html/node30.html

[^16_8]: https://www.metafunction.co.uk/post/the-wavetable-synthesis-architecture

[^16_9]: https://www.gwfnet.net/Metadata/Record/T-2022-12-05-q1y9x6hohe0CHLBTUgvGcVw

[^16_10]: https://www.scribd.com/document/284392873/Faust-Tutorial2

[^16_11]: https://www.ti.com/lit/SCLA071

[^16_12]: https://stackoverflow.com/questions/60336862/faust-table-lookup-performance

[^16_13]: https://faust.readthedocs.io/en/latest/reference/faust.tables.html

[^16_14]: https://www.embeddedrelated.com/showcode/324.php

[^16_15]: https://zenodo.org/records/6769595

[^16_16]: https://faustdoc.grame.fr/manual/optimizing/

[^16_17]: https://www.sciencedirect.com/science/article/abs/pii/S1477842411000029

[^16_18]: https://github.com/grame-cncm/faustlibraries/blob/master/instruments.lib

[^16_19]: https://dl.acm.org/doi/10.1145/3064899.3064902

[^16_20]: https://scm.cri.mines-paristech.fr/git/Faustine.git/blob/df77f6467561ab164f95d774f6890fbc7c0a3309:/interpretor/faust-0.9.47mr3/documentation/Faust_tutorial2.pdf

[^16_21]: https://www.semanticscholar.org/paper/Direct-Function-Evaluation-versus-Lookup-Tables:-to-Green-Bohn/19c84426d33552e7b926a2eafc0cae7d9bdfd3db


---

# That’s exactly what I’m aiming for. ONE update per sample. As if there is a stroboscope running at 48khz in order to represent the 100khz wave. But that’s ONE flash/Sample at SR and not millions like I’m doing now.

A “stroboscope at 48 kHz” can’t represent a real 100 kHz sine in discrete time without it turning into an alias (a different low frequency) at the sample points.  So one update per sample can only work if the goal is *not* to reconstruct the carrier, but to capture the **cycle-averaged effect** that the carrier has on the hysteresis (i.e., a two-time-scale/averaging approximation).[^17_1][^17_2][^17_3]

## What to do instead (one update/sample)

Make your JA update see a *bias-averaged* anhysteretic curve (and ideally its slope), so you don’t need phase/substeps at all.[^17_1]

The core idea is: replace the explicit $H_\text{audio} + B\sin(\phi)$ stepping with an average over one bias period:

- \$ \overline{M}_{an}(H_e, B) = \frac{1}{2\pi}\int_0^{2\pi} M_{an}(H_e + B\sin\theta)\,d\theta \$[^17_1]
- Likewise for the slope you use in your update: \$ \overline{dM_{an}/dH}(H_e, B) \$ (because after averaging, $1-\overline{M}^2$ is generally **not** the correct derivative).[^17_1]

Then you do exactly **one** JA step per sample using:

- `Man_e := ManAvg(He, bias_amp)`
- `dMan_dH := dManAvg(He, bias_amp)`
instead of computing them from instantaneous `sin(midpoint)`.


## Make it fast in Faust (2D LUT)

This is where Faust is strong: precompute the averaged functions as a LUT and read them at audio rate.

- Use `ba.tabulateNd` to tabulate an expensive multi-parameter function and access it with linear/cubic interpolation.[^17_4]
- Build 2 tables (or one table returning a pair): one for `ManAvg(He, B)` and one for `dManAvg(He, B)`.[^17_4]
- If `bias_amp` is fixed or changes slowly, it can even collapse to a 1D LUT in `He`.[^17_4]

`ba.tabulateNd(...).lin` / `.cub` is explicitly designed for “expensive function with known ranges,” including wavetable-style use cases.[^17_4]

## What this buys you (and what it won’t)

This approach can get you “one update per sample” and remove the brutal substep loop, because the bias enters as a pre-averaged modification of the magnetization law.  What it won’t do is reproduce bias/audio intermod sidebands that depend on the instantaneous phase of the bias carrier, because phase has been averaged out on purpose.[^17_1]

If you tell whether you need those bias intermod products (audible “whistles/sidebands”) or mainly the bias-driven linearization/deadzone behavior, it’s possible to pick the right averaging (full-cycle average vs a small quadrature that keeps some asymmetry like your `sin(2θ)` term).
<span style="display:none">[^17_10][^17_11][^17_12][^17_13][^17_14][^17_15][^17_16][^17_17][^17_18][^17_19][^17_20][^17_5][^17_6][^17_7][^17_8][^17_9]</span>

<div align="center">⁂</div>

[^17_1]: https://www.dafx.de/paper-archive/2019/DAFx2019_paper_3.pdf

[^17_2]: https://www.ni.com/docs/en-US/bundle/ni-rfsg/page/sampling-nyquist-shannon.html

[^17_3]: https://en.wikipedia.org/wiki/Nyquist–Shannon_sampling_theorem

[^17_4]: https://faustlibraries.grame.fr/libs/basics/

[^17_5]: https://dafx.de/paper-archive/details/Z5Ow_E5YHVxvndTAyeEuzA

[^17_6]: https://www.facebook.com/groups/heyaudiostudent/posts/24568716892756469/

[^17_7]: https://aaltodoc.aalto.fi/bitstream/handle/123456789/117331/master_Kaloinen_Jussi_2022.pdf?sequence=1\&isAllowed=y

[^17_8]: https://gearspace.com/board/so-much-gear-so-little-time/1383184-analog-tape-recording-process-demystified-21.html

[^17_9]: https://home.strw.leidenuniv.nl/~por/AOT2019/docs/AOT_2019_Ex13_NyquistTheorem.pdf

[^17_10]: https://www.pulsarmodular.com/wp-content/uploads/2025/03/P821-MDN-Tape-User-Guide_V1.0.0_EN_Print.pdf

[^17_11]: https://github.com/grame-cncm/faustlibraries/blob/master/basics.lib

[^17_12]: https://www.nature.com/articles/s41586-025-09501-1

[^17_13]: https://data.code.gouv.fr/hosts/GitHub/repositories/grame-cncm%2Ffaustlibraries?path=basics.lib

[^17_14]: https://u-he.com/products/satin/

[^17_15]: https://faustlibraries.grame.fr/libs/

[^17_16]: https://resources.pcb.cadence.com/blog/2023-nyquist-sampling-theorem-conserving-signal-characteristics

[^17_17]: https://github.com/jatinchowdhury18/AnalogTapeModel/wiki/User-Manual

[^17_18]: https://faustlibraries.grame.fr

[^17_19]: https://mres.uni-potsdam.de/2021/12/29/vinyls-compact-discs-and-the-nyquist-frequency/

[^17_20]: https://www.openwarelab.org/Faust/


---

# But that’s an issue! Bias is a user parameter and not a fixed value! I already did a workaround a lite version where LUT is precomputed at 9 steps of bias and then called at runtime. But that’s ok but not highend

A user-controlled bias parameter is fine—just don’t think of it as “9 separate LUTs”; treat it as a **2D LUT** where one axis is $H_e$ (or your `x_man`) and the other axis is `bias_amp`. Faust already has exactly this use-case covered with `ba.tabulateNd`, which is the N‑dimensional version of `tabulate` and supports `.lin` and `.cub` interpolation *in N dimensions* at runtime.[^18_1]

## What to tabulate (so you can do 1 update/sample)

To get your “one JA update per sample” idea working without substeps, tabulate the bias-averaged pieces you currently compute inside each substep:

- `ManAvg(He, B)` = cycle-averaged anhysteretic magnetization under bias.
- `dManAvg_dH(He, B)` = the cycle-averaged slope you need (don’t derive it from `1 - ManAvg^2`; average the derivative consistently if you want stability/accuracy).

Then your per-sample update uses `He = H_audio + alpha_norm*M_prev` (your existing line) and reads these two values from the table.[^18_1]

## Why this is “high-end” vs 9 steps

Your “9 bias steps” approach is basically a coarse 2D LUT with manual bias interpolation. `ba.tabulateNd(...).lin` is the same idea but generalized: you pick table sizes and ranges per dimension, and Faust performs the multi-dimensional interpolation for you.  If you need higher quality, bump the bias-axis resolution (e.g., 33/65 points) and use `.cub` instead of `.lin`.[^18_1]

## Practical details that matter

- Bias is a user parameter, but it’s still typically slow-moving compared to audio; even linear interpolation on the bias axis is usually perceptually smooth if the bias table has enough points.[^18_1]
- Clamp ranges: `tabulateNd` can optionally clamp inputs to the defined ranges (the `C` parameter), which is useful when users crank bias beyond your assumed limits.[^18_1]

If you share your intended ranges for `He` (or `x_man`) and bias (min/max), it’s possible to suggest concrete table sizes that won’t blow CPU or memory.
<span style="display:none">[^18_10][^18_11][^18_12][^18_13][^18_14][^18_15][^18_16][^18_17][^18_18][^18_19][^18_2][^18_20][^18_3][^18_4][^18_5][^18_6][^18_7][^18_8][^18_9]</span>

<div align="center">⁂</div>

[^18_1]: https://faustlibraries.grame.fr/libs/basics/

[^18_2]: https://data.code.gouv.fr/hosts/GitHub/repositories/grame-cncm%2Ffaustlibraries?path=basics.lib

[^18_3]: https://faustlibraries.grame.fr/libs/interpolators/

[^18_4]: https://sourceforge.net/p/faudiostream/mailman/message/37197269/

[^18_5]: https://www.netlib.org/math/docpdf/ch12-02.pdf

[^18_6]: https://faustgrp.gitlabpages.inria.fr/faust/last-doc/html/FAQ.html

[^18_7]: https://github.com/grame-cncm/faustlibraries/blob/master/basics.lib

[^18_8]: https://faustlibraries.grame.fr/libs/

[^18_9]: https://faust.readthedocs.io/en/latest/reference/faust.tables.table.html

[^18_10]: https://faustlibraries.grame.fr/organization/

[^18_11]: https://stackoverflow.com/questions/67805754/fast-multidimensional-interpolation-of-unstructured-data

[^18_12]: https://github.com/DBraun/Faust-Tutorial/blob/main/utils.lib

[^18_13]: https://www.openwarelab.org/Faust/

[^18_14]: https://www.sciencedirect.com/science/article/pii/S0885064X09000041

[^18_15]: https://cran.r-universe.dev/FastCUB/doc/manual.html

[^18_16]: https://ijc8.me/faustdoc/manual/syntax/

[^18_17]: https://github.com/PriorLabs/TabPFN/issues/246

[^18_18]: https://ijc8.me/faustdoc/manual/quick-start/

[^18_19]: https://www.reddit.com/r/reinforcementlearning/comments/13ie8xh/automatic_hyperparameter_tuning_a_visual_guide/

[^18_20]: https://github.com/grame-cncm/faustlibraries/blob/master/maths.lib


---

# This is what I have so far LUT wise

// 3xK29 hysteresis for each bias preset
ja_hysteresis_01(H) = (loop ~ _) : (!, _)
with {
loop(recM) = M_end, Mavg with {
M1 = ja_lookup_m_end_k29_bias_01(recM, H); s1 = ja_lookup_sum_m_rest_k29_bias_01(recM, H);
M2 = ja_lookup_m_end_k29_bias_01(M1, H);   s2 = ja_lookup_sum_m_rest_k29_bias_01(M1, H);
M_end = ja_lookup_m_end_k29_bias_01(M2, H); s3 = ja_lookup_sum_m_rest_k29_bias_01(M2, H);
Mavg = (s1+s2+s3) * inv_n;
};
};

What you have is already the right *structure* for “one update per sample”: an iterative implicit solve (`M1 -> M2 -> M_end`) plus an averaged quantity. The part that’s holding you back is that you’ve discretized bias as “preset N”, which forces you into multiple LUT sets.[^19_1]

## Upgrade it to a 2D LUT (H, bias)

Instead of separate functions like `ja_lookup_m_end_k29_bias_01(...)`, make a single lookup that takes `(M_prev, H, bias)` and returns the values you need:

- `M_end = LUT_Mend(M_prev, H, bias)`
- `s = LUT_Sum(M_prev, H, bias)`

In Faust, `ba.tabulateNd` is designed for exactly “tabulate an expensive function then evaluate it with interpolation,” including multi-dimensional tables with `.lin` or `.cub`.  That means you can keep your 3-iteration fixed-point solver exactly as-is, but the LUT becomes continuous over bias instead of “bias_01/bias_02/…”.[^19_1]

## Keep your solver, just swap the LUT calls

Your current code:

- `M1 = lookup(recM, H)`
- `M2 = lookup(M1, H)`
- `M_end = lookup(M2, H)`
- `Mavg = (s1+s2+s3) * inv_n`

still works if `lookup(...)` becomes `lookup(..., bias)`.

So you’d end up with:

- `M1 = LUT_Mend(recM, H, bias)`
- `M2 = LUT_Mend(M1, H, bias)`
- `M_end = LUT_Mend(M2, H, bias)`
- `Mavg` from `LUT_Sum(…, bias)` the same way

This is “high-end” because bias becomes smoothly variable with proper interpolation rather than jumping between presets.[^19_1]

## One more important detail

If bias is a UI parameter, it will move slowly; you can typically low-pass/smooth it before feeding the LUT to prevent zippering without touching the audio-rate loop.[^19_1]

If you tell how many total bias presets you currently have and what ranges you assume for `H` and `M_prev`, it’s possible to suggest an approximate 3D table resolution that stays memory-safe but improves over 9 steps substantially.

<div align="center">⁂</div>

[^19_1]: https://faustlibraries.grame.fr/libs/basics/


---

# So how would you change this script in py scripts/generate_ja_lut.py

To make bias a real-time user parameter (not “9 presets”), change your generator from a 2D LUT to a **3D LUT**: $(M_{in}, H_{audio}, bias)\rightarrow(M_{end}, \sum M_{rest})$, and do trilinear (or tricubic) interpolation in Faust. Faust’s own `ba.tabulateNd` explicitly supports N‑D tabulation + N‑D linear/cubic interpolation, which is the same conceptual upgrade you’re implementing manually with waveforms.[^20_1]

## Core Python changes (2D → 3D)

### 1) Add a bias axis to LUT generation

Replace `generate_2d_lut(...)` with something like `generate_3d_lut(...)`:

```py
def generate_3d_lut(
    name: str,
    phase_span: float,
    total_substeps: int,
    physics: PhysicsParams,
    bias_scale: float = 11.0,
    m_size: int = 65,
    h_size: int = 129,
    b_size: int = 33,
    h_range: Tuple[float, float] = (-1.0, 1.0),
    bias_range: Tuple[float, float] = (0.0, 1.0),
    real_substeps: int = 0,
):
    bias_lut = generate_bias_lut(phase_span, total_substeps)
    consts = get_derived_constants(physics)

    m_grid = np.linspace(-1.0, 1.0, m_size)
    h_grid = np.linspace(h_range[^20_0], h_range[^20_1], h_size)
    b_grid = np.linspace(bias_range[^20_0], bias_range[^20_1], b_size)

    lut_M_end = np.zeros((m_size, h_size, b_size), dtype=np.float32)
    lut_sumM_rest = np.zeros((m_size, h_size, b_size), dtype=np.float32)

    for i, M_in in enumerate(m_grid):
        for j, H_audio in enumerate(h_grid):
            for k, bias_level in enumerate(b_grid):
                bias_amplitude = bias_level * bias_scale
                M_end, sumM_rest = compute_remainder_response(
                    M_in, H_audio, bias_lut, bias_amplitude, consts,
                    start_substep=real_substeps
                )
                lut_M_end[i, j, k] = M_end
                lut_sumM_rest[i, j, k] = sumM_rest

    return m_grid, h_grid, b_grid, lut_M_end, lut_sumM_rest
```

Notes:

- If you truly want “ONE update per sample” with no substeps computed at runtime, set `real_substeps = 0` so the LUT represents the entire bias cycle remainder.


### 2) Add CLI args

Add:

- `--bias-size` (e.g. 33 or 65)
- `--bias-range 0.0 1.0` (or your meaningful UI range)
and pass them into `generate_3d_lut`.


### 3) Update validation interpolation (bilinear → trilinear)

Your validation currently does bilinear. Extend it to 3D with 8 corners. (Keep it linear in validation even if you export bicubic/tricubic for Faust.)

## Export changes (C++ and Faust)

### 1) Flatten 3D consistently

Right now you do `lut.flatten()` for 2D. For 3D you’ll flatten in `(m,h,b)` order (NumPy default is C-order; last index varies fastest), and your index becomes:

```faust
idx(mi, hi, bi) = (mi * H_SIZE + hi) * B_SIZE + bi;
```


### 2) Change Faust lookup signature to include bias

You currently generate `ja_lookup_m_end_prefix(m,h)`. Make it:

- `ja_lookup_m_end_prefix(m,h,b)`
- `ja_lookup_sum_m_rest_prefix(m,h,b)`


### 3) Trilinear interpolation in Faust (fast + “high-end enough”)

You’ll fetch 8 points instead of 4 (2D bilinear) or 16 (2D bicubic). Trilinear is usually the sweet spot: continuous control, low CPU.

Pseudocode structure:

- Normalize `m,h,b` to [0..1]
- Scale to grid, compute `mi,hi,bi` and fractions `mt,ht,bt`
- Fetch 8 corners `p000..p111`
- Interpolate:
    - along bias: `p00 = lerp(bt, p000, p001)`, `p01 = lerp(bt, p010, p011)`, `p10 = lerp(bt, p100, p101)`, `p11 = lerp(bt, p110, p111)`
    - along H: `p0 = lerp(ht, p00, p01)`, `p1 = lerp(ht, p10, p11)`
    - along M: `result = lerp(mt, p0, p1)`


## Practical sizing guidance

- Try `b_size = 33` first; it’s already a big step up from “9 presets” while staying memory/CPU sane.
- Consider storing `float32` in C++ (`np.float32`) to cut size by 2×.
- If bias is a UI knob, smooth it before lookup (even a tiny 5–20 ms smoothing) so you can keep bias resolution moderate without zipper artifacts.

If the intended runtime is “3 fixed-point iterations” like your current Faust loop, the 3D LUT approach drops in cleanly: just thread `bias` into each lookup call, no other structural change needed.
<span style="display:none">[^20_10][^20_2][^20_3][^20_4][^20_5][^20_6][^20_7][^20_8][^20_9]</span>

<div align="center">⁂</div>

[^20_1]: https://faustlibraries.grame.fr/libs/basics/

[^20_2]: https://data.code.gouv.fr/hosts/GitHub/repositories/grame-cncm%2Ffaustlibraries?path=basics.lib

[^20_3]: https://faustlibraries.grame.fr/libs/interpolators/

[^20_4]: https://faustcloud.grame.fr/doc/libraries/index.html

[^20_5]: https://github.com/grame-cncm/faustlibraries/blob/master/maths.lib

[^20_6]: https://github.com/grame-cncm/faustlibraries

[^20_7]: https://gist.github.com/plampite/0857fdc5efcb4986da56f56205326059

[^20_8]: https://www.reddit.com/r/cpp/comments/2fzp41/multivariate_splines_opensource_c_library_for/

[^20_9]: https://faustgrp.gitlabpages.inria.fr/faust/last-doc/html/namespacemembers_func.html

[^20_10]: https://github.com/cmbant/forutils/blob/master/Interpolation.f90

