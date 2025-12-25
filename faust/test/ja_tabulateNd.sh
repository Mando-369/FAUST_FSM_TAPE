#!/bin/bash
set -e
cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../.. && pwd)"

DSP="ja_tabulateNd.dsp"
NAME="${DSP%.dsp}"

echo "=== Building $NAME (3D LUT via ba.tabulateNd, 33×65×9 grid) ==="
echo "Table size: $(( 33 * 65 * 9 * 2 )) entries (M_end + sum_rest)"
time /opt/homebrew/bin/faust2juce -double -t 600 -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" "$DSP"
"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" --resave "$NAME/$NAME.jucer"

xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" -configuration Release -quiet build

echo "Done: AU built"
