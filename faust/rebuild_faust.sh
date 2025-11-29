#!/bin/bash
# Rebuild FAUST plugin without changing plugin IDs
# This preserves the .jucer file so Live recognizes the plugin

set -e
cd "$(dirname "$0")"

DSP_FILE="ja_streaming_bias_proto.dsp"
PROJECT_DIR="ja_streaming_bias_proto"
JUCER_FILE="$PROJECT_DIR/ja_streaming_bias_proto.jucer"
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
    xcodebuild -project ja_streaming_bias_proto.xcodeproj -scheme "ja_streaming_bias_proto - AU" -configuration Release build | tail -3

    # Install to Components folder (remove old first)
    COMPONENT_SRC="build/Release/ja_streaming_bias_proto.component"
    COMPONENT_DST="$HOME/Library/Audio/Plug-Ins/Components/ja_streaming_bias_proto.component"
    rm -rf "$COMPONENT_DST"
    cp -R "$COMPONENT_SRC" "$COMPONENT_DST"
    echo "✓ Installed to Components"

    echo "=== Done ==="
else
    echo "=== Project doesn't exist, creating fresh ==="
    faust2juce -jucemodulesdir ../../JUCE/modules -osc -double "$DSP_FILE"
    echo "✓ Project created"
    echo "IMPORTANT: Note the plugin IDs in .jucer for future reference!"
    grep -E "pluginCode|bundleIdentifier" "$JUCER_FILE"
fi
