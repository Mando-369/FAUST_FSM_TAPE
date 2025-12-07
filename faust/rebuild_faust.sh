#!/bin/bash
# Rebuild FAUST plugin without changing plugin IDs
# This preserves the .jucer file so Live recognizes the plugin

set -e
cd "$(dirname "$0")"

DSP_FILE="dev/jahysteresislib_proto.dsp"
PROJECT_DIR="jahysteresislib_proto"
JUCER_FILE="$PROJECT_DIR/jahysteresislib_proto.jucer"
JUCE_ARCH="/opt/homebrew/share/faust/juce/juce-plugin.cpp"
PROJUCER="$(dirname "$0")/../JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer"

# Check if project exists
if [ -d "$PROJECT_DIR" ] && [ -f "$JUCER_FILE" ]; then
    echo "=== Regenerating C++ only (preserving .jucer) ==="

    # Generate new C++ code using faust directly with JUCE architecture
    faust -double -a "$JUCE_ARCH" -cn mydsp "$DSP_FILE" -o "$PROJECT_DIR/FaustPluginProcessor.cpp"

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
    xcodebuild -project jahysteresislib_proto.xcodeproj -scheme "jahysteresislib_proto - AU" -configuration Release build | tail -3
    COMPONENT_SRC="build/Release/jahysteresislib_proto.component"
    COMPONENT_DST="$HOME/Library/Audio/Plug-Ins/Components/jahysteresislib_proto.component"
    rm -rf "$COMPONENT_DST"
    cp -R "$COMPONENT_SRC" "$COMPONENT_DST"
    echo "✓ AU installed to Components"

    # Build VST3
    xcodebuild -project jahysteresislib_proto.xcodeproj -scheme "jahysteresislib_proto - VST3" -configuration Release build | tail -3
    VST3_SRC="build/Release/jahysteresislib_proto.vst3"
    VST3_DST="$HOME/Library/Audio/Plug-Ins/VST3/jahysteresislib_proto.vst3"
    rm -rf "$VST3_DST"
    cp -R "$VST3_SRC" "$VST3_DST"
    echo "✓ VST3 installed to VST3"

    echo "=== Done ==="
else
    echo "=== Project doesn't exist, creating fresh ==="
    # Note: faust2juce creates output dir based on .dsp filename, we move it after
    faust2juce -jucemodulesdir ../../JUCE/modules -osc -double "$DSP_FILE"
    # Move generated folder from dev/ to faust/ root if needed
    [ -d "dev/jahysteresislib_proto" ] && mv dev/jahysteresislib_proto .
    echo "✓ Project created"
    echo "IMPORTANT: Note the plugin IDs in .jucer for future reference!"
    grep -E "pluginCode|bundleIdentifier" "$JUCER_FILE"
fi
