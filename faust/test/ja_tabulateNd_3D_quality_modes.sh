#!/bin/bash
# Build script for 3D tabulateNd with quality modes
# Tests if ondemand bug affects 3D LUTs (not just 4D)
#
# 3 quality modes × 3D LUT (19k points each) = ~460k points total
# Expected: CPU should change based on mode selection
# Bug: CPU stays constant (all modes compute)

set -e

cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../.. && pwd)"

DSP="ja_tabulateNd_3D_quality_modes.dsp"
NAME="${DSP%.dsp}"

# Use dev fork for ondemand primitive
FAUST_DEV="$PROJECT_ROOT/tools/faust-ondemand"
if [ ! -f "$FAUST_DEV/build/bin/faust" ]; then
  echo "ERROR: faust-ondemand dev fork not found at $FAUST_DEV"
  echo "Install with:"
  echo "  git clone -b master-dev-ocpp-od-fir-2-FIR13 https://github.com/grame-cncm/faust.git $FAUST_DEV"
  echo "  cd $FAUST_DEV/build && make"
  exit 1
fi

echo "=== Building $NAME (3D LUT + 3 quality modes via ondemand) ==="
echo "Modes: K23 Eco | K45 Standard | K90 HQ"
echo "3D grid: 33×65×9 = 19,305 points × 3 modes × 2 LUTs = 115k table entries"
echo "This may take 60-120 seconds to compile..."

# Set up faust-ondemand environment
export PATH="$FAUST_DEV/build/bin:$PATH"
export FAUSTARCH="$FAUST_DEV/architecture"
export FAUSTLIB="$FAUST_DEV/share/faust"
export FAUSTINC="$FAUST_DEV/architecture"

# Clean previous build
rm -rf "$NAME"

# Compile with faust2juce from dev fork
time "$FAUST_DEV/tools/faust2appls/faust2juce" -double -t 600 \
  -jucemodulesdir "$PROJECT_ROOT/JUCE/modules" \
  "$DSP"

# Generate Xcode project
"$PROJECT_ROOT/JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer" \
  --resave "$NAME/$NAME.jucer"

# Build AU
xcodebuild -project "$NAME/Builds/MacOSX/$NAME.xcodeproj" \
  -scheme "$NAME - AU" -configuration Release -quiet build

# Install
AU_SRC="$NAME/Builds/MacOSX/build/Release/$NAME.component"
AU_DST="$HOME/Library/Audio/Plug-Ins/Components/$NAME.component"
rm -rf "$AU_DST"
cp -R "$AU_SRC" "$AU_DST"

echo ""
echo "Done! Installed to: $AU_DST"
echo "Restart your DAW to load the plugin."
echo ""
echo "Test procedure:"
echo "1. Load plugin in DAW"
echo "2. Note CPU usage at K45 Standard (default)"
echo "3. Switch to K23 Eco - CPU should DROP"
echo "4. Switch to K90 HQ - CPU should INCREASE"
echo "5. If CPU stays constant across all modes = BUG CONFIRMED for 3D"
