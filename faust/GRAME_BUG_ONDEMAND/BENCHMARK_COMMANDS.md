# Ondemand Benchmark Commands

## Setup (run once per terminal session)

```bash
export PATH="$HOME/Dev/JUCE_Projects/FAUST_FSM_TAPE/tools/faust-ondemand/build/bin:$PATH"
export FAUSTLIB="$HOME/Dev/JUCE_Projects/FAUST_FSM_TAPE/tools/faust-ondemand/share/faust"
```

## Single mode benchmark

```bash
cd /tmp && faust -a /opt/homebrew/share/faust/minimal-bench.cpp ~/Dev/JUCE_Projects/FAUST_FSM_TAPE/faust/dev/lib_latest_proto/ja_tabulateNd.dsp -o bench_single.cpp && clang++ -O3 -ffast-math bench_single.cpp -o bench_single && ./bench_single
```

## 3-mode benchmark (with ondemand)

```bash
cd /tmp && faust -a /opt/homebrew/share/faust/minimal-bench.cpp ~/Dev/JUCE_Projects/FAUST_FSM_TAPE/faust/GRAME_BUG_ONDEMAND/ja_tabulateNd_3D_quality_modes.dsp -o bench_3mode.cpp && clang++ -O3 -ffast-math bench_3mode.cpp -o bench_3mode && ./bench_3mode
```

## Expected output

Look for lines like:
```
mydsp : XX.XX MBytes/sec (DSP CPU % : X.XX), DSP size : XXX
```

---

## faustbench Results

### M4 Max

| Test | MBytes/sec | CPU % | DSP Size |
|------|------------|-------|----------|
| ja_tabulateNd.dsp (single) | 15.6 | 4.75% | 328 |
| ja_tabulateNd_3D_quality_modes.dsp (3-mode) | 16.3 | 4.62% | 352 |

Ratio: ~1:1

### M1

| Test | MBytes/sec | CPU % | DSP Size |
|------|------------|-------|----------|
| ja_tabulateNd.dsp (single) | 10.45 | 6.58% | 328 |
| ja_tabulateNd_3D_quality_modes.dsp (3-mode) | 10.32 | 6.66% | 352 |

Ratio: ~1:1

---

## Reaper Results (M4 Max)

### OLD: Mixed Compilers (INVALID)

Built with different compilers - this was the source of confusion:
- ja_tabulateNd.dsp (official FAUST): 0.42% CPU
- ja_tabulateNd_3D_quality_modes.dsp (ondemand fork): 1.2% CPU
- Ratio: ~1:3 (misleading - compiler difference, not ondemand failure)

### NEW: Same Compiler (VALID)

Both plugins built with ondemand fork:

| Plugin | Format | Alone | With Other |
|--------|--------|-------|------------|
| ja_tabulateNd | AU | 1.2% | 0.8% |
| ja_tabulateNd_3D_quality_modes | AU | 1.2% | 0.75% |
| ja_tabulateNd | VST3 | 1.5% | 0.85% |
| ja_tabulateNd_3D_quality_modes | VST3 | 1.5% | 0.79% |

Ratio: **1:1** - ondemand works correctly

---

## Conclusion

The ondemand fork produces ~2.7x slower code than official FAUST. When comparing plugins built with the same compiler, ondemand works correctly with `ba.tabulateNd`.
