#!/bin/bash
# Build script for 4D tabulateNd with machine presets
# Uses ondemand primitive from faust-ondemand dev fork
#
# 4 presets × 4D LUT (96k points each) = ~6 MB
# K90 × 4 cascades = 360 substeps (matches 3D quality)
# Expected compile: ~30s

set -e

cd "$(dirname "$0")"
PROJECT_ROOT="$(cd ../.. && pwd)"

DSP="ja_tabulateNd_4D_presets.dsp"
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

echo "=== Building $NAME (4D LUT + 4 machine presets via ondemand) ==="
echo "Presets: A800 (warm) | A810 (clean) | ATR-102 (hifi) | MX5050 (gritty)"
echo "4D grid: 33×65×9×5 = 96,525 points × 4 presets"
echo "This may take 30-60 seconds to compile..."

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
