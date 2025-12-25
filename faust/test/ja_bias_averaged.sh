#!/bin/bash
set -e
cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../.. && pwd)"

DSP="ja_bias_averaged.dsp"
NAME="${DSP%.dsp}"

echo "=== Building $NAME (bias-averaged JA, 4 mini-substeps, 4-point quadrature) ==="
time /opt/homebrew/bin/faust2juce -double -t 120 -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" "$DSP"
"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" --resave "$NAME/$NAME.jucer"

xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" -configuration Release -quiet build

echo "Done: AU built"
