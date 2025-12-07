# CODEX_START.md

Quick boot reference for Codex sessions on `FAUST_FSM_TAPE`. Check this file first whenever a new command arrives.

## 0. Mandatory context refresh
1. `CLAUDE.md` — project story, LUT breakthrough, current intents.
2. `docs/CURRENT_STATUS.md` — live blockers, research, ownership.
3. Scan user prompt for focus + environment overrides.

## 1. Repository map (must-know)
- `faust/jahysteresis.lib` — shipping LUT-based library (10 modes K28..K2101).
- `faust/dev/jahysteresislib_proto.dsp` — full-physics K60 Ultra reference (72 substeps).
- `faust/dev/ja_streaming_bias_proto*.dsp` — streaming prototypes (`ba.if`, ondemand).
- `faust/test/` — LUT experiments (variable substep prototype, gated tests).
- `cpp_reference/` — legacy JA scheduler + new LUT scheduler.
- `scripts/generate_ja_lut.py` — LUT + variant generator (bias level 0.41, scale 11).
- `docs/` — STATUS, LUT restructure, variable substep plan, ondemand notes.

## 2. Working assumptions
- Physics params fixed: Ms=320, a=720, k=280, c=0.18, α=0.015.
- LUT builds use bias_level=0.41, bias_scale=11.0 (see CLAUDE table for modes).
- Full-physics FAUST prototype currently single-mode (K60 Ultra, 3 cycles × 24 substeps).
- Variable substep behavior now documented in `docs/VARIABLE_SUBSTEP_LUT_PLAN.md`.
- `ba.if` still evaluates every branch; ondemand prototype lives in `faust/dev/ja_streaming_bias_proto_OD_72.dsp`.
- Use `apply_patch` for textual edits, keep ASCII, no destructive git commands.
- New exploration: evaluate ~50/50 real substeps vs. LUT split for `jahysteresis_lite.lib` (see docs plan).

## 3. Typical workflows
- **FAUST (full physics)**: tweak `faust/dev/jahysteresislib_proto.dsp`, compare against `cpp_reference/`.
- **FAUST (LUT)**: edit `faust/jahysteresis.lib`, rebuild via `cd faust && ./rebuild_faust.sh`.
- **Generate LUT / variants**: `cd scripts && python3 generate_ja_lut.py --mode K121 --variants --bias-level 0.41 --output-dir ../faust`.
- **Test variable substep LUT**: see `faust/test/test_var_subst_lut.dsp` and related docs.
- **Analysis**: lean on Plugin Doctor captures, `docs/CURRENT_STATUS.md`, LUT restructure notes.

## 4. Default investigation steps
1. Read prompt, note user’s active files.
2. Skim relevant docs (STATUS, LUT + variable-substep plans, ondemand notes).
3. Inspect FAUST/C++ counterparts for diffs (physics params, scheduler, LUT tables).
4. Only then propose edits/tests.

## 5. Reporting style
- Lead with findings/analysis; reference files with line numbers.
- Summaries concise; suggest next actions if obvious (tests, commits, listening).
- Mention if something couldn’t be run (tests/builds).

Use this as the grounding checklist before each task to avoid re-asking for context.
