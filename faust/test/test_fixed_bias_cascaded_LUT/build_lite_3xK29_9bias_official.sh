#!/bin/bash
set -e
cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../../.. && pwd)"

DSP="jahysteresis_lite_3xK29_9bias.dsp"
NAME="${DSP%.dsp}"

# Use official homebrew faust
FAUST2JUCE="/opt/homebrew/bin/faust2juce"

echo "=== Building $NAME with official faust (9 presets - expect ~4min) ==="
time $FAUST2JUCE -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" "$DSP"
"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" --resave "$NAME/$NAME.jucer"

xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" -configuration Release -quiet build

echo "✓ AU built"
