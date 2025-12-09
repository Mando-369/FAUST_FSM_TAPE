#!/bin/bash
# Rebuild FAUST plugin without changing plugin IDs
# This preserves the .jucer file so Live recognizes the plugin

set -e
cd "$(dirname "$0")"

# Faust version selection
echo "Which FAUST compiler?"
echo "  [1] Normal (system faust)"
echo "  [2] Dev fork (ondemand primitive)"
read -p "Choice [1/2]: " FAUST_CHOICE

if [ "$FAUST_CHOICE" = "2" ]; then
    FAUST_DEV_DIR="$(dirname "$0")/../tools/faust-ondemand"
    if [ ! -f "$FAUST_DEV_DIR/build/bin/faust" ]; then
        echo "ERROR: Dev fork not found at $FAUST_DEV_DIR"
        echo "Clone it: git clone -b master-dev-ocpp-od-fir-2-FIR13 https://github.com/grame-cncm/faust.git tools/faust-ondemand"
        exit 1
    fi
    FAUST_CMD="$FAUST_DEV_DIR/build/bin/faust"
    JUCE_ARCH="$FAUST_DEV_DIR/architecture/juce/juce-plugin.cpp"
    export FAUSTLIB="$FAUST_DEV_DIR/share/faust"
    echo "Using dev fork: $FAUST_CMD"
else
    FAUST_CMD="faust"
    JUCE_ARCH="/opt/homebrew/share/faust/juce/juce-plugin.cpp"
    echo "Using system faust"
fi

# DSP file selection
echo ""
echo "Which DSP file?"
echo "  [1] jahysteresislib_proto.dsp (K96 single mode)"
echo "  [2] jahysteresislib_proto_OD_4_modes.dsp (ondemand 4 modes)"
read -p "Choice [1/2]: " DSP_CHOICE

if [ "$DSP_CHOICE" = "2" ]; then
    DSP_FILE="dev/lib_latest_proto/jahysteresislib_proto_OD_4_modes.dsp"
    PROJECT_DIR="jahysteresislib_proto_OD_4_modes"
else
    DSP_FILE="dev/lib_latest_proto/jahysteresislib_proto.dsp"
    PROJECT_DIR="jahysteresislib_proto"
fi
echo "Using: $DSP_FILE"
echo "Project: $PROJECT_DIR"

JUCER_FILE="$PROJECT_DIR/$PROJECT_DIR.jucer"
PROJUCER="$(dirname "$0")/../JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer"

# Check if project exists
if [ -d "$PROJECT_DIR" ] && [ -f "$JUCER_FILE" ]; then
    echo "=== Regenerating C++ only (preserving .jucer) ==="

    # Generate new C++ code using faust directly with JUCE architecture
    "$FAUST_CMD" -double -a "$JUCE_ARCH" -cn mydsp "$DSP_FILE" -o "$PROJECT_DIR/FaustPluginProcessor.cpp"

    # Add missing FAUST_INPUTS/OUTPUTS defines (stereo)
    sed -i '' '/#define FAUSTCLASS mydsp/a\
#define FAUST_INPUTS 2\
#define FAUST_OUTPUTS 2
' "$PROJECT_DIR/FaustPluginProcessor.cpp"

    echo "✓ C++ regenerated"

    # Resave and rebuild
    "$PROJUCER" --resave "$JUCER_FILE"
    echo "✓ Xcode project updated"

    cd "$PROJECT_DIR/Builds/MacOSX"

    # Build AU
    xcodebuild -project "$PROJECT_DIR.xcodeproj" -scheme "$PROJECT_DIR - AU" -configuration Release build | tail -3
    COMPONENT_SRC="build/Release/$PROJECT_DIR.component"
    COMPONENT_DST="$HOME/Library/Audio/Plug-Ins/Components/$PROJECT_DIR.component"
    rm -rf "$COMPONENT_DST"
    cp -R "$COMPONENT_SRC" "$COMPONENT_DST"
    echo "✓ AU installed to Components"

    # Build VST3
    xcodebuild -project "$PROJECT_DIR.xcodeproj" -scheme "$PROJECT_DIR - VST3" -configuration Release build | tail -3
    VST3_SRC="build/Release/$PROJECT_DIR.vst3"
    VST3_DST="$HOME/Library/Audio/Plug-Ins/VST3/$PROJECT_DIR.vst3"
    rm -rf "$VST3_DST"
    cp -R "$VST3_SRC" "$VST3_DST"
    echo "✓ VST3 installed to VST3"

    echo "=== Done ==="
else
    echo "=== Project doesn't exist, creating fresh ==="
    # Note: faust2juce creates output dir based on .dsp filename, we move it after
    faust2juce -jucemodulesdir ../../JUCE/modules -osc -double "$DSP_FILE"
    # Move generated folder from dev/lib_latest_proto/ to faust/ root if needed
    [ -d "dev/lib_latest_proto/jahysteresislib_proto" ] && mv dev/lib_latest_proto/jahysteresislib_proto .
    echo "✓ Project created"
    echo "IMPORTANT: Note the plugin IDs in .jucer for future reference!"
    grep -E "pluginCode|bundleIdentifier" "$JUCER_FILE"
fi
