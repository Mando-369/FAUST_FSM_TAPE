#!/bin/bash
# Build script for 4D tabulateNd JA Hysteresis
# 4D LUT: M × H × Bias × Asym = 33 × 65 × 9 × 5 = 96,525 points
# Expected compile time: ~10-30s (5x more than 3D)

set -e

cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../.. && pwd)"

DSP="ja_tabulateNd_4D.dsp"
NAME="${DSP%.dsp}"

echo "=== Building $NAME (4D LUT via ba.tabulateNd) ==="
echo "4D grid: 33×65×9×5 = 96,525 points | 4×K45 = 180 substeps"
echo "This may take 10-30 seconds to compile..."

# Compile DSP to JUCE project
time /opt/homebrew/bin/faust2juce -double -t 600 \
  -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" \
  "$DSP"

# Generate Xcode project from .jucer
"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" \
  --resave "$NAME/$NAME.jucer"

# Build the AU
xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" -configuration Release -quiet build

echo ""
echo "Done! AU built at: $NAME/Builds/MacOSX/build/Release/$NAME.component"
