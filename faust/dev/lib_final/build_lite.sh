#!/bin/bash
set -e
cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../../.. && pwd)"

DSP="jahysteresis_lite_demo.dsp"
NAME="${DSP%.dsp}"

# Create demo DSP that imports the library
cat > "$DSP" << 'EOF'
import("jahysteresis_lite.lib");
process = par(i, 2, fsm_channel_ui);
EOF

echo "=== Building $NAME (9 bias presets - ~3 min compile) ==="

# faust2juce with extended timeout (-t 600 passed to faust)
time /opt/homebrew/bin/faust2juce -t 600 \
  -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" "$DSP"

"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" \
  --resave "$NAME/$NAME.jucer"

xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" -configuration Release -quiet build

echo "✓ AU built"
