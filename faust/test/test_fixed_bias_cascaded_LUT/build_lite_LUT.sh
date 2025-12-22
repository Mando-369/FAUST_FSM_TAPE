#!/bin/bash
# Build script for jahysteresis_lite_proto_OD_3_bias_LUT
# Uses faust-ondemand for the ondemand primitive

set -e
cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../../.. && pwd)"

DSP="jahysteresis_lite_proto_OD_3_bias_LUT.dsp"
NAME="${DSP%.dsp}"

# Use faust-ondemand - must set PATH first so faust2juce uses correct faust
export PATH="$PROJECT_ROOT/tools/faust-ondemand/build/bin:$PATH"
export FAUSTARCH="$PROJECT_ROOT/tools/faust-ondemand/architecture"
export FAUSTLIB="$PROJECT_ROOT/tools/faust-ondemand/share/faust"
export FAUSTINC="$PROJECT_ROOT/tools/faust-ondemand/architecture"
FAUST2JUCE="$PROJECT_ROOT/tools/faust-ondemand/tools/faust2appls/faust2juce"

echo "=== Building $NAME ==="

# Generate JUCE project
echo "Generating JUCE project..."
$FAUST2JUCE -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" "$DSP"

# Update Xcode project
echo "Updating Xcode project..."
"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" --resave "$NAME/$NAME.jucer"

# Build AU
echo "=== Building AU ==="
xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" \
  -configuration Release \
  -quiet \
  build

echo "✓ AU built"

# Build VST3
echo "=== Building VST3 ==="
xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - VST3" \
  -configuration Release \
  -quiet \
  build

echo "✓ VST3 built"
echo "=== Done ==="
