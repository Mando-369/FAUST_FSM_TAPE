#!/bin/bash
# Build script for jahysteresis_lite_2xK58
set -e
cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../../.. && pwd)"

DSP="jahysteresis_lite_2xK58.dsp"
NAME="${DSP%.dsp}"

export PATH="$PROJECT_ROOT/tools/faust-ondemand/build/bin:$PATH"
export FAUSTARCH="$PROJECT_ROOT/tools/faust-ondemand/architecture"
export FAUSTLIB="$PROJECT_ROOT/tools/faust-ondemand/share/faust"
export FAUSTINC="$PROJECT_ROOT/tools/faust-ondemand/architecture"
FAUST2JUCE="$PROJECT_ROOT/tools/faust-ondemand/tools/faust2appls/faust2juce"

echo "=== Building $NAME ==="

echo "Generating JUCE project..."
$FAUST2JUCE -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" "$DSP"

echo "Updating Xcode project..."
"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" --resave "$NAME/$NAME.jucer"

echo "=== Building AU ==="
xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" \
  -configuration Release \
  -quiet \
  build

echo "✓ AU built"
echo "=== Done ==="
