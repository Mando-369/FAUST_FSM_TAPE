#!/bin/bash
set -e
cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../../.. && pwd)"

DSP="jahysteresis_lite_3xK29_9bias.dsp"
NAME="${DSP%.dsp}"

echo "=== Building $NAME (9 bias presets - ~3 min compile) ==="
time /opt/homebrew/bin/faust2juce -t 600 -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" "$DSP"
"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" --resave "$NAME/$NAME.jucer"

xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" -configuration Release -quiet build

echo "✓ AU built"
