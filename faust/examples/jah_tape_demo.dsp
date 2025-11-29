// jah_tape_demo.dsp
// Demo: Stereo tape saturation using jahysteresis.lib
//
// Run with: faust2jaqt jah_tape_demo.dsp
// Or: faust2caqt jah_tape_demo.dsp (CoreAudio on macOS)

import("stdfaust.lib");
jah = library("jahysteresis.lib");

// Stereo tape channel with UI controls
process = par(i, 2, jah.tape_channel_ui);
