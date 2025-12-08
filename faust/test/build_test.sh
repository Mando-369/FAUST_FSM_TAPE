#!/bin/bash
# Build test plugin (AU + VST3) with ondemand faust fork
set -e
cd "$(dirname "$0")"

# Use ondemand fork
FAUST_OD="$(dirname "$0")/../../tools/faust-ondemand"
export PATH="$FAUST_OD/build/bin:$PATH"
export FAUSTARCH="$FAUST_OD/architecture"
export FAUSTLIB="$FAUST_OD/share/faust"
export FAUSTINC="$FAUST_OD/architecture"

DSP_FILE="test_bias_asym_substeps.dsp"
PROJECT_DIR="test_bias_asym_substeps"
JUCER_FILE="$PROJECT_DIR/test_bias_asym_substeps.jucer"
JUCE_ARCH="$FAUST_OD/architecture/juce/juce-plugin.cpp"
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
    xcodebuild -project test_bias_asym_substeps.xcodeproj -scheme "test_bias_asym_substeps - AU" -configuration Release build | tail -3
    COMPONENT_SRC="build/Release/test_bias_asym_substeps.component"
    COMPONENT_DST="$HOME/Library/Audio/Plug-Ins/Components/test_bias_asym_substeps.component"
    rm -rf "$COMPONENT_DST"
    cp -R "$COMPONENT_SRC" "$COMPONENT_DST"
    echo "AU installed"

    # Build VST3
    xcodebuild -project test_bias_asym_substeps.xcodeproj -scheme "test_bias_asym_substeps - VST3" -configuration Release build | tail -3
    VST3_SRC="build/Release/test_bias_asym_substeps.vst3"
    VST3_DST="$HOME/Library/Audio/Plug-Ins/VST3/test_bias_asym_substeps.vst3"
    rm -rf "$VST3_DST"
    cp -R "$VST3_SRC" "$VST3_DST"
    echo "VST3 installed"

    echo "=== Done ==="
else
    echo "=== Creating project ==="
    "$FAUST_OD/tools/faust2appls/faust2juce" -jucemodulesdir ../../JUCE/modules -osc -double "$DSP_FILE"
    # Fix JUCE module paths (faust2juce uses wrong relative depth)
    sed -i '' 's|path="../../JUCE/modules"|path="../../../JUCE/modules"|g' "$JUCER_FILE"
    echo "Project created - run script again to build"
fi
