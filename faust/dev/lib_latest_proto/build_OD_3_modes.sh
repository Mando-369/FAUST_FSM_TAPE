#!/bin/bash
# Build jahysteresislib_proto_OD_3_modes.dsp with dev fork faust
# Plugin ID: e0a3, Bundle: com.grame.jahysteresislib_proto_OD_3_modes
set -e
cd "$(dirname "$0")"

FAUST_DEV="../../../tools/faust-ondemand"
FAUST_CMD="$FAUST_DEV/build/bin/faust"
FAUST2JUCE="$FAUST_DEV/tools/faust2appls/faust2juce"
JUCE_MODULES="../../../../JUCE/modules"
PROJECT="jahysteresislib_proto_OD_3_modes"
DSP="jahysteresislib_proto_OD_3_modes.dsp"

export FAUSTLIB="$FAUST_DEV/share/faust"
export FAUSTINC="$FAUST_DEV/architecture"
export PATH="$FAUST_DEV/build/bin:$PATH"

if [ ! -d "$PROJECT" ]; then
    echo "=== Creating project ==="
    "$FAUST2JUCE" -jucemodulesdir "$JUCE_MODULES" -double "$DSP"
    # Fix plugin ID to e0a3 (unique for this variant)
    sed -i '' 's/pluginCode="[^"]*"/pluginCode="e0a3"/' "$PROJECT/$PROJECT.jucer"
    echo "✓ Project created with pluginCode=e0a3"
fi

echo "=== Regenerating C++ ==="
"$FAUST_CMD" -double -a "$FAUST_DEV/architecture/juce/juce-plugin.cpp" -cn mydsp "$DSP" -o "$PROJECT/FaustPluginProcessor.cpp"
sed -i '' '/#define FAUSTCLASS mydsp/a\
#define FAUST_INPUTS 2\
#define FAUST_OUTPUTS 2
' "$PROJECT/FaustPluginProcessor.cpp"
echo "✓ C++ regenerated"

PROJUCER="../../../../JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer"
"$PROJUCER" --resave "$PROJECT/$PROJECT.jucer"
echo "✓ Xcode project updated"

cd "$PROJECT/Builds/MacOSX"

echo "=== Building AU ==="
xcodebuild -project "$PROJECT.xcodeproj" -scheme "$PROJECT - AU" -configuration Release build 2>&1 | tail -3
rm -rf ~/Library/Audio/Plug-Ins/Components/$PROJECT.component
cp -R "build/Release/$PROJECT.component" ~/Library/Audio/Plug-Ins/Components/
echo "✓ AU installed"

echo "=== Building VST3 ==="
xcodebuild -project "$PROJECT.xcodeproj" -scheme "$PROJECT - VST3" -configuration Release build 2>&1 | tail -3
rm -rf ~/Library/Audio/Plug-Ins/VST3/$PROJECT.vst3
cp -R "build/Release/$PROJECT.vst3" ~/Library/Audio/Plug-Ins/VST3/
echo "✓ VST3 installed"

echo "=== Done ==="
