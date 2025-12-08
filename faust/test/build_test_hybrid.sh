#!/bin/bash
# Build hybrid test plugin (AU + VST3) with standard faust
set -e
cd "$(dirname "$0")"

DSP_FILE="test_mode3_bias_asym_hybrid.dsp"
PROJECT_DIR="test_mode3_bias_asym_hybrid"
JUCER_FILE="$PROJECT_DIR/test_mode3_bias_asym_hybrid.jucer"
JUCE_ARCH="/opt/homebrew/share/faust/juce/juce-plugin.cpp"
PROJUCER="$(dirname "$0")/../../JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer"

if [ -d "$PROJECT_DIR" ] && [ -f "$JUCER_FILE" ]; then
    echo "=== Regenerating C++ only (preserving .jucer) ==="
    faust -double -a "$JUCE_ARCH" -cn mydsp "$DSP_FILE" -o "$PROJECT_DIR/FaustPluginProcessor.cpp"
    sed -i '' '/#define FAUSTCLASS mydsp/a\
#define FAUST_INPUTS 2\
#define FAUST_OUTPUTS 2
' "$PROJECT_DIR/FaustPluginProcessor.cpp"
    echo "C++ regenerated"

    "$PROJUCER" --resave "$JUCER_FILE"
    echo "Xcode project updated"

    cd "$PROJECT_DIR/Builds/MacOSX"

    # Build AU
    xcodebuild -project test_mode3_bias_asym_hybrid.xcodeproj -scheme "test_mode3_bias_asym_hybrid - AU" -configuration Release build | tail -3
    COMPONENT_SRC="build/Release/test_mode3_bias_asym_hybrid.component"
    COMPONENT_DST="$HOME/Library/Audio/Plug-Ins/Components/test_mode3_bias_asym_hybrid.component"
    rm -rf "$COMPONENT_DST"
    cp -R "$COMPONENT_SRC" "$COMPONENT_DST"
    echo "AU installed"

    # Build VST3
    xcodebuild -project test_mode3_bias_asym_hybrid.xcodeproj -scheme "test_mode3_bias_asym_hybrid - VST3" -configuration Release build | tail -3
    VST3_SRC="build/Release/test_mode3_bias_asym_hybrid.vst3"
    VST3_DST="$HOME/Library/Audio/Plug-Ins/VST3/test_mode3_bias_asym_hybrid.vst3"
    rm -rf "$VST3_DST"
    cp -R "$VST3_SRC" "$VST3_DST"
    echo "VST3 installed"

    echo "=== Done ==="
else
    echo "=== Creating project ==="
    faust2juce -jucemodulesdir ../../JUCE/modules -osc -double "$DSP_FILE"
    # Fix JUCE module paths (faust2juce uses wrong relative depth)
    sed -i '' 's|path="../../JUCE/modules"|path="../../../JUCE/modules"|g' "$JUCER_FILE"
    echo "Project created - run script again to build"
fi
