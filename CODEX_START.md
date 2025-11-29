# CODEX_START.md

Quick boot reference for Codex sessions on `FAUST_FSM_TAPE`. Check this file first whenever a new command arrives.

## 0. Mandatory context refresh
1. `CLAUDE.md` — project story, LUT breakthrough, current intents.
2. `docs/CURRENT_STATUS.md` — live blockers, research, ownership.
3. Scan user prompt for focus + environment overrides.

## 1. Repository map (must-know)
- `faust/` — shipping library + LUT banks.
- `faust/dev/ja_streaming_bias_proto.dsp` — truth reference.
- `juce_plugin/Source/` — C++ scheduler baseline (11% CPU).
- `scripts/generate_ja_lut.py` — 2D LUT builder (bias level 0.41, scale 11).
- `docs/` — LUT restructure plan, status, phase-locked oscillator notes.

## 2. Working assumptions
- Physics params fixed: Ms=320, a=720, k=280, c=0.18, α=0.015.
- Bias amp fixed (0.41 * 11) for LUT compatibility.
- 10 bias modes (K28..K1920) share same interface; CPU hit because `ba.if` evaluates all.
- Use `apply_patch` for textual edits, keep ASCII, no destructive git commands.

## 3. Typical workflows
- **FAUST iteration**: edit `faust/jahysteresis.lib` → (optionally) `cd faust && ./rebuild_faust.sh`.
- **LUT regen**: `cd scripts && python3 generate_ja_lut.py --mode KXXX --bias-level 0.41 --output-dir ../faust`.
- **Analysis**: leverage `docs/FSM-PHL-SRD-BIAS-OSC.md`, Plugin Doctor notes, spectral data if provided.

## 4. Default investigation steps
1. Read prompt, note user’s active files.
2. Skim relevant docs (status, bias research, LUT plans).
3. Inspect FAUST/C++ counterparts for diffs.
4. Only then propose edits/tests.

## 5. Reporting style
- Lead with findings/analysis; reference files with line numbers.
- Summaries concise; suggest next actions if obvious (tests, commits, listening).
- Mention if something couldn’t be run (tests/builds).

Use this as the grounding checklist before each task to avoid re-asking for context.
