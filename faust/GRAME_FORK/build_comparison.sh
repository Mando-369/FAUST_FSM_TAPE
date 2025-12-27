#!/bin/bash
# Build TUDI_Limiter with both compilers for performance comparison
# Tests whether ondemand fork overhead is tabulateNd-specific or general

set -e
cd "$(dirname "$0")"

JUCE_MODULES="$HOME/Dev/JUCE_Projects/FAUST_FSM_TAPE/JUCE/modules"
FORK_PATH="$HOME/Dev/JUCE_Projects/FAUST_FSM_TAPE/tools/faust-ondemand"
PROJUCER="$HOME/JUCE/Projucer.app/Contents/MacOS/Projucer"

echo "=== Generating TUDI_Limiter_official with official FAUST ==="
/opt/homebrew/bin/faust2juce -jucemodulesdir "$JUCE_MODULES" \
  TUDI_Limiter_official.dsp

echo ""
echo "=== Generating TUDI_Limiter_ondemand with ondemand fork ==="
export PATH="$FORK_PATH/build/bin:$PATH"
export FAUSTLIB="$FORK_PATH/share/faust"
export FAUSTINC="$FORK_PATH/architecture"
export FAUSTARCH="$FORK_PATH/architecture"

"$FORK_PATH/tools/faust2appls/faust2juce" -jucemodulesdir "$JUCE_MODULES" \
  TUDI_Limiter_ondemand.dsp

echo ""
echo "=== Building official with Projucer + xcodebuild ==="
"$PROJUCER" --resave TUDI_Limiter_official/TUDI_Limiter_official.jucer
xcodebuild -project TUDI_Limiter_official/Builds/MacOSX/TUDI_Limiter_official.xcodeproj \
  -configuration Release -target "TUDI_Limiter_official - AU" -quiet

echo ""
echo "=== Building ondemand with Projucer + xcodebuild ==="
"$PROJUCER" --resave TUDI_Limiter_ondemand/TUDI_Limiter_ondemand.jucer
xcodebuild -project TUDI_Limiter_ondemand/Builds/MacOSX/TUDI_Limiter_ondemand.xcodeproj \
  -configuration Release -target "TUDI_Limiter_ondemand - AU" -quiet

echo ""
echo "=== Done ==="
echo "Plugins installed to ~/Library/Audio/Plug-Ins/Components/"
echo ""
echo "Test in Reaper:"
echo "  - TUDI_Limiter_official (built with official FAUST)"
echo "  - TUDI_Limiter_ondemand (built with ondemand fork)"
echo ""
echo "Compare CPU % in FX window bottom left"
