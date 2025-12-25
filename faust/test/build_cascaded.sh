#!/bin/bash
set -e
cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../.. && pwd)"

DSP="jahysteresislib_proto_cascaded.dsp"
NAME="${DSP%.dsp}"

echo "=== Building $NAME (4×K23 cascaded full physics) ==="
echo "Architecture: 4 cascades × 23 substeps = 92 total"
echo "Using dev fork with ondemand primitive"

# Use dev fork with ondemand support
export PATH="$PROJECT_ROOT/tools/faust-ondemand/build/bin:$PATH"
export FAUSTARCH="$PROJECT_ROOT/tools/faust-ondemand/architecture"
export FAUSTLIB="$PROJECT_ROOT/tools/faust-ondemand/share/faust"
export FAUSTINC="$PROJECT_ROOT/tools/faust-ondemand/architecture"

time "$PROJECT_ROOT/tools/faust-ondemand/tools/faust2appls/faust2juce" -double -t 600 -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" "$DSP"
"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" --resave "$NAME/$NAME.jucer"

xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" -configuration Release -quiet build

echo "Done: AU built"
