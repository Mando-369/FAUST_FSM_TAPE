/* ------------------------------------------------------------
name: "test_mode3_bias_asym"
Code generated with Faust 2.81.10 (https://faust.grame.fr)
Compilation options: -a /opt/homebrew/share/faust/juce/juce-plugin.cpp -lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -double -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2016 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 
 ************************************************************************
 ************************************************************************/

#include <algorithm>
#include <assert.h>

#if JUCE_WINDOWS
#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1
#endif

#include "JuceLibraryCode/JuceHeader.h"

#include "faust/gui/MapUI.h"
#include "faust/dsp/dsp-adapter.h"
#include "faust/gui/MidiUI.h"
#include "faust/dsp/poly-dsp.h"
#ifndef PLUGIN_MAGIC
#include "faust/gui/JuceGUI.h"
#endif
#include "faust/gui/JuceParameterUI.h"
#include "faust/gui/JuceStateUI.h"

// Always included otherwise -i mode sometimes fails...
#include "faust/gui/DecoratorUI.h"

#if defined(SOUNDFILE)
#include "faust/gui/SoundUI.h"
#endif

#if defined(OSCCTRL)
#include "faust/gui/JuceOSCUI.h"
#endif

#if defined(MIDICTRL)
#include "faust/midi/juce-midi.h"
#include "faust/dsp/timed-dsp.h"
#endif

#if defined(POLY2)
#include "faust/dsp/dsp-combiner.h"
#include "effect.h"
#endif 

// we require macro declarations
#define FAUST_UIMACROS

// but we will ignore most of them
#define FAUST_ADDBUTTON(l,f)
#define FAUST_ADDCHECKBOX(l,f)
#define FAUST_ADDSOUNDFILE(l,s)
#define FAUST_ADDVERTICALSLIDER(l,f,i,a,b,s)
#define FAUST_ADDHORIZONTALSLIDER(l,f,i,a,b,s)
#define FAUST_ADDNUMENTRY(l,f,i,a,b,s)
#define FAUST_ADDVERTICALBARGRAPH(l,f,a,b)
#define FAUST_ADDHORIZONTALBARGRAPH(l,f,a,b)


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

/* link with : "" */
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#define FAUST_INPUTS 2
#define FAUST_OUTPUTS 2
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

static double mydsp_faustpower2_f(double value) {
	return value * value;
}

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fVslider0;
	int fSampleRate;
	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	double fConst6;
	FAUSTFLOAT fVslider1;
	double fConst7;
	double fRec7[2];
	FAUSTFLOAT fVslider2;
	double fRec8[2];
	FAUSTFLOAT fVslider3;
	FAUSTFLOAT fVslider4;
	FAUSTFLOAT fVslider5;
	FAUSTFLOAT fVslider6;
	FAUSTFLOAT fVslider7;
	FAUSTFLOAT fVslider8;
	FAUSTFLOAT fVslider9;
	FAUSTFLOAT fVslider10;
	FAUSTFLOAT fVslider11;
	double fRec3[3];
	double fRec4[3];
	double fRec5[3];
	double fRec0[2];
	double fConst8;
	double fRec1[2];
	double fConst9;
	FAUSTFLOAT fVslider12;
	double fRec9[2];
	double fRec13[3];
	double fRec14[3];
	double fRec15[3];
	double fRec10[2];
	double fRec11[2];
	
 public:
	mydsp() {
	}
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "1.22.0");
		m->declare("compile_options", "-a /opt/homebrew/share/faust/juce/juce-plugin.cpp -lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -double -ftz 0");
		m->declare("filename", "test_mode3_bias_asym.dsp");
		m->declare("filters.lib/SVFTPT:author", "Dario Sanfilippo");
		m->declare("filters.lib/SVFTPT:copyright", "Copyright (C) 2024 Dario Sanfilippo <sanfilippo.dario@gmail.com>");
		m->declare("filters.lib/SVFTPT:license", "MIT License");
		m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "1.7.1");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.9.0");
		m->declare("misceffects.lib/dryWetMixer:author", "David Braun, revised by Stéphane Letz");
		m->declare("misceffects.lib/name", "Misc Effects Library");
		m->declare("misceffects.lib/version", "2.5.1");
		m->declare("name", "test_mode3_bias_asym");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "1.3.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "1.6.0");
	}

	virtual int getNumInputs() {
		return 2;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<double>(1.92e+05, std::max<double>(1.0, static_cast<double>(fSampleRate)));
		fConst1 = std::tan(31.41592653589793 / fConst0);
		fConst2 = fConst1 + 1.3513513513513513;
		fConst3 = fConst1 * fConst2 + 1.0;
		fConst4 = fConst1 / fConst3;
		fConst5 = 2.0 * fConst4;
		fConst6 = 44.1 / fConst0;
		fConst7 = 1.0 - fConst6;
		fConst8 = 2.0 * fConst1;
		fConst9 = 1.0 / fConst3;
	}
	
	virtual void instanceResetUserInterface() {
		fVslider0 = static_cast<FAUSTFLOAT>(1.0);
		fVslider1 = static_cast<FAUSTFLOAT>(0.0);
		fVslider2 = static_cast<FAUSTFLOAT>(0.0);
		fVslider3 = static_cast<FAUSTFLOAT>(0.4);
		fVslider4 = static_cast<FAUSTFLOAT>(11.0);
		fVslider5 = static_cast<FAUSTFLOAT>(0.0);
		fVslider6 = static_cast<FAUSTFLOAT>(0.18);
		fVslider7 = static_cast<FAUSTFLOAT>(7.2e+02);
		fVslider8 = static_cast<FAUSTFLOAT>(3.2e+02);
		fVslider9 = static_cast<FAUSTFLOAT>(0.015);
		fVslider10 = static_cast<FAUSTFLOAT>(1.0);
		fVslider11 = static_cast<FAUSTFLOAT>(2.8e+02);
		fVslider12 = static_cast<FAUSTFLOAT>(0.0);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			fRec7[l0] = 0.0;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			fRec8[l1] = 0.0;
		}
		for (int l2 = 0; l2 < 3; l2 = l2 + 1) {
			fRec3[l2] = 0.0;
		}
		for (int l3 = 0; l3 < 3; l3 = l3 + 1) {
			fRec4[l3] = 0.0;
		}
		for (int l4 = 0; l4 < 3; l4 = l4 + 1) {
			fRec5[l4] = 0.0;
		}
		for (int l5 = 0; l5 < 2; l5 = l5 + 1) {
			fRec0[l5] = 0.0;
		}
		for (int l6 = 0; l6 < 2; l6 = l6 + 1) {
			fRec1[l6] = 0.0;
		}
		for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
			fRec9[l7] = 0.0;
		}
		for (int l8 = 0; l8 < 3; l8 = l8 + 1) {
			fRec13[l8] = 0.0;
		}
		for (int l9 = 0; l9 < 3; l9 = l9 + 1) {
			fRec14[l9] = 0.0;
		}
		for (int l10 = 0; l10 < 3; l10 = l10 + 1) {
			fRec15[l10] = 0.0;
		}
		for (int l11 = 0; l11 < 2; l11 = l11 + 1) {
			fRec10[l11] = 0.0;
		}
		for (int l12 = 0; l12 < 2; l12 = l12 + 1) {
			fRec11[l12] = 0.0;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openHorizontalBox("JA");
		ui_interface->declare(0, "01", "");
		ui_interface->openHorizontalBox("GAIN");
		ui_interface->declare(&fVslider1, "0", "");
		ui_interface->declare(&fVslider1, "dB", "");
		ui_interface->addVerticalSlider("Input", &fVslider1, FAUSTFLOAT(0.0), FAUSTFLOAT(-24.0), FAUSTFLOAT(24.0), FAUSTFLOAT(0.1));
		ui_interface->declare(&fVslider12, "1", "");
		ui_interface->declare(&fVslider12, "dB", "");
		ui_interface->addVerticalSlider("Output", &fVslider12, FAUSTFLOAT(0.0), FAUSTFLOAT(-24.0), FAUSTFLOAT(24.0), FAUSTFLOAT(0.1));
		ui_interface->declare(&fVslider2, "2", "");
		ui_interface->declare(&fVslider2, "dB", "");
		ui_interface->addVerticalSlider("Drive", &fVslider2, FAUSTFLOAT(0.0), FAUSTFLOAT(-18.0), FAUSTFLOAT(29.0), FAUSTFLOAT(0.1));
		ui_interface->declare(&fVslider0, "3", "");
		ui_interface->addVerticalSlider("Mix", &fVslider0, FAUSTFLOAT(1.0), FAUSTFLOAT(0.0), FAUSTFLOAT(1.0), FAUSTFLOAT(0.01));
		ui_interface->closeBox();
		ui_interface->declare(0, "02", "");
		ui_interface->openHorizontalBox("BIAS");
		ui_interface->declare(&fVslider3, "0", "");
		ui_interface->addVerticalSlider("Level", &fVslider3, FAUSTFLOAT(0.4), FAUSTFLOAT(0.0), FAUSTFLOAT(1.0), FAUSTFLOAT(0.01));
		ui_interface->declare(&fVslider4, "1", "");
		ui_interface->addVerticalSlider("Scale", &fVslider4, FAUSTFLOAT(11.0), FAUSTFLOAT(1.0), FAUSTFLOAT(1e+02), FAUSTFLOAT(0.1));
		ui_interface->declare(&fVslider5, "2", "");
		ui_interface->addVerticalSlider("Asym", &fVslider5, FAUSTFLOAT(0.0), FAUSTFLOAT(0.0), FAUSTFLOAT(0.5), FAUSTFLOAT(0.01));
		ui_interface->closeBox();
		ui_interface->declare(0, "03", "");
		ui_interface->openHorizontalBox("STAB");
		ui_interface->declare(&fVslider10, "0", "");
		ui_interface->addVerticalSlider("Diff Scale", &fVslider10, FAUSTFLOAT(1.0), FAUSTFLOAT(0.0), FAUSTFLOAT(4.0), FAUSTFLOAT(0.01));
		ui_interface->closeBox();
		ui_interface->declare(0, "04", "");
		ui_interface->openHorizontalBox("PHYSICS");
		ui_interface->declare(&fVslider8, "0", "");
		ui_interface->addVerticalSlider("Ms", &fVslider8, FAUSTFLOAT(3.2e+02), FAUSTFLOAT(1e+02), FAUSTFLOAT(1e+03), FAUSTFLOAT(1.0));
		ui_interface->declare(&fVslider7, "1", "");
		ui_interface->addVerticalSlider("a", &fVslider7, FAUSTFLOAT(7.2e+02), FAUSTFLOAT(1e+02), FAUSTFLOAT(2e+03), FAUSTFLOAT(1.0));
		ui_interface->declare(&fVslider11, "2", "");
		ui_interface->addVerticalSlider("k", &fVslider11, FAUSTFLOAT(2.8e+02), FAUSTFLOAT(5e+01), FAUSTFLOAT(1e+03), FAUSTFLOAT(1.0));
		ui_interface->declare(&fVslider6, "3", "");
		ui_interface->addVerticalSlider("c", &fVslider6, FAUSTFLOAT(0.18), FAUSTFLOAT(0.0), FAUSTFLOAT(1.0), FAUSTFLOAT(0.01));
		ui_interface->declare(&fVslider9, "4", "");
		ui_interface->addVerticalSlider("alpha", &fVslider9, FAUSTFLOAT(0.015), FAUSTFLOAT(0.001), FAUSTFLOAT(0.1), FAUSTFLOAT(0.001));
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		double fSlow0 = static_cast<double>(fVslider0);
		double fSlow1 = 6.025595860743578 * fSlow0;
		double fSlow2 = fConst6 * std::pow(1e+01, 0.05 * static_cast<double>(fVslider1));
		double fSlow3 = fConst6 * std::pow(1e+01, 0.05 * static_cast<double>(fVslider2));
		double fSlow4 = static_cast<double>(fVslider3) * static_cast<double>(fVslider4);
		double fSlow5 = static_cast<double>(fVslider5);
		double fSlow6 = static_cast<double>(fVslider6);
		double fSlow7 = std::max<double>(static_cast<double>(fVslider8), 1e-06);
		double fSlow8 = std::max<double>(static_cast<double>(fVslider7) / fSlow7, 1e-09);
		double fSlow9 = fSlow6 / fSlow8;
		double fSlow10 = 1.0 / fSlow8;
		double fSlow11 = static_cast<double>(fVslider9);
		double fSlow12 = static_cast<double>(fVslider10);
		double fSlow13 = static_cast<double>(fVslider11) / fSlow7;
		double fSlow14 = fSlow6 * fSlow11 / fSlow8;
		double fSlow15 = fConst6 * std::pow(1e+01, 0.05 * static_cast<double>(fVslider12));
		double fSlow16 = fSlow4 + -4.4;
		double fSlow17 = std::pow(1e+01, 0.05 * ((fSlow4 < 4.4) ? 2.07 * fSlow16 : 1.003 * fSlow16));
		double fSlow18 = 1.0 - fSlow0;
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			double fTemp0 = static_cast<double>(input0[i0]);
			fRec7[0] = fSlow2 + fConst7 * fRec7[1];
			fRec8[0] = fSlow3 + fConst7 * fRec8[1];
			double fTemp1 = fTemp0 * fRec7[0] * fRec8[0];
			double fTemp2 = 0.15915494309189535 * (fRec5[2] + 0.1308996938995747);
			double fTemp3 = fTemp2 - std::floor(fTemp2);
			double fTemp4 = std::sin(6.283185307179586 * fTemp3) + fSlow5 * std::sin(12.566370614359172 * fTemp3);
			double fTemp5 = fTemp1 + fSlow4 * fTemp4;
			double fTemp6 = fTemp5 - fRec4[2];
			double fTemp7 = tanh(fSlow10 * (fTemp5 + fSlow11 * fRec3[2]));
			double fTemp8 = 1.0 - mydsp_faustpower2_f(fTemp7);
			double fTemp9 = fTemp7 - fRec3[2];
			double fTemp10 = fSlow12 * std::fabs(fTemp9) + 1.0;
			double fTemp11 = std::max<double>(-1.0, std::min<double>(1.0, fRec3[2] + fTemp6 * (fSlow9 * fTemp8 + fTemp9 / (fTemp10 * (fSlow13 * ((fTemp6 >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp9 / fTemp10))))) / (1.000000001 - fSlow14 * fTemp8)));
			double fTemp12 = fRec5[2] + 0.2617993877991494;
			double fTemp13 = ((fTemp12 >= 6.283185307179586) ? fRec5[2] + -6.021385919380437 : fTemp12);
			double fTemp14 = 0.15915494309189535 * (fTemp13 + 0.1308996938995747);
			double fTemp15 = fTemp14 - std::floor(fTemp14);
			double fTemp16 = std::sin(6.283185307179586 * fTemp15) + fSlow5 * std::sin(12.566370614359172 * fTemp15);
			double fTemp17 = fTemp4 - fTemp16;
			double fTemp18 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp16 + fSlow11 * fTemp11));
			double fTemp19 = 1.0 - mydsp_faustpower2_f(fTemp18);
			double fTemp20 = fTemp18 - fTemp11;
			double fTemp21 = fSlow12 * std::fabs(fTemp20) + 1.0;
			double fTemp22 = std::max<double>(-1.0, std::min<double>(1.0, fTemp11 - fSlow4 * (fTemp17 * (fSlow9 * fTemp19 + fTemp20 / (fTemp21 * (fSlow13 * ((-(fSlow4 * fTemp17) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp20 / fTemp21))))) / (1.000000001 - fSlow14 * fTemp19))));
			double fTemp23 = fTemp13 + 0.2617993877991494;
			double fTemp24 = ((fTemp23 >= 6.283185307179586) ? fTemp13 + -6.021385919380437 : fTemp23);
			double fTemp25 = 0.15915494309189535 * (fTemp24 + 0.1308996938995747);
			double fTemp26 = fTemp25 - std::floor(fTemp25);
			double fTemp27 = std::sin(6.283185307179586 * fTemp26) + fSlow5 * std::sin(12.566370614359172 * fTemp26);
			double fTemp28 = fTemp16 - fTemp27;
			double fTemp29 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp27 + fSlow11 * fTemp22));
			double fTemp30 = 1.0 - mydsp_faustpower2_f(fTemp29);
			double fTemp31 = fTemp29 - fTemp22;
			double fTemp32 = fSlow12 * std::fabs(fTemp31) + 1.0;
			double fTemp33 = std::max<double>(-1.0, std::min<double>(1.0, fTemp22 - fSlow4 * (fTemp28 * (fSlow9 * fTemp30 + fTemp31 / (fTemp32 * (fSlow13 * ((-(fSlow4 * fTemp28) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp31 / fTemp32))))) / (1.000000001 - fSlow14 * fTemp30))));
			double fTemp34 = fTemp24 + 0.2617993877991494;
			double fTemp35 = ((fTemp34 >= 6.283185307179586) ? fTemp24 + -6.021385919380437 : fTemp34);
			double fTemp36 = 0.15915494309189535 * (fTemp35 + 0.1308996938995747);
			double fTemp37 = fTemp36 - std::floor(fTemp36);
			double fTemp38 = std::sin(6.283185307179586 * fTemp37) + fSlow5 * std::sin(12.566370614359172 * fTemp37);
			double fTemp39 = fTemp27 - fTemp38;
			double fTemp40 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp38 + fSlow11 * fTemp33));
			double fTemp41 = 1.0 - mydsp_faustpower2_f(fTemp40);
			double fTemp42 = fTemp40 - fTemp33;
			double fTemp43 = fSlow12 * std::fabs(fTemp42) + 1.0;
			double fTemp44 = std::max<double>(-1.0, std::min<double>(1.0, fTemp33 - fSlow4 * (fTemp39 * (fSlow9 * fTemp41 + fTemp42 / (fTemp43 * (fSlow13 * ((-(fSlow4 * fTemp39) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp42 / fTemp43))))) / (1.000000001 - fSlow14 * fTemp41))));
			double fTemp45 = fTemp35 + 0.2617993877991494;
			double fTemp46 = ((fTemp45 >= 6.283185307179586) ? fTemp35 + -6.021385919380437 : fTemp45);
			double fTemp47 = 0.15915494309189535 * (fTemp46 + 0.1308996938995747);
			double fTemp48 = fTemp47 - std::floor(fTemp47);
			double fTemp49 = std::sin(6.283185307179586 * fTemp48) + fSlow5 * std::sin(12.566370614359172 * fTemp48);
			double fTemp50 = fTemp38 - fTemp49;
			double fTemp51 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp49 + fSlow11 * fTemp44));
			double fTemp52 = 1.0 - mydsp_faustpower2_f(fTemp51);
			double fTemp53 = fTemp51 - fTemp44;
			double fTemp54 = fSlow12 * std::fabs(fTemp53) + 1.0;
			double fTemp55 = std::max<double>(-1.0, std::min<double>(1.0, fTemp44 - fSlow4 * (fTemp50 * (fSlow9 * fTemp52 + fTemp53 / (fTemp54 * (fSlow13 * ((-(fSlow4 * fTemp50) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp53 / fTemp54))))) / (1.000000001 - fSlow14 * fTemp52))));
			double fTemp56 = fTemp46 + 0.2617993877991494;
			double fTemp57 = ((fTemp56 >= 6.283185307179586) ? fTemp46 + -6.021385919380437 : fTemp56);
			double fTemp58 = 0.15915494309189535 * (fTemp57 + 0.1308996938995747);
			double fTemp59 = fTemp58 - std::floor(fTemp58);
			double fTemp60 = std::sin(6.283185307179586 * fTemp59) + fSlow5 * std::sin(12.566370614359172 * fTemp59);
			double fTemp61 = fTemp49 - fTemp60;
			double fTemp62 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp60 + fSlow11 * fTemp55));
			double fTemp63 = 1.0 - mydsp_faustpower2_f(fTemp62);
			double fTemp64 = fTemp62 - fTemp55;
			double fTemp65 = fSlow12 * std::fabs(fTemp64) + 1.0;
			double fTemp66 = std::max<double>(-1.0, std::min<double>(1.0, fTemp55 - fSlow4 * (fTemp61 * (fSlow9 * fTemp63 + fTemp64 / (fTemp65 * (fSlow13 * ((-(fSlow4 * fTemp61) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp64 / fTemp65))))) / (1.000000001 - fSlow14 * fTemp63))));
			double fTemp67 = fTemp57 + 0.2617993877991494;
			double fTemp68 = ((fTemp67 >= 6.283185307179586) ? fTemp57 + -6.021385919380437 : fTemp67);
			double fTemp69 = 0.15915494309189535 * (fTemp68 + 0.1308996938995747);
			double fTemp70 = fTemp69 - std::floor(fTemp69);
			double fTemp71 = std::sin(6.283185307179586 * fTemp70) + fSlow5 * std::sin(12.566370614359172 * fTemp70);
			double fTemp72 = fTemp60 - fTemp71;
			double fTemp73 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp71 + fSlow11 * fTemp66));
			double fTemp74 = 1.0 - mydsp_faustpower2_f(fTemp73);
			double fTemp75 = fTemp73 - fTemp66;
			double fTemp76 = fSlow12 * std::fabs(fTemp75) + 1.0;
			double fTemp77 = std::max<double>(-1.0, std::min<double>(1.0, fTemp66 - fSlow4 * (fTemp72 * (fSlow9 * fTemp74 + fTemp75 / (fTemp76 * (fSlow13 * ((-(fSlow4 * fTemp72) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp75 / fTemp76))))) / (1.000000001 - fSlow14 * fTemp74))));
			double fTemp78 = fTemp68 + 0.2617993877991494;
			double fTemp79 = ((fTemp78 >= 6.283185307179586) ? fTemp68 + -6.021385919380437 : fTemp78);
			double fTemp80 = 0.15915494309189535 * (fTemp79 + 0.1308996938995747);
			double fTemp81 = fTemp80 - std::floor(fTemp80);
			double fTemp82 = std::sin(6.283185307179586 * fTemp81) + fSlow5 * std::sin(12.566370614359172 * fTemp81);
			double fTemp83 = fTemp71 - fTemp82;
			double fTemp84 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp82 + fSlow11 * fTemp77));
			double fTemp85 = 1.0 - mydsp_faustpower2_f(fTemp84);
			double fTemp86 = fTemp84 - fTemp77;
			double fTemp87 = fSlow12 * std::fabs(fTemp86) + 1.0;
			double fTemp88 = std::max<double>(-1.0, std::min<double>(1.0, fTemp77 - fSlow4 * (fTemp83 * (fSlow9 * fTemp85 + fTemp86 / (fTemp87 * (fSlow13 * ((-(fSlow4 * fTemp83) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp86 / fTemp87))))) / (1.000000001 - fSlow14 * fTemp85))));
			double fTemp89 = fTemp79 + 0.2617993877991494;
			double fTemp90 = ((fTemp89 >= 6.283185307179586) ? fTemp79 + -6.021385919380437 : fTemp89);
			double fTemp91 = 0.15915494309189535 * (fTemp90 + 0.1308996938995747);
			double fTemp92 = fTemp91 - std::floor(fTemp91);
			double fTemp93 = std::sin(6.283185307179586 * fTemp92) + fSlow5 * std::sin(12.566370614359172 * fTemp92);
			double fTemp94 = fTemp82 - fTemp93;
			double fTemp95 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp93 + fSlow11 * fTemp88));
			double fTemp96 = 1.0 - mydsp_faustpower2_f(fTemp95);
			double fTemp97 = fTemp95 - fTemp88;
			double fTemp98 = fSlow12 * std::fabs(fTemp97) + 1.0;
			double fTemp99 = std::max<double>(-1.0, std::min<double>(1.0, fTemp88 - fSlow4 * (fTemp94 * (fSlow9 * fTemp96 + fTemp97 / (fTemp98 * (fSlow13 * ((-(fSlow4 * fTemp94) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp97 / fTemp98))))) / (1.000000001 - fSlow14 * fTemp96))));
			double fTemp100 = fTemp90 + 0.2617993877991494;
			double fTemp101 = ((fTemp100 >= 6.283185307179586) ? fTemp90 + -6.021385919380437 : fTemp100);
			double fTemp102 = 0.15915494309189535 * (fTemp101 + 0.1308996938995747);
			double fTemp103 = fTemp102 - std::floor(fTemp102);
			double fTemp104 = std::sin(6.283185307179586 * fTemp103) + fSlow5 * std::sin(12.566370614359172 * fTemp103);
			double fTemp105 = fTemp93 - fTemp104;
			double fTemp106 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp104 + fSlow11 * fTemp99));
			double fTemp107 = 1.0 - mydsp_faustpower2_f(fTemp106);
			double fTemp108 = fTemp106 - fTemp99;
			double fTemp109 = fSlow12 * std::fabs(fTemp108) + 1.0;
			double fTemp110 = std::max<double>(-1.0, std::min<double>(1.0, fTemp99 - fSlow4 * (fTemp105 * (fSlow9 * fTemp107 + fTemp108 / (fTemp109 * (fSlow13 * ((-(fSlow4 * fTemp105) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp108 / fTemp109))))) / (1.000000001 - fSlow14 * fTemp107))));
			double fTemp111 = fTemp101 + 0.2617993877991494;
			double fTemp112 = ((fTemp111 >= 6.283185307179586) ? fTemp101 + -6.021385919380437 : fTemp111);
			double fTemp113 = 0.15915494309189535 * (fTemp112 + 0.1308996938995747);
			double fTemp114 = fTemp113 - std::floor(fTemp113);
			double fTemp115 = std::sin(6.283185307179586 * fTemp114) + fSlow5 * std::sin(12.566370614359172 * fTemp114);
			double fTemp116 = fTemp104 - fTemp115;
			double fTemp117 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp115 + fSlow11 * fTemp110));
			double fTemp118 = 1.0 - mydsp_faustpower2_f(fTemp117);
			double fTemp119 = fTemp117 - fTemp110;
			double fTemp120 = fSlow12 * std::fabs(fTemp119) + 1.0;
			double fTemp121 = std::max<double>(-1.0, std::min<double>(1.0, fTemp110 - fSlow4 * (fTemp116 * (fSlow9 * fTemp118 + fTemp119 / (fTemp120 * (fSlow13 * ((-(fSlow4 * fTemp116) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp119 / fTemp120))))) / (1.000000001 - fSlow14 * fTemp118))));
			double fTemp122 = fTemp112 + 0.2617993877991494;
			double fTemp123 = ((fTemp122 >= 6.283185307179586) ? fTemp112 + -6.021385919380437 : fTemp122);
			double fTemp124 = 0.15915494309189535 * (fTemp123 + 0.1308996938995747);
			double fTemp125 = fTemp124 - std::floor(fTemp124);
			double fTemp126 = std::sin(6.283185307179586 * fTemp125) + fSlow5 * std::sin(12.566370614359172 * fTemp125);
			double fTemp127 = fTemp115 - fTemp126;
			double fTemp128 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp126 + fSlow11 * fTemp121));
			double fTemp129 = 1.0 - mydsp_faustpower2_f(fTemp128);
			double fTemp130 = fTemp128 - fTemp121;
			double fTemp131 = fSlow12 * std::fabs(fTemp130) + 1.0;
			double fTemp132 = std::max<double>(-1.0, std::min<double>(1.0, fTemp121 - fSlow4 * (fTemp127 * (fSlow9 * fTemp129 + fTemp130 / (fTemp131 * (fSlow13 * ((-(fSlow4 * fTemp127) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp130 / fTemp131))))) / (1.000000001 - fSlow14 * fTemp129))));
			double fTemp133 = fTemp123 + 0.2617993877991494;
			double fTemp134 = ((fTemp133 >= 6.283185307179586) ? fTemp123 + -6.021385919380437 : fTemp133);
			double fTemp135 = 0.15915494309189535 * (fTemp134 + 0.1308996938995747);
			double fTemp136 = fTemp135 - std::floor(fTemp135);
			double fTemp137 = std::sin(6.283185307179586 * fTemp136) + fSlow5 * std::sin(12.566370614359172 * fTemp136);
			double fTemp138 = fTemp126 - fTemp137;
			double fTemp139 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp137 + fSlow11 * fTemp132));
			double fTemp140 = 1.0 - mydsp_faustpower2_f(fTemp139);
			double fTemp141 = fTemp139 - fTemp132;
			double fTemp142 = fSlow12 * std::fabs(fTemp141) + 1.0;
			double fTemp143 = std::max<double>(-1.0, std::min<double>(1.0, fTemp132 - fSlow4 * (fTemp138 * (fSlow9 * fTemp140 + fTemp141 / (fTemp142 * (fSlow13 * ((-(fSlow4 * fTemp138) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp141 / fTemp142))))) / (1.000000001 - fSlow14 * fTemp140))));
			double fTemp144 = fTemp134 + 0.2617993877991494;
			double fTemp145 = ((fTemp144 >= 6.283185307179586) ? fTemp134 + -6.021385919380437 : fTemp144);
			double fTemp146 = 0.15915494309189535 * (fTemp145 + 0.1308996938995747);
			double fTemp147 = fTemp146 - std::floor(fTemp146);
			double fTemp148 = std::sin(6.283185307179586 * fTemp147) + fSlow5 * std::sin(12.566370614359172 * fTemp147);
			double fTemp149 = fTemp137 - fTemp148;
			double fTemp150 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp148 + fSlow11 * fTemp143));
			double fTemp151 = 1.0 - mydsp_faustpower2_f(fTemp150);
			double fTemp152 = fTemp150 - fTemp143;
			double fTemp153 = fSlow12 * std::fabs(fTemp152) + 1.0;
			double fTemp154 = std::max<double>(-1.0, std::min<double>(1.0, fTemp143 - fSlow4 * (fTemp149 * (fSlow9 * fTemp151 + fTemp152 / (fTemp153 * (fSlow13 * ((-(fSlow4 * fTemp149) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp152 / fTemp153))))) / (1.000000001 - fSlow14 * fTemp151))));
			double fTemp155 = fTemp145 + 0.2617993877991494;
			double fTemp156 = ((fTemp155 >= 6.283185307179586) ? fTemp145 + -6.021385919380437 : fTemp155);
			double fTemp157 = 0.15915494309189535 * (fTemp156 + 0.1308996938995747);
			double fTemp158 = fTemp157 - std::floor(fTemp157);
			double fTemp159 = std::sin(6.283185307179586 * fTemp158) + fSlow5 * std::sin(12.566370614359172 * fTemp158);
			double fTemp160 = fTemp148 - fTemp159;
			double fTemp161 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp159 + fSlow11 * fTemp154));
			double fTemp162 = 1.0 - mydsp_faustpower2_f(fTemp161);
			double fTemp163 = fTemp161 - fTemp154;
			double fTemp164 = fSlow12 * std::fabs(fTemp163) + 1.0;
			double fTemp165 = std::max<double>(-1.0, std::min<double>(1.0, fTemp154 - fSlow4 * (fTemp160 * (fSlow9 * fTemp162 + fTemp163 / (fTemp164 * (fSlow13 * ((-(fSlow4 * fTemp160) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp163 / fTemp164))))) / (1.000000001 - fSlow14 * fTemp162))));
			double fTemp166 = fTemp156 + 0.2617993877991494;
			double fTemp167 = ((fTemp166 >= 6.283185307179586) ? fTemp156 + -6.021385919380437 : fTemp166);
			double fTemp168 = 0.15915494309189535 * (fTemp167 + 0.1308996938995747);
			double fTemp169 = fTemp168 - std::floor(fTemp168);
			double fTemp170 = std::sin(6.283185307179586 * fTemp169) + fSlow5 * std::sin(12.566370614359172 * fTemp169);
			double fTemp171 = fTemp159 - fTemp170;
			double fTemp172 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp170 + fSlow11 * fTemp165));
			double fTemp173 = 1.0 - mydsp_faustpower2_f(fTemp172);
			double fTemp174 = fTemp172 - fTemp165;
			double fTemp175 = fSlow12 * std::fabs(fTemp174) + 1.0;
			double fTemp176 = std::max<double>(-1.0, std::min<double>(1.0, fTemp165 - fSlow4 * (fTemp171 * (fSlow9 * fTemp173 + fTemp174 / (fTemp175 * (fSlow13 * ((-(fSlow4 * fTemp171) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp174 / fTemp175))))) / (1.000000001 - fSlow14 * fTemp173))));
			double fTemp177 = fTemp167 + 0.2617993877991494;
			double fTemp178 = ((fTemp177 >= 6.283185307179586) ? fTemp167 + -6.021385919380437 : fTemp177);
			double fTemp179 = 0.15915494309189535 * (fTemp178 + 0.1308996938995747);
			double fTemp180 = fTemp179 - std::floor(fTemp179);
			double fTemp181 = std::sin(6.283185307179586 * fTemp180) + fSlow5 * std::sin(12.566370614359172 * fTemp180);
			double fTemp182 = fTemp170 - fTemp181;
			double fTemp183 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp181 + fSlow11 * fTemp176));
			double fTemp184 = 1.0 - mydsp_faustpower2_f(fTemp183);
			double fTemp185 = fTemp183 - fTemp176;
			double fTemp186 = fSlow12 * std::fabs(fTemp185) + 1.0;
			double fTemp187 = std::max<double>(-1.0, std::min<double>(1.0, fTemp176 - fSlow4 * (fTemp182 * (fSlow9 * fTemp184 + fTemp185 / (fTemp186 * (fSlow13 * ((-(fSlow4 * fTemp182) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp185 / fTemp186))))) / (1.000000001 - fSlow14 * fTemp184))));
			double fTemp188 = fTemp178 + 0.2617993877991494;
			double fTemp189 = ((fTemp188 >= 6.283185307179586) ? fTemp178 + -6.021385919380437 : fTemp188);
			double fTemp190 = 0.15915494309189535 * (fTemp189 + 0.1308996938995747);
			double fTemp191 = fTemp190 - std::floor(fTemp190);
			double fTemp192 = std::sin(6.283185307179586 * fTemp191) + fSlow5 * std::sin(12.566370614359172 * fTemp191);
			double fTemp193 = fTemp181 - fTemp192;
			double fTemp194 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp192 + fSlow11 * fTemp187));
			double fTemp195 = 1.0 - mydsp_faustpower2_f(fTemp194);
			double fTemp196 = fTemp194 - fTemp187;
			double fTemp197 = fSlow12 * std::fabs(fTemp196) + 1.0;
			double fTemp198 = std::max<double>(-1.0, std::min<double>(1.0, fTemp187 - fSlow4 * (fTemp193 * (fSlow9 * fTemp195 + fTemp196 / (fTemp197 * (fSlow13 * ((-(fSlow4 * fTemp193) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp196 / fTemp197))))) / (1.000000001 - fSlow14 * fTemp195))));
			double fTemp199 = fTemp189 + 0.2617993877991494;
			double fTemp200 = ((fTemp199 >= 6.283185307179586) ? fTemp189 + -6.021385919380437 : fTemp199);
			double fTemp201 = 0.15915494309189535 * (fTemp200 + 0.1308996938995747);
			double fTemp202 = fTemp201 - std::floor(fTemp201);
			double fTemp203 = std::sin(6.283185307179586 * fTemp202) + fSlow5 * std::sin(12.566370614359172 * fTemp202);
			double fTemp204 = fTemp192 - fTemp203;
			double fTemp205 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp203 + fSlow11 * fTemp198));
			double fTemp206 = 1.0 - mydsp_faustpower2_f(fTemp205);
			double fTemp207 = fTemp205 - fTemp198;
			double fTemp208 = fSlow12 * std::fabs(fTemp207) + 1.0;
			double fTemp209 = std::max<double>(-1.0, std::min<double>(1.0, fTemp198 - fSlow4 * (fTemp204 * (fSlow9 * fTemp206 + fTemp207 / (fTemp208 * (fSlow13 * ((-(fSlow4 * fTemp204) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp207 / fTemp208))))) / (1.000000001 - fSlow14 * fTemp206))));
			double fTemp210 = fTemp200 + 0.2617993877991494;
			double fTemp211 = ((fTemp210 >= 6.283185307179586) ? fTemp200 + -6.021385919380437 : fTemp210);
			double fTemp212 = 0.15915494309189535 * (fTemp211 + 0.1308996938995747);
			double fTemp213 = fTemp212 - std::floor(fTemp212);
			double fTemp214 = std::sin(6.283185307179586 * fTemp213) + fSlow5 * std::sin(12.566370614359172 * fTemp213);
			double fTemp215 = fTemp203 - fTemp214;
			double fTemp216 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp214 + fSlow11 * fTemp209));
			double fTemp217 = 1.0 - mydsp_faustpower2_f(fTemp216);
			double fTemp218 = fTemp216 - fTemp209;
			double fTemp219 = fSlow12 * std::fabs(fTemp218) + 1.0;
			double fTemp220 = std::max<double>(-1.0, std::min<double>(1.0, fTemp209 - fSlow4 * (fTemp215 * (fSlow9 * fTemp217 + fTemp218 / (fTemp219 * (fSlow13 * ((-(fSlow4 * fTemp215) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp218 / fTemp219))))) / (1.000000001 - fSlow14 * fTemp217))));
			double fTemp221 = fTemp211 + 0.2617993877991494;
			double fTemp222 = ((fTemp221 >= 6.283185307179586) ? fTemp211 + -6.021385919380437 : fTemp221);
			double fTemp223 = 0.15915494309189535 * (fTemp222 + 0.1308996938995747);
			double fTemp224 = fTemp223 - std::floor(fTemp223);
			double fTemp225 = std::sin(6.283185307179586 * fTemp224) + fSlow5 * std::sin(12.566370614359172 * fTemp224);
			double fTemp226 = fTemp214 - fTemp225;
			double fTemp227 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp225 + fSlow11 * fTemp220));
			double fTemp228 = 1.0 - mydsp_faustpower2_f(fTemp227);
			double fTemp229 = fTemp227 - fTemp220;
			double fTemp230 = fSlow12 * std::fabs(fTemp229) + 1.0;
			double fTemp231 = std::max<double>(-1.0, std::min<double>(1.0, fTemp220 - fSlow4 * (fTemp226 * (fSlow9 * fTemp228 + fTemp229 / (fTemp230 * (fSlow13 * ((-(fSlow4 * fTemp226) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp229 / fTemp230))))) / (1.000000001 - fSlow14 * fTemp228))));
			double fTemp232 = fTemp222 + 0.2617993877991494;
			double fTemp233 = ((fTemp232 >= 6.283185307179586) ? fTemp222 + -6.021385919380437 : fTemp232);
			double fTemp234 = 0.15915494309189535 * (fTemp233 + 0.1308996938995747);
			double fTemp235 = fTemp234 - std::floor(fTemp234);
			double fTemp236 = std::sin(6.283185307179586 * fTemp235) + fSlow5 * std::sin(12.566370614359172 * fTemp235);
			double fTemp237 = fTemp225 - fTemp236;
			double fTemp238 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp236 + fSlow11 * fTemp231));
			double fTemp239 = 1.0 - mydsp_faustpower2_f(fTemp238);
			double fTemp240 = fTemp238 - fTemp231;
			double fTemp241 = fSlow12 * std::fabs(fTemp240) + 1.0;
			double fTemp242 = std::max<double>(-1.0, std::min<double>(1.0, fTemp231 - fSlow4 * (fTemp237 * (fSlow9 * fTemp239 + fTemp240 / (fTemp241 * (fSlow13 * ((-(fSlow4 * fTemp237) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp240 / fTemp241))))) / (1.000000001 - fSlow14 * fTemp239))));
			double fTemp243 = fTemp233 + 0.2617993877991494;
			double fTemp244 = ((fTemp243 >= 6.283185307179586) ? fTemp233 + -6.021385919380437 : fTemp243);
			double fTemp245 = 0.15915494309189535 * (fTemp244 + 0.1308996938995747);
			double fTemp246 = fTemp245 - std::floor(fTemp245);
			double fTemp247 = std::sin(6.283185307179586 * fTemp246) + fSlow5 * std::sin(12.566370614359172 * fTemp246);
			double fTemp248 = fTemp236 - fTemp247;
			double fTemp249 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp247 + fSlow11 * fTemp242));
			double fTemp250 = 1.0 - mydsp_faustpower2_f(fTemp249);
			double fTemp251 = fTemp249 - fTemp242;
			double fTemp252 = fSlow12 * std::fabs(fTemp251) + 1.0;
			double fTemp253 = std::max<double>(-1.0, std::min<double>(1.0, fTemp242 - fSlow4 * (fTemp248 * (fSlow9 * fTemp250 + fTemp251 / (fTemp252 * (fSlow13 * ((-(fSlow4 * fTemp248) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp251 / fTemp252))))) / (1.000000001 - fSlow14 * fTemp250))));
			double fTemp254 = fTemp244 + 0.2617993877991494;
			double fTemp255 = ((fTemp254 >= 6.283185307179586) ? fTemp244 + -6.021385919380437 : fTemp254);
			double fTemp256 = 0.15915494309189535 * (fTemp255 + 0.1308996938995747);
			double fTemp257 = fTemp256 - std::floor(fTemp256);
			double fTemp258 = std::sin(6.283185307179586 * fTemp257) + fSlow5 * std::sin(12.566370614359172 * fTemp257);
			double fTemp259 = fTemp247 - fTemp258;
			double fTemp260 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp258 + fSlow11 * fTemp253));
			double fTemp261 = 1.0 - mydsp_faustpower2_f(fTemp260);
			double fTemp262 = fTemp260 - fTemp253;
			double fTemp263 = fSlow12 * std::fabs(fTemp262) + 1.0;
			double fTemp264 = std::max<double>(-1.0, std::min<double>(1.0, fTemp253 - fSlow4 * (fTemp259 * (fSlow9 * fTemp261 + fTemp262 / (fTemp263 * (fSlow13 * ((-(fSlow4 * fTemp259) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp262 / fTemp263))))) / (1.000000001 - fSlow14 * fTemp261))));
			double fTemp265 = fTemp255 + 0.2617993877991494;
			double fTemp266 = ((fTemp265 >= 6.283185307179586) ? fTemp255 + -6.021385919380437 : fTemp265);
			double fTemp267 = 0.15915494309189535 * (fTemp266 + 0.1308996938995747);
			double fTemp268 = fTemp267 - std::floor(fTemp267);
			double fTemp269 = std::sin(6.283185307179586 * fTemp268) + fSlow5 * std::sin(12.566370614359172 * fTemp268);
			double fTemp270 = fTemp258 - fTemp269;
			double fTemp271 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp269 + fSlow11 * fTemp264));
			double fTemp272 = 1.0 - mydsp_faustpower2_f(fTemp271);
			double fTemp273 = fTemp271 - fTemp264;
			double fTemp274 = fSlow12 * std::fabs(fTemp273) + 1.0;
			double fTemp275 = std::max<double>(-1.0, std::min<double>(1.0, fTemp264 - fSlow4 * (fTemp270 * (fSlow9 * fTemp272 + fTemp273 / (fTemp274 * (fSlow13 * ((-(fSlow4 * fTemp270) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp273 / fTemp274))))) / (1.000000001 - fSlow14 * fTemp272))));
			double fTemp276 = fTemp266 + 0.2617993877991494;
			double fTemp277 = ((fTemp276 >= 6.283185307179586) ? fTemp266 + -6.021385919380437 : fTemp276);
			double fTemp278 = 0.15915494309189535 * (fTemp277 + 0.1308996938995747);
			double fTemp279 = fTemp278 - std::floor(fTemp278);
			double fTemp280 = std::sin(6.283185307179586 * fTemp279) + fSlow5 * std::sin(12.566370614359172 * fTemp279);
			double fTemp281 = fTemp269 - fTemp280;
			double fTemp282 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp280 + fSlow11 * fTemp275));
			double fTemp283 = 1.0 - mydsp_faustpower2_f(fTemp282);
			double fTemp284 = fTemp282 - fTemp275;
			double fTemp285 = fSlow12 * std::fabs(fTemp284) + 1.0;
			double fTemp286 = std::max<double>(-1.0, std::min<double>(1.0, fTemp275 - fSlow4 * (fTemp281 * (fSlow9 * fTemp283 + fTemp284 / (fTemp285 * (fSlow13 * ((-(fSlow4 * fTemp281) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp284 / fTemp285))))) / (1.000000001 - fSlow14 * fTemp283))));
			double fTemp287 = fTemp277 + 0.2617993877991494;
			double fTemp288 = ((fTemp287 >= 6.283185307179586) ? fTemp277 + -6.021385919380437 : fTemp287);
			double fTemp289 = 0.15915494309189535 * (fTemp288 + 0.1308996938995747);
			double fTemp290 = fTemp289 - std::floor(fTemp289);
			double fTemp291 = std::sin(6.283185307179586 * fTemp290) + fSlow5 * std::sin(12.566370614359172 * fTemp290);
			double fTemp292 = fTemp280 - fTemp291;
			double fTemp293 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp291 + fSlow11 * fTemp286));
			double fTemp294 = 1.0 - mydsp_faustpower2_f(fTemp293);
			double fTemp295 = fTemp293 - fTemp286;
			double fTemp296 = fSlow12 * std::fabs(fTemp295) + 1.0;
			double fTemp297 = std::max<double>(-1.0, std::min<double>(1.0, fTemp286 - fSlow4 * (fTemp292 * (fSlow9 * fTemp294 + fTemp295 / (fTemp296 * (fSlow13 * ((-(fSlow4 * fTemp292) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp295 / fTemp296))))) / (1.000000001 - fSlow14 * fTemp294))));
			double fTemp298 = fTemp288 + 0.2617993877991494;
			double fTemp299 = ((fTemp298 >= 6.283185307179586) ? fTemp288 + -6.021385919380437 : fTemp298);
			double fTemp300 = 0.15915494309189535 * (fTemp299 + 0.1308996938995747);
			double fTemp301 = fTemp300 - std::floor(fTemp300);
			double fTemp302 = std::sin(6.283185307179586 * fTemp301) + fSlow5 * std::sin(12.566370614359172 * fTemp301);
			double fTemp303 = fTemp291 - fTemp302;
			double fTemp304 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp302 + fSlow11 * fTemp297));
			double fTemp305 = 1.0 - mydsp_faustpower2_f(fTemp304);
			double fTemp306 = fTemp304 - fTemp297;
			double fTemp307 = fSlow12 * std::fabs(fTemp306) + 1.0;
			double fTemp308 = std::max<double>(-1.0, std::min<double>(1.0, fTemp297 - fSlow4 * (fTemp303 * (fSlow9 * fTemp305 + fTemp306 / (fTemp307 * (fSlow13 * ((-(fSlow4 * fTemp303) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp306 / fTemp307))))) / (1.000000001 - fSlow14 * fTemp305))));
			double fTemp309 = fTemp299 + 0.2617993877991494;
			double fTemp310 = ((fTemp309 >= 6.283185307179586) ? fTemp299 + -6.021385919380437 : fTemp309);
			double fTemp311 = 0.15915494309189535 * (fTemp310 + 0.1308996938995747);
			double fTemp312 = fTemp311 - std::floor(fTemp311);
			double fTemp313 = std::sin(6.283185307179586 * fTemp312) + fSlow5 * std::sin(12.566370614359172 * fTemp312);
			double fTemp314 = fTemp302 - fTemp313;
			double fTemp315 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp313 + fSlow11 * fTemp308));
			double fTemp316 = 1.0 - mydsp_faustpower2_f(fTemp315);
			double fTemp317 = fTemp315 - fTemp308;
			double fTemp318 = fSlow12 * std::fabs(fTemp317) + 1.0;
			double fTemp319 = std::max<double>(-1.0, std::min<double>(1.0, fTemp308 - fSlow4 * (fTemp314 * (fSlow9 * fTemp316 + fTemp317 / (fTemp318 * (fSlow13 * ((-(fSlow4 * fTemp314) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp317 / fTemp318))))) / (1.000000001 - fSlow14 * fTemp316))));
			double fTemp320 = fTemp310 + 0.2617993877991494;
			double fTemp321 = ((fTemp320 >= 6.283185307179586) ? fTemp310 + -6.021385919380437 : fTemp320);
			double fTemp322 = 0.15915494309189535 * (fTemp321 + 0.1308996938995747);
			double fTemp323 = fTemp322 - std::floor(fTemp322);
			double fTemp324 = std::sin(6.283185307179586 * fTemp323) + fSlow5 * std::sin(12.566370614359172 * fTemp323);
			double fTemp325 = fTemp313 - fTemp324;
			double fTemp326 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp324 + fSlow11 * fTemp319));
			double fTemp327 = 1.0 - mydsp_faustpower2_f(fTemp326);
			double fTemp328 = fTemp326 - fTemp319;
			double fTemp329 = fSlow12 * std::fabs(fTemp328) + 1.0;
			double fTemp330 = std::max<double>(-1.0, std::min<double>(1.0, fTemp319 - fSlow4 * (fTemp325 * (fSlow9 * fTemp327 + fTemp328 / (fTemp329 * (fSlow13 * ((-(fSlow4 * fTemp325) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp328 / fTemp329))))) / (1.000000001 - fSlow14 * fTemp327))));
			double fTemp331 = fTemp321 + 0.2617993877991494;
			double fTemp332 = ((fTemp331 >= 6.283185307179586) ? fTemp321 + -6.021385919380437 : fTemp331);
			double fTemp333 = 0.15915494309189535 * (fTemp332 + 0.1308996938995747);
			double fTemp334 = fTemp333 - std::floor(fTemp333);
			double fTemp335 = std::sin(6.283185307179586 * fTemp334) + fSlow5 * std::sin(12.566370614359172 * fTemp334);
			double fTemp336 = fTemp324 - fTemp335;
			double fTemp337 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp335 + fSlow11 * fTemp330));
			double fTemp338 = 1.0 - mydsp_faustpower2_f(fTemp337);
			double fTemp339 = fTemp337 - fTemp330;
			double fTemp340 = fSlow12 * std::fabs(fTemp339) + 1.0;
			double fTemp341 = std::max<double>(-1.0, std::min<double>(1.0, fTemp330 - fSlow4 * (fTemp336 * (fSlow9 * fTemp338 + fTemp339 / (fTemp340 * (fSlow13 * ((-(fSlow4 * fTemp336) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp339 / fTemp340))))) / (1.000000001 - fSlow14 * fTemp338))));
			double fTemp342 = fTemp332 + 0.2617993877991494;
			double fTemp343 = ((fTemp342 >= 6.283185307179586) ? fTemp332 + -6.021385919380437 : fTemp342);
			double fTemp344 = 0.15915494309189535 * (fTemp343 + 0.1308996938995747);
			double fTemp345 = fTemp344 - std::floor(fTemp344);
			double fTemp346 = std::sin(6.283185307179586 * fTemp345) + fSlow5 * std::sin(12.566370614359172 * fTemp345);
			double fTemp347 = fTemp335 - fTemp346;
			double fTemp348 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp346 + fSlow11 * fTemp341));
			double fTemp349 = 1.0 - mydsp_faustpower2_f(fTemp348);
			double fTemp350 = fTemp348 - fTemp341;
			double fTemp351 = fSlow12 * std::fabs(fTemp350) + 1.0;
			double fTemp352 = std::max<double>(-1.0, std::min<double>(1.0, fTemp341 - fSlow4 * (fTemp347 * (fSlow9 * fTemp349 + fTemp350 / (fTemp351 * (fSlow13 * ((-(fSlow4 * fTemp347) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp350 / fTemp351))))) / (1.000000001 - fSlow14 * fTemp349))));
			double fTemp353 = fTemp343 + 0.2617993877991494;
			double fTemp354 = ((fTemp353 >= 6.283185307179586) ? fTemp343 + -6.021385919380437 : fTemp353);
			double fTemp355 = 0.15915494309189535 * (fTemp354 + 0.1308996938995747);
			double fTemp356 = fTemp355 - std::floor(fTemp355);
			double fTemp357 = std::sin(6.283185307179586 * fTemp356) + fSlow5 * std::sin(12.566370614359172 * fTemp356);
			double fTemp358 = fTemp346 - fTemp357;
			double fTemp359 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp357 + fSlow11 * fTemp352));
			double fTemp360 = 1.0 - mydsp_faustpower2_f(fTemp359);
			double fTemp361 = fTemp359 - fTemp352;
			double fTemp362 = fSlow12 * std::fabs(fTemp361) + 1.0;
			double fTemp363 = std::max<double>(-1.0, std::min<double>(1.0, fTemp352 - fSlow4 * (fTemp358 * (fSlow9 * fTemp360 + fTemp361 / (fTemp362 * (fSlow13 * ((-(fSlow4 * fTemp358) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp361 / fTemp362))))) / (1.000000001 - fSlow14 * fTemp360))));
			double fTemp364 = fTemp354 + 0.2617993877991494;
			double fTemp365 = ((fTemp364 >= 6.283185307179586) ? fTemp354 + -6.021385919380437 : fTemp364);
			double fTemp366 = 0.15915494309189535 * (fTemp365 + 0.1308996938995747);
			double fTemp367 = fTemp366 - std::floor(fTemp366);
			double fTemp368 = std::sin(6.283185307179586 * fTemp367) + fSlow5 * std::sin(12.566370614359172 * fTemp367);
			double fTemp369 = fTemp357 - fTemp368;
			double fTemp370 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp368 + fSlow11 * fTemp363));
			double fTemp371 = 1.0 - mydsp_faustpower2_f(fTemp370);
			double fTemp372 = fTemp370 - fTemp363;
			double fTemp373 = fSlow12 * std::fabs(fTemp372) + 1.0;
			double fTemp374 = std::max<double>(-1.0, std::min<double>(1.0, fTemp363 - fSlow4 * (fTemp369 * (fSlow9 * fTemp371 + fTemp372 / (fTemp373 * (fSlow13 * ((-(fSlow4 * fTemp369) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp372 / fTemp373))))) / (1.000000001 - fSlow14 * fTemp371))));
			double fTemp375 = fTemp365 + 0.2617993877991494;
			double fTemp376 = ((fTemp375 >= 6.283185307179586) ? fTemp365 + -6.021385919380437 : fTemp375);
			double fTemp377 = 0.15915494309189535 * (fTemp376 + 0.1308996938995747);
			double fTemp378 = fTemp377 - std::floor(fTemp377);
			double fTemp379 = std::sin(6.283185307179586 * fTemp378) + fSlow5 * std::sin(12.566370614359172 * fTemp378);
			double fTemp380 = fTemp368 - fTemp379;
			double fTemp381 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp379 + fSlow11 * fTemp374));
			double fTemp382 = 1.0 - mydsp_faustpower2_f(fTemp381);
			double fTemp383 = fTemp381 - fTemp374;
			double fTemp384 = fSlow12 * std::fabs(fTemp383) + 1.0;
			double fTemp385 = std::max<double>(-1.0, std::min<double>(1.0, fTemp374 - fSlow4 * (fTemp380 * (fSlow9 * fTemp382 + fTemp383 / (fTemp384 * (fSlow13 * ((-(fSlow4 * fTemp380) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp383 / fTemp384))))) / (1.000000001 - fSlow14 * fTemp382))));
			double fTemp386 = fTemp376 + 0.2617993877991494;
			double fTemp387 = ((fTemp386 >= 6.283185307179586) ? fTemp376 + -6.021385919380437 : fTemp386);
			double fTemp388 = 0.15915494309189535 * (fTemp387 + 0.1308996938995747);
			double fTemp389 = fTemp388 - std::floor(fTemp388);
			double fTemp390 = std::sin(6.283185307179586 * fTemp389) + fSlow5 * std::sin(12.566370614359172 * fTemp389);
			double fTemp391 = fTemp379 - fTemp390;
			double fTemp392 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp390 + fSlow11 * fTemp385));
			double fTemp393 = 1.0 - mydsp_faustpower2_f(fTemp392);
			double fTemp394 = fTemp392 - fTemp385;
			double fTemp395 = fSlow12 * std::fabs(fTemp394) + 1.0;
			double fTemp396 = std::max<double>(-1.0, std::min<double>(1.0, fTemp385 - fSlow4 * (fTemp391 * (fSlow9 * fTemp393 + fTemp394 / (fTemp395 * (fSlow13 * ((-(fSlow4 * fTemp391) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp394 / fTemp395))))) / (1.000000001 - fSlow14 * fTemp393))));
			double fTemp397 = fTemp387 + 0.2617993877991494;
			double fTemp398 = ((fTemp397 >= 6.283185307179586) ? fTemp387 + -6.021385919380437 : fTemp397);
			double fTemp399 = 0.15915494309189535 * (fTemp398 + 0.1308996938995747);
			double fTemp400 = fTemp399 - std::floor(fTemp399);
			double fTemp401 = std::sin(6.283185307179586 * fTemp400) + fSlow5 * std::sin(12.566370614359172 * fTemp400);
			double fTemp402 = fTemp390 - fTemp401;
			double fTemp403 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp401 + fSlow11 * fTemp396));
			double fTemp404 = 1.0 - mydsp_faustpower2_f(fTemp403);
			double fTemp405 = fTemp403 - fTemp396;
			double fTemp406 = fSlow12 * std::fabs(fTemp405) + 1.0;
			double fTemp407 = std::max<double>(-1.0, std::min<double>(1.0, fTemp396 - fSlow4 * (fTemp402 * (fSlow9 * fTemp404 + fTemp405 / (fTemp406 * (fSlow13 * ((-(fSlow4 * fTemp402) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp405 / fTemp406))))) / (1.000000001 - fSlow14 * fTemp404))));
			double fTemp408 = fTemp398 + 0.2617993877991494;
			double fTemp409 = ((fTemp408 >= 6.283185307179586) ? fTemp398 + -6.021385919380437 : fTemp408);
			double fTemp410 = 0.15915494309189535 * (fTemp409 + 0.1308996938995747);
			double fTemp411 = fTemp410 - std::floor(fTemp410);
			double fTemp412 = std::sin(6.283185307179586 * fTemp411) + fSlow5 * std::sin(12.566370614359172 * fTemp411);
			double fTemp413 = fTemp401 - fTemp412;
			double fTemp414 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp412 + fSlow11 * fTemp407));
			double fTemp415 = 1.0 - mydsp_faustpower2_f(fTemp414);
			double fTemp416 = fTemp414 - fTemp407;
			double fTemp417 = fSlow12 * std::fabs(fTemp416) + 1.0;
			double fTemp418 = std::max<double>(-1.0, std::min<double>(1.0, fTemp407 - fSlow4 * (fTemp413 * (fSlow9 * fTemp415 + fTemp416 / (fTemp417 * (fSlow13 * ((-(fSlow4 * fTemp413) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp416 / fTemp417))))) / (1.000000001 - fSlow14 * fTemp415))));
			double fTemp419 = fTemp409 + 0.2617993877991494;
			double fTemp420 = ((fTemp419 >= 6.283185307179586) ? fTemp409 + -6.021385919380437 : fTemp419);
			double fTemp421 = 0.15915494309189535 * (fTemp420 + 0.1308996938995747);
			double fTemp422 = fTemp421 - std::floor(fTemp421);
			double fTemp423 = std::sin(6.283185307179586 * fTemp422) + fSlow5 * std::sin(12.566370614359172 * fTemp422);
			double fTemp424 = fTemp412 - fTemp423;
			double fTemp425 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp423 + fSlow11 * fTemp418));
			double fTemp426 = 1.0 - mydsp_faustpower2_f(fTemp425);
			double fTemp427 = fTemp425 - fTemp418;
			double fTemp428 = fSlow12 * std::fabs(fTemp427) + 1.0;
			double fTemp429 = std::max<double>(-1.0, std::min<double>(1.0, fTemp418 - fSlow4 * (fTemp424 * (fSlow9 * fTemp426 + fTemp427 / (fTemp428 * (fSlow13 * ((-(fSlow4 * fTemp424) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp427 / fTemp428))))) / (1.000000001 - fSlow14 * fTemp426))));
			double fTemp430 = fTemp420 + 0.2617993877991494;
			double fTemp431 = ((fTemp430 >= 6.283185307179586) ? fTemp420 + -6.021385919380437 : fTemp430);
			double fTemp432 = 0.15915494309189535 * (fTemp431 + 0.1308996938995747);
			double fTemp433 = fTemp432 - std::floor(fTemp432);
			double fTemp434 = std::sin(6.283185307179586 * fTemp433) + fSlow5 * std::sin(12.566370614359172 * fTemp433);
			double fTemp435 = fTemp423 - fTemp434;
			double fTemp436 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp434 + fSlow11 * fTemp429));
			double fTemp437 = 1.0 - mydsp_faustpower2_f(fTemp436);
			double fTemp438 = fTemp436 - fTemp429;
			double fTemp439 = fSlow12 * std::fabs(fTemp438) + 1.0;
			double fTemp440 = std::max<double>(-1.0, std::min<double>(1.0, fTemp429 - fSlow4 * (fTemp435 * (fSlow9 * fTemp437 + fTemp438 / (fTemp439 * (fSlow13 * ((-(fSlow4 * fTemp435) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp438 / fTemp439))))) / (1.000000001 - fSlow14 * fTemp437))));
			double fTemp441 = fTemp431 + 0.2617993877991494;
			double fTemp442 = ((fTemp441 >= 6.283185307179586) ? fTemp431 + -6.021385919380437 : fTemp441);
			double fTemp443 = 0.15915494309189535 * (fTemp442 + 0.1308996938995747);
			double fTemp444 = fTemp443 - std::floor(fTemp443);
			double fTemp445 = std::sin(6.283185307179586 * fTemp444) + fSlow5 * std::sin(12.566370614359172 * fTemp444);
			double fTemp446 = fTemp434 - fTemp445;
			double fTemp447 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp445 + fSlow11 * fTemp440));
			double fTemp448 = 1.0 - mydsp_faustpower2_f(fTemp447);
			double fTemp449 = fTemp447 - fTemp440;
			double fTemp450 = fSlow12 * std::fabs(fTemp449) + 1.0;
			double fTemp451 = std::max<double>(-1.0, std::min<double>(1.0, fTemp440 - fSlow4 * (fTemp446 * (fSlow9 * fTemp448 + fTemp449 / (fTemp450 * (fSlow13 * ((-(fSlow4 * fTemp446) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp449 / fTemp450))))) / (1.000000001 - fSlow14 * fTemp448))));
			double fTemp452 = fTemp442 + 0.2617993877991494;
			double fTemp453 = ((fTemp452 >= 6.283185307179586) ? fTemp442 + -6.021385919380437 : fTemp452);
			double fTemp454 = 0.15915494309189535 * (fTemp453 + 0.1308996938995747);
			double fTemp455 = fTemp454 - std::floor(fTemp454);
			double fTemp456 = std::sin(6.283185307179586 * fTemp455) + fSlow5 * std::sin(12.566370614359172 * fTemp455);
			double fTemp457 = fTemp445 - fTemp456;
			double fTemp458 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp456 + fSlow11 * fTemp451));
			double fTemp459 = 1.0 - mydsp_faustpower2_f(fTemp458);
			double fTemp460 = fTemp458 - fTemp451;
			double fTemp461 = fSlow12 * std::fabs(fTemp460) + 1.0;
			double fTemp462 = std::max<double>(-1.0, std::min<double>(1.0, fTemp451 - fSlow4 * (fTemp457 * (fSlow9 * fTemp459 + fTemp460 / (fTemp461 * (fSlow13 * ((-(fSlow4 * fTemp457) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp460 / fTemp461))))) / (1.000000001 - fSlow14 * fTemp459))));
			double fTemp463 = fTemp453 + 0.2617993877991494;
			double fTemp464 = ((fTemp463 >= 6.283185307179586) ? fTemp453 + -6.021385919380437 : fTemp463);
			double fTemp465 = 0.15915494309189535 * (fTemp464 + 0.1308996938995747);
			double fTemp466 = fTemp465 - std::floor(fTemp465);
			double fTemp467 = std::sin(6.283185307179586 * fTemp466) + fSlow5 * std::sin(12.566370614359172 * fTemp466);
			double fTemp468 = fTemp456 - fTemp467;
			double fTemp469 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp467 + fSlow11 * fTemp462));
			double fTemp470 = 1.0 - mydsp_faustpower2_f(fTemp469);
			double fTemp471 = fTemp469 - fTemp462;
			double fTemp472 = fSlow12 * std::fabs(fTemp471) + 1.0;
			double fTemp473 = std::max<double>(-1.0, std::min<double>(1.0, fTemp462 - fSlow4 * (fTemp468 * (fSlow9 * fTemp470 + fTemp471 / (fTemp472 * (fSlow13 * ((-(fSlow4 * fTemp468) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp471 / fTemp472))))) / (1.000000001 - fSlow14 * fTemp470))));
			double fTemp474 = fTemp464 + 0.2617993877991494;
			double fTemp475 = ((fTemp474 >= 6.283185307179586) ? fTemp464 + -6.021385919380437 : fTemp474);
			double fTemp476 = 0.15915494309189535 * (fTemp475 + 0.1308996938995747);
			double fTemp477 = fTemp476 - std::floor(fTemp476);
			double fTemp478 = std::sin(6.283185307179586 * fTemp477) + fSlow5 * std::sin(12.566370614359172 * fTemp477);
			double fTemp479 = fTemp467 - fTemp478;
			double fTemp480 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp478 + fSlow11 * fTemp473));
			double fTemp481 = 1.0 - mydsp_faustpower2_f(fTemp480);
			double fTemp482 = fTemp480 - fTemp473;
			double fTemp483 = fSlow12 * std::fabs(fTemp482) + 1.0;
			double fTemp484 = std::max<double>(-1.0, std::min<double>(1.0, fTemp473 - fSlow4 * (fTemp479 * (fSlow9 * fTemp481 + fTemp482 / (fTemp483 * (fSlow13 * ((-(fSlow4 * fTemp479) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp482 / fTemp483))))) / (1.000000001 - fSlow14 * fTemp481))));
			double fTemp485 = fTemp475 + 0.2617993877991494;
			double fTemp486 = ((fTemp485 >= 6.283185307179586) ? fTemp475 + -6.021385919380437 : fTemp485);
			double fTemp487 = 0.15915494309189535 * (fTemp486 + 0.1308996938995747);
			double fTemp488 = fTemp487 - std::floor(fTemp487);
			double fTemp489 = std::sin(6.283185307179586 * fTemp488) + fSlow5 * std::sin(12.566370614359172 * fTemp488);
			double fTemp490 = fTemp478 - fTemp489;
			double fTemp491 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp489 + fSlow11 * fTemp484));
			double fTemp492 = 1.0 - mydsp_faustpower2_f(fTemp491);
			double fTemp493 = fTemp491 - fTemp484;
			double fTemp494 = fSlow12 * std::fabs(fTemp493) + 1.0;
			double fTemp495 = std::max<double>(-1.0, std::min<double>(1.0, fTemp484 - fSlow4 * (fTemp490 * (fSlow9 * fTemp492 + fTemp493 / (fTemp494 * (fSlow13 * ((-(fSlow4 * fTemp490) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp493 / fTemp494))))) / (1.000000001 - fSlow14 * fTemp492))));
			double fTemp496 = fTemp486 + 0.2617993877991494;
			double fTemp497 = ((fTemp496 >= 6.283185307179586) ? fTemp486 + -6.021385919380437 : fTemp496);
			double fTemp498 = 0.15915494309189535 * (fTemp497 + 0.1308996938995747);
			double fTemp499 = fTemp498 - std::floor(fTemp498);
			double fTemp500 = std::sin(6.283185307179586 * fTemp499) + fSlow5 * std::sin(12.566370614359172 * fTemp499);
			double fTemp501 = fTemp489 - fTemp500;
			double fTemp502 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp500 + fSlow11 * fTemp495));
			double fTemp503 = 1.0 - mydsp_faustpower2_f(fTemp502);
			double fTemp504 = fTemp502 - fTemp495;
			double fTemp505 = fSlow12 * std::fabs(fTemp504) + 1.0;
			double fTemp506 = std::max<double>(-1.0, std::min<double>(1.0, fTemp495 - fSlow4 * (fTemp501 * (fSlow9 * fTemp503 + fTemp504 / (fTemp505 * (fSlow13 * ((-(fSlow4 * fTemp501) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp504 / fTemp505))))) / (1.000000001 - fSlow14 * fTemp503))));
			double fTemp507 = fTemp497 + 0.2617993877991494;
			double fTemp508 = ((fTemp507 >= 6.283185307179586) ? fTemp497 + -6.021385919380437 : fTemp507);
			double fTemp509 = 0.15915494309189535 * (fTemp508 + 0.1308996938995747);
			double fTemp510 = fTemp509 - std::floor(fTemp509);
			double fTemp511 = std::sin(6.283185307179586 * fTemp510) + fSlow5 * std::sin(12.566370614359172 * fTemp510);
			double fTemp512 = fTemp500 - fTemp511;
			double fTemp513 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp511 + fSlow11 * fTemp506));
			double fTemp514 = 1.0 - mydsp_faustpower2_f(fTemp513);
			double fTemp515 = fTemp513 - fTemp506;
			double fTemp516 = fSlow12 * std::fabs(fTemp515) + 1.0;
			double fTemp517 = std::max<double>(-1.0, std::min<double>(1.0, fTemp506 - fSlow4 * (fTemp512 * (fSlow9 * fTemp514 + fTemp515 / (fTemp516 * (fSlow13 * ((-(fSlow4 * fTemp512) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp515 / fTemp516))))) / (1.000000001 - fSlow14 * fTemp514))));
			double fTemp518 = fTemp508 + 0.2617993877991494;
			double fTemp519 = ((fTemp518 >= 6.283185307179586) ? fTemp508 + -6.021385919380437 : fTemp518);
			double fTemp520 = 0.15915494309189535 * (fTemp519 + 0.1308996938995747);
			double fTemp521 = fTemp520 - std::floor(fTemp520);
			double fTemp522 = std::sin(6.283185307179586 * fTemp521) + fSlow5 * std::sin(12.566370614359172 * fTemp521);
			double fTemp523 = fTemp511 - fTemp522;
			double fTemp524 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp522 + fSlow11 * fTemp517));
			double fTemp525 = 1.0 - mydsp_faustpower2_f(fTemp524);
			double fTemp526 = fTemp524 - fTemp517;
			double fTemp527 = fSlow12 * std::fabs(fTemp526) + 1.0;
			double fTemp528 = std::max<double>(-1.0, std::min<double>(1.0, fTemp517 - fSlow4 * (fTemp523 * (fSlow9 * fTemp525 + fTemp526 / (fTemp527 * (fSlow13 * ((-(fSlow4 * fTemp523) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp526 / fTemp527))))) / (1.000000001 - fSlow14 * fTemp525))));
			double fTemp529 = fTemp519 + 0.2617993877991494;
			double fTemp530 = ((fTemp529 >= 6.283185307179586) ? fTemp519 + -6.021385919380437 : fTemp529);
			double fTemp531 = 0.15915494309189535 * (fTemp530 + 0.1308996938995747);
			double fTemp532 = fTemp531 - std::floor(fTemp531);
			double fTemp533 = std::sin(6.283185307179586 * fTemp532) + fSlow5 * std::sin(12.566370614359172 * fTemp532);
			double fTemp534 = fTemp522 - fTemp533;
			double fTemp535 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp533 + fSlow11 * fTemp528));
			double fTemp536 = 1.0 - mydsp_faustpower2_f(fTemp535);
			double fTemp537 = fTemp535 - fTemp528;
			double fTemp538 = fSlow12 * std::fabs(fTemp537) + 1.0;
			double fTemp539 = std::max<double>(-1.0, std::min<double>(1.0, fTemp528 - fSlow4 * (fTemp534 * (fSlow9 * fTemp536 + fTemp537 / (fTemp538 * (fSlow13 * ((-(fSlow4 * fTemp534) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp537 / fTemp538))))) / (1.000000001 - fSlow14 * fTemp536))));
			double fTemp540 = fTemp530 + 0.2617993877991494;
			double fTemp541 = ((fTemp540 >= 6.283185307179586) ? fTemp530 + -6.021385919380437 : fTemp540);
			double fTemp542 = 0.15915494309189535 * (fTemp541 + 0.1308996938995747);
			double fTemp543 = fTemp542 - std::floor(fTemp542);
			double fTemp544 = std::sin(6.283185307179586 * fTemp543) + fSlow5 * std::sin(12.566370614359172 * fTemp543);
			double fTemp545 = fTemp533 - fTemp544;
			double fTemp546 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp544 + fSlow11 * fTemp539));
			double fTemp547 = 1.0 - mydsp_faustpower2_f(fTemp546);
			double fTemp548 = fTemp546 - fTemp539;
			double fTemp549 = fSlow12 * std::fabs(fTemp548) + 1.0;
			double fTemp550 = std::max<double>(-1.0, std::min<double>(1.0, fTemp539 - fSlow4 * (fTemp545 * (fSlow9 * fTemp547 + fTemp548 / (fTemp549 * (fSlow13 * ((-(fSlow4 * fTemp545) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp548 / fTemp549))))) / (1.000000001 - fSlow14 * fTemp547))));
			double fTemp551 = fTemp541 + 0.2617993877991494;
			double fTemp552 = ((fTemp551 >= 6.283185307179586) ? fTemp541 + -6.021385919380437 : fTemp551);
			double fTemp553 = 0.15915494309189535 * (fTemp552 + 0.1308996938995747);
			double fTemp554 = fTemp553 - std::floor(fTemp553);
			double fTemp555 = std::sin(6.283185307179586 * fTemp554) + fSlow5 * std::sin(12.566370614359172 * fTemp554);
			double fTemp556 = fTemp544 - fTemp555;
			double fTemp557 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp555 + fSlow11 * fTemp550));
			double fTemp558 = 1.0 - mydsp_faustpower2_f(fTemp557);
			double fTemp559 = fTemp557 - fTemp550;
			double fTemp560 = fSlow12 * std::fabs(fTemp559) + 1.0;
			double fTemp561 = std::max<double>(-1.0, std::min<double>(1.0, fTemp550 - fSlow4 * (fTemp556 * (fSlow9 * fTemp558 + fTemp559 / (fTemp560 * (fSlow13 * ((-(fSlow4 * fTemp556) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp559 / fTemp560))))) / (1.000000001 - fSlow14 * fTemp558))));
			double fTemp562 = fTemp552 + 0.2617993877991494;
			double fTemp563 = ((fTemp562 >= 6.283185307179586) ? fTemp552 + -6.021385919380437 : fTemp562);
			double fTemp564 = 0.15915494309189535 * (fTemp563 + 0.1308996938995747);
			double fTemp565 = fTemp564 - std::floor(fTemp564);
			double fTemp566 = std::sin(6.283185307179586 * fTemp565) + fSlow5 * std::sin(12.566370614359172 * fTemp565);
			double fTemp567 = fTemp555 - fTemp566;
			double fTemp568 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp566 + fSlow11 * fTemp561));
			double fTemp569 = 1.0 - mydsp_faustpower2_f(fTemp568);
			double fTemp570 = fTemp568 - fTemp561;
			double fTemp571 = fSlow12 * std::fabs(fTemp570) + 1.0;
			double fTemp572 = std::max<double>(-1.0, std::min<double>(1.0, fTemp561 - fSlow4 * (fTemp567 * (fSlow9 * fTemp569 + fTemp570 / (fTemp571 * (fSlow13 * ((-(fSlow4 * fTemp567) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp570 / fTemp571))))) / (1.000000001 - fSlow14 * fTemp569))));
			double fTemp573 = fTemp563 + 0.2617993877991494;
			double fTemp574 = ((fTemp573 >= 6.283185307179586) ? fTemp563 + -6.021385919380437 : fTemp573);
			double fTemp575 = 0.15915494309189535 * (fTemp574 + 0.1308996938995747);
			double fTemp576 = fTemp575 - std::floor(fTemp575);
			double fTemp577 = std::sin(6.283185307179586 * fTemp576) + fSlow5 * std::sin(12.566370614359172 * fTemp576);
			double fTemp578 = fTemp566 - fTemp577;
			double fTemp579 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp577 + fSlow11 * fTemp572));
			double fTemp580 = 1.0 - mydsp_faustpower2_f(fTemp579);
			double fTemp581 = fTemp579 - fTemp572;
			double fTemp582 = fSlow12 * std::fabs(fTemp581) + 1.0;
			double fTemp583 = std::max<double>(-1.0, std::min<double>(1.0, fTemp572 - fSlow4 * (fTemp578 * (fSlow9 * fTemp580 + fTemp581 / (fTemp582 * (fSlow13 * ((-(fSlow4 * fTemp578) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp581 / fTemp582))))) / (1.000000001 - fSlow14 * fTemp580))));
			double fTemp584 = fTemp574 + 0.2617993877991494;
			double fTemp585 = ((fTemp584 >= 6.283185307179586) ? fTemp574 + -6.021385919380437 : fTemp584);
			double fTemp586 = 0.15915494309189535 * (fTemp585 + 0.1308996938995747);
			double fTemp587 = fTemp586 - std::floor(fTemp586);
			double fTemp588 = std::sin(6.283185307179586 * fTemp587) + fSlow5 * std::sin(12.566370614359172 * fTemp587);
			double fTemp589 = fTemp577 - fTemp588;
			double fTemp590 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp588 + fSlow11 * fTemp583));
			double fTemp591 = 1.0 - mydsp_faustpower2_f(fTemp590);
			double fTemp592 = fTemp590 - fTemp583;
			double fTemp593 = fSlow12 * std::fabs(fTemp592) + 1.0;
			double fTemp594 = std::max<double>(-1.0, std::min<double>(1.0, fTemp583 - fSlow4 * (fTemp589 * (fSlow9 * fTemp591 + fTemp592 / (fTemp593 * (fSlow13 * ((-(fSlow4 * fTemp589) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp592 / fTemp593))))) / (1.000000001 - fSlow14 * fTemp591))));
			double fTemp595 = fTemp585 + 0.2617993877991494;
			double fTemp596 = ((fTemp595 >= 6.283185307179586) ? fTemp585 + -6.021385919380437 : fTemp595);
			double fTemp597 = 0.15915494309189535 * (fTemp596 + 0.1308996938995747);
			double fTemp598 = fTemp597 - std::floor(fTemp597);
			double fTemp599 = std::sin(6.283185307179586 * fTemp598) + fSlow5 * std::sin(12.566370614359172 * fTemp598);
			double fTemp600 = fTemp588 - fTemp599;
			double fTemp601 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp599 + fSlow11 * fTemp594));
			double fTemp602 = 1.0 - mydsp_faustpower2_f(fTemp601);
			double fTemp603 = fTemp601 - fTemp594;
			double fTemp604 = fSlow12 * std::fabs(fTemp603) + 1.0;
			double fTemp605 = std::max<double>(-1.0, std::min<double>(1.0, fTemp594 - fSlow4 * (fTemp600 * (fSlow9 * fTemp602 + fTemp603 / (fTemp604 * (fSlow13 * ((-(fSlow4 * fTemp600) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp603 / fTemp604))))) / (1.000000001 - fSlow14 * fTemp602))));
			double fTemp606 = fTemp596 + 0.2617993877991494;
			double fTemp607 = ((fTemp606 >= 6.283185307179586) ? fTemp596 + -6.021385919380437 : fTemp606);
			double fTemp608 = 0.15915494309189535 * (fTemp607 + 0.1308996938995747);
			double fTemp609 = fTemp608 - std::floor(fTemp608);
			double fTemp610 = std::sin(6.283185307179586 * fTemp609) + fSlow5 * std::sin(12.566370614359172 * fTemp609);
			double fTemp611 = fTemp599 - fTemp610;
			double fTemp612 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp610 + fSlow11 * fTemp605));
			double fTemp613 = 1.0 - mydsp_faustpower2_f(fTemp612);
			double fTemp614 = fTemp612 - fTemp605;
			double fTemp615 = fSlow12 * std::fabs(fTemp614) + 1.0;
			double fTemp616 = std::max<double>(-1.0, std::min<double>(1.0, fTemp605 - fSlow4 * (fTemp611 * (fSlow9 * fTemp613 + fTemp614 / (fTemp615 * (fSlow13 * ((-(fSlow4 * fTemp611) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp614 / fTemp615))))) / (1.000000001 - fSlow14 * fTemp613))));
			double fTemp617 = fTemp607 + 0.2617993877991494;
			double fTemp618 = ((fTemp617 >= 6.283185307179586) ? fTemp607 + -6.021385919380437 : fTemp617);
			double fTemp619 = 0.15915494309189535 * (fTemp618 + 0.1308996938995747);
			double fTemp620 = fTemp619 - std::floor(fTemp619);
			double fTemp621 = std::sin(6.283185307179586 * fTemp620) + fSlow5 * std::sin(12.566370614359172 * fTemp620);
			double fTemp622 = fTemp610 - fTemp621;
			double fTemp623 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp621 + fSlow11 * fTemp616));
			double fTemp624 = 1.0 - mydsp_faustpower2_f(fTemp623);
			double fTemp625 = fTemp623 - fTemp616;
			double fTemp626 = fSlow12 * std::fabs(fTemp625) + 1.0;
			double fTemp627 = std::max<double>(-1.0, std::min<double>(1.0, fTemp616 - fSlow4 * (fTemp622 * (fSlow9 * fTemp624 + fTemp625 / (fTemp626 * (fSlow13 * ((-(fSlow4 * fTemp622) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp625 / fTemp626))))) / (1.000000001 - fSlow14 * fTemp624))));
			double fTemp628 = fTemp618 + 0.2617993877991494;
			double fTemp629 = ((fTemp628 >= 6.283185307179586) ? fTemp618 + -6.021385919380437 : fTemp628);
			double fTemp630 = 0.15915494309189535 * (fTemp629 + 0.1308996938995747);
			double fTemp631 = fTemp630 - std::floor(fTemp630);
			double fTemp632 = std::sin(6.283185307179586 * fTemp631) + fSlow5 * std::sin(12.566370614359172 * fTemp631);
			double fTemp633 = fTemp621 - fTemp632;
			double fTemp634 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp632 + fSlow11 * fTemp627));
			double fTemp635 = 1.0 - mydsp_faustpower2_f(fTemp634);
			double fTemp636 = fTemp634 - fTemp627;
			double fTemp637 = fSlow12 * std::fabs(fTemp636) + 1.0;
			double fTemp638 = std::max<double>(-1.0, std::min<double>(1.0, fTemp627 - fSlow4 * (fTemp633 * (fSlow9 * fTemp635 + fTemp636 / (fTemp637 * (fSlow13 * ((-(fSlow4 * fTemp633) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp636 / fTemp637))))) / (1.000000001 - fSlow14 * fTemp635))));
			double fTemp639 = fTemp629 + 0.2617993877991494;
			double fTemp640 = ((fTemp639 >= 6.283185307179586) ? fTemp629 + -6.021385919380437 : fTemp639);
			double fTemp641 = 0.15915494309189535 * (fTemp640 + 0.1308996938995747);
			double fTemp642 = fTemp641 - std::floor(fTemp641);
			double fTemp643 = std::sin(6.283185307179586 * fTemp642) + fSlow5 * std::sin(12.566370614359172 * fTemp642);
			double fTemp644 = fTemp632 - fTemp643;
			double fTemp645 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp643 + fSlow11 * fTemp638));
			double fTemp646 = 1.0 - mydsp_faustpower2_f(fTemp645);
			double fTemp647 = fTemp645 - fTemp638;
			double fTemp648 = fSlow12 * std::fabs(fTemp647) + 1.0;
			double fTemp649 = std::max<double>(-1.0, std::min<double>(1.0, fTemp638 - fSlow4 * (fTemp644 * (fSlow9 * fTemp646 + fTemp647 / (fTemp648 * (fSlow13 * ((-(fSlow4 * fTemp644) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp647 / fTemp648))))) / (1.000000001 - fSlow14 * fTemp646))));
			double fTemp650 = fTemp640 + 0.2617993877991494;
			double fTemp651 = ((fTemp650 >= 6.283185307179586) ? fTemp640 + -6.021385919380437 : fTemp650);
			double fTemp652 = 0.15915494309189535 * (fTemp651 + 0.1308996938995747);
			double fTemp653 = fTemp652 - std::floor(fTemp652);
			double fTemp654 = std::sin(6.283185307179586 * fTemp653) + fSlow5 * std::sin(12.566370614359172 * fTemp653);
			double fTemp655 = fTemp643 - fTemp654;
			double fTemp656 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp654 + fSlow11 * fTemp649));
			double fTemp657 = 1.0 - mydsp_faustpower2_f(fTemp656);
			double fTemp658 = fTemp656 - fTemp649;
			double fTemp659 = fSlow12 * std::fabs(fTemp658) + 1.0;
			double fTemp660 = std::max<double>(-1.0, std::min<double>(1.0, fTemp649 - fSlow4 * (fTemp655 * (fSlow9 * fTemp657 + fTemp658 / (fTemp659 * (fSlow13 * ((-(fSlow4 * fTemp655) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp658 / fTemp659))))) / (1.000000001 - fSlow14 * fTemp657))));
			double fTemp661 = fTemp651 + 0.2617993877991494;
			double fTemp662 = ((fTemp661 >= 6.283185307179586) ? fTemp651 + -6.021385919380437 : fTemp661);
			double fTemp663 = 0.15915494309189535 * (fTemp662 + 0.1308996938995747);
			double fTemp664 = fTemp663 - std::floor(fTemp663);
			double fTemp665 = std::sin(6.283185307179586 * fTemp664) + fSlow5 * std::sin(12.566370614359172 * fTemp664);
			double fTemp666 = fTemp654 - fTemp665;
			double fTemp667 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp665 + fSlow11 * fTemp660));
			double fTemp668 = 1.0 - mydsp_faustpower2_f(fTemp667);
			double fTemp669 = fTemp667 - fTemp660;
			double fTemp670 = fSlow12 * std::fabs(fTemp669) + 1.0;
			double fTemp671 = std::max<double>(-1.0, std::min<double>(1.0, fTemp660 - fSlow4 * (fTemp666 * (fSlow9 * fTemp668 + fTemp669 / (fTemp670 * (fSlow13 * ((-(fSlow4 * fTemp666) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp669 / fTemp670))))) / (1.000000001 - fSlow14 * fTemp668))));
			double fTemp672 = fTemp662 + 0.2617993877991494;
			double fTemp673 = ((fTemp672 >= 6.283185307179586) ? fTemp662 + -6.021385919380437 : fTemp672);
			double fTemp674 = 0.15915494309189535 * (fTemp673 + 0.1308996938995747);
			double fTemp675 = fTemp674 - std::floor(fTemp674);
			double fTemp676 = std::sin(6.283185307179586 * fTemp675) + fSlow5 * std::sin(12.566370614359172 * fTemp675);
			double fTemp677 = fTemp665 - fTemp676;
			double fTemp678 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp676 + fSlow11 * fTemp671));
			double fTemp679 = 1.0 - mydsp_faustpower2_f(fTemp678);
			double fTemp680 = fTemp678 - fTemp671;
			double fTemp681 = fSlow12 * std::fabs(fTemp680) + 1.0;
			double fTemp682 = std::max<double>(-1.0, std::min<double>(1.0, fTemp671 - fSlow4 * (fTemp677 * (fSlow9 * fTemp679 + fTemp680 / (fTemp681 * (fSlow13 * ((-(fSlow4 * fTemp677) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp680 / fTemp681))))) / (1.000000001 - fSlow14 * fTemp679))));
			double fTemp683 = fTemp673 + 0.2617993877991494;
			double fTemp684 = ((fTemp683 >= 6.283185307179586) ? fTemp673 + -6.021385919380437 : fTemp683);
			double fTemp685 = 0.15915494309189535 * (fTemp684 + 0.1308996938995747);
			double fTemp686 = fTemp685 - std::floor(fTemp685);
			double fTemp687 = std::sin(6.283185307179586 * fTemp686) + fSlow5 * std::sin(12.566370614359172 * fTemp686);
			double fTemp688 = fTemp676 - fTemp687;
			double fTemp689 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp687 + fSlow11 * fTemp682));
			double fTemp690 = 1.0 - mydsp_faustpower2_f(fTemp689);
			double fTemp691 = fTemp689 - fTemp682;
			double fTemp692 = fSlow12 * std::fabs(fTemp691) + 1.0;
			double fTemp693 = std::max<double>(-1.0, std::min<double>(1.0, fTemp682 - fSlow4 * (fTemp688 * (fSlow9 * fTemp690 + fTemp691 / (fTemp692 * (fSlow13 * ((-(fSlow4 * fTemp688) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp691 / fTemp692))))) / (1.000000001 - fSlow14 * fTemp690))));
			double fTemp694 = fTemp684 + 0.2617993877991494;
			double fTemp695 = ((fTemp694 >= 6.283185307179586) ? fTemp684 + -6.021385919380437 : fTemp694);
			double fTemp696 = 0.15915494309189535 * (fTemp695 + 0.1308996938995747);
			double fTemp697 = fTemp696 - std::floor(fTemp696);
			double fTemp698 = std::sin(6.283185307179586 * fTemp697) + fSlow5 * std::sin(12.566370614359172 * fTemp697);
			double fTemp699 = fTemp687 - fTemp698;
			double fTemp700 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp698 + fSlow11 * fTemp693));
			double fTemp701 = 1.0 - mydsp_faustpower2_f(fTemp700);
			double fTemp702 = fTemp700 - fTemp693;
			double fTemp703 = fSlow12 * std::fabs(fTemp702) + 1.0;
			double fTemp704 = std::max<double>(-1.0, std::min<double>(1.0, fTemp693 - fSlow4 * (fTemp699 * (fSlow9 * fTemp701 + fTemp702 / (fTemp703 * (fSlow13 * ((-(fSlow4 * fTemp699) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp702 / fTemp703))))) / (1.000000001 - fSlow14 * fTemp701))));
			double fTemp705 = fTemp695 + 0.2617993877991494;
			double fTemp706 = ((fTemp705 >= 6.283185307179586) ? fTemp695 + -6.021385919380437 : fTemp705);
			double fTemp707 = 0.15915494309189535 * (fTemp706 + 0.1308996938995747);
			double fTemp708 = fTemp707 - std::floor(fTemp707);
			double fTemp709 = std::sin(6.283185307179586 * fTemp708) + fSlow5 * std::sin(12.566370614359172 * fTemp708);
			double fTemp710 = fTemp698 - fTemp709;
			double fTemp711 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp709 + fSlow11 * fTemp704));
			double fTemp712 = 1.0 - mydsp_faustpower2_f(fTemp711);
			double fTemp713 = fTemp711 - fTemp704;
			double fTemp714 = fSlow12 * std::fabs(fTemp713) + 1.0;
			double fTemp715 = std::max<double>(-1.0, std::min<double>(1.0, fTemp704 - fSlow4 * (fTemp710 * (fSlow9 * fTemp712 + fTemp713 / (fTemp714 * (fSlow13 * ((-(fSlow4 * fTemp710) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp713 / fTemp714))))) / (1.000000001 - fSlow14 * fTemp712))));
			double fTemp716 = fTemp706 + 0.2617993877991494;
			double fTemp717 = ((fTemp716 >= 6.283185307179586) ? fTemp706 + -6.021385919380437 : fTemp716);
			double fTemp718 = 0.15915494309189535 * (fTemp717 + 0.1308996938995747);
			double fTemp719 = fTemp718 - std::floor(fTemp718);
			double fTemp720 = std::sin(6.283185307179586 * fTemp719) + fSlow5 * std::sin(12.566370614359172 * fTemp719);
			double fTemp721 = fTemp709 - fTemp720;
			double fTemp722 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp720 + fSlow11 * fTemp715));
			double fTemp723 = 1.0 - mydsp_faustpower2_f(fTemp722);
			double fTemp724 = fTemp722 - fTemp715;
			double fTemp725 = fSlow12 * std::fabs(fTemp724) + 1.0;
			double fTemp726 = std::max<double>(-1.0, std::min<double>(1.0, fTemp715 - fSlow4 * (fTemp721 * (fSlow9 * fTemp723 + fTemp724 / (fTemp725 * (fSlow13 * ((-(fSlow4 * fTemp721) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp724 / fTemp725))))) / (1.000000001 - fSlow14 * fTemp723))));
			double fTemp727 = fTemp717 + 0.2617993877991494;
			double fTemp728 = ((fTemp727 >= 6.283185307179586) ? fTemp717 + -6.021385919380437 : fTemp727);
			double fTemp729 = 0.15915494309189535 * (fTemp728 + 0.1308996938995747);
			double fTemp730 = fTemp729 - std::floor(fTemp729);
			double fTemp731 = std::sin(6.283185307179586 * fTemp730) + fSlow5 * std::sin(12.566370614359172 * fTemp730);
			double fTemp732 = fTemp720 - fTemp731;
			double fTemp733 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp731 + fSlow11 * fTemp726));
			double fTemp734 = 1.0 - mydsp_faustpower2_f(fTemp733);
			double fTemp735 = fTemp733 - fTemp726;
			double fTemp736 = fSlow12 * std::fabs(fTemp735) + 1.0;
			double fTemp737 = std::max<double>(-1.0, std::min<double>(1.0, fTemp726 - fSlow4 * (fTemp732 * (fSlow9 * fTemp734 + fTemp735 / (fTemp736 * (fSlow13 * ((-(fSlow4 * fTemp732) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp735 / fTemp736))))) / (1.000000001 - fSlow14 * fTemp734))));
			double fTemp738 = fTemp728 + 0.2617993877991494;
			double fTemp739 = ((fTemp738 >= 6.283185307179586) ? fTemp728 + -6.021385919380437 : fTemp738);
			double fTemp740 = 0.15915494309189535 * (fTemp739 + 0.1308996938995747);
			double fTemp741 = fTemp740 - std::floor(fTemp740);
			double fTemp742 = std::sin(6.283185307179586 * fTemp741) + fSlow5 * std::sin(12.566370614359172 * fTemp741);
			double fTemp743 = fTemp731 - fTemp742;
			double fTemp744 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp742 + fSlow11 * fTemp737));
			double fTemp745 = 1.0 - mydsp_faustpower2_f(fTemp744);
			double fTemp746 = fTemp744 - fTemp737;
			double fTemp747 = fSlow12 * std::fabs(fTemp746) + 1.0;
			double fTemp748 = std::max<double>(-1.0, std::min<double>(1.0, fTemp737 - fSlow4 * (fTemp743 * (fSlow9 * fTemp745 + fTemp746 / (fTemp747 * (fSlow13 * ((-(fSlow4 * fTemp743) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp746 / fTemp747))))) / (1.000000001 - fSlow14 * fTemp745))));
			double fTemp749 = fTemp739 + 0.2617993877991494;
			double fTemp750 = ((fTemp749 >= 6.283185307179586) ? fTemp739 + -6.021385919380437 : fTemp749);
			double fTemp751 = 0.15915494309189535 * (fTemp750 + 0.1308996938995747);
			double fTemp752 = fTemp751 - std::floor(fTemp751);
			double fTemp753 = std::sin(6.283185307179586 * fTemp752) + fSlow5 * std::sin(12.566370614359172 * fTemp752);
			double fTemp754 = fTemp742 - fTemp753;
			double fTemp755 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp753 + fSlow11 * fTemp748));
			double fTemp756 = 1.0 - mydsp_faustpower2_f(fTemp755);
			double fTemp757 = fTemp755 - fTemp748;
			double fTemp758 = fSlow12 * std::fabs(fTemp757) + 1.0;
			double fTemp759 = std::max<double>(-1.0, std::min<double>(1.0, fTemp748 - fSlow4 * (fTemp754 * (fSlow9 * fTemp756 + fTemp757 / (fTemp758 * (fSlow13 * ((-(fSlow4 * fTemp754) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp757 / fTemp758))))) / (1.000000001 - fSlow14 * fTemp756))));
			double fTemp760 = fTemp750 + 0.2617993877991494;
			double fTemp761 = ((fTemp760 >= 6.283185307179586) ? fTemp750 + -6.021385919380437 : fTemp760);
			double fTemp762 = 0.15915494309189535 * (fTemp761 + 0.1308996938995747);
			double fTemp763 = fTemp762 - std::floor(fTemp762);
			double fTemp764 = std::sin(6.283185307179586 * fTemp763) + fSlow5 * std::sin(12.566370614359172 * fTemp763);
			double fTemp765 = fTemp753 - fTemp764;
			double fTemp766 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp764 + fSlow11 * fTemp759));
			double fTemp767 = 1.0 - mydsp_faustpower2_f(fTemp766);
			double fTemp768 = fTemp766 - fTemp759;
			double fTemp769 = fSlow12 * std::fabs(fTemp768) + 1.0;
			double fTemp770 = std::max<double>(-1.0, std::min<double>(1.0, fTemp759 - fSlow4 * (fTemp765 * (fSlow9 * fTemp767 + fTemp768 / (fTemp769 * (fSlow13 * ((-(fSlow4 * fTemp765) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp768 / fTemp769))))) / (1.000000001 - fSlow14 * fTemp767))));
			double fTemp771 = fTemp761 + 0.2617993877991494;
			double fTemp772 = ((fTemp771 >= 6.283185307179586) ? fTemp761 + -6.021385919380437 : fTemp771);
			double fTemp773 = 0.15915494309189535 * (fTemp772 + 0.1308996938995747);
			double fTemp774 = fTemp773 - std::floor(fTemp773);
			double fTemp775 = std::sin(6.283185307179586 * fTemp774) + fSlow5 * std::sin(12.566370614359172 * fTemp774);
			double fTemp776 = fTemp764 - fTemp775;
			double fTemp777 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp775 + fSlow11 * fTemp770));
			double fTemp778 = 1.0 - mydsp_faustpower2_f(fTemp777);
			double fTemp779 = fTemp777 - fTemp770;
			double fTemp780 = fSlow12 * std::fabs(fTemp779) + 1.0;
			double fTemp781 = std::max<double>(-1.0, std::min<double>(1.0, fTemp770 - fSlow4 * (fTemp776 * (fSlow9 * fTemp778 + fTemp779 / (fTemp780 * (fSlow13 * ((-(fSlow4 * fTemp776) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp779 / fTemp780))))) / (1.000000001 - fSlow14 * fTemp778))));
			double fTemp782 = fTemp772 + 0.2617993877991494;
			double fTemp783 = ((fTemp782 >= 6.283185307179586) ? fTemp772 + -6.021385919380437 : fTemp782);
			double fTemp784 = 0.15915494309189535 * (fTemp783 + 0.1308996938995747);
			double fTemp785 = fTemp784 - std::floor(fTemp784);
			double fTemp786 = std::sin(6.283185307179586 * fTemp785) + fSlow5 * std::sin(12.566370614359172 * fTemp785);
			double fTemp787 = fTemp775 - fTemp786;
			double fTemp788 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp786 + fSlow11 * fTemp781));
			double fTemp789 = 1.0 - mydsp_faustpower2_f(fTemp788);
			double fTemp790 = fTemp788 - fTemp781;
			double fTemp791 = fSlow12 * std::fabs(fTemp790) + 1.0;
			double fTemp792 = std::max<double>(-1.0, std::min<double>(1.0, fTemp781 - fSlow4 * (fTemp787 * (fSlow9 * fTemp789 + fTemp790 / (fTemp791 * (fSlow13 * ((-(fSlow4 * fTemp787) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp790 / fTemp791))))) / (1.000000001 - fSlow14 * fTemp789))));
			double fTemp793 = fTemp783 + 0.2617993877991494;
			double fTemp794 = ((fTemp793 >= 6.283185307179586) ? fTemp783 + -6.021385919380437 : fTemp793);
			double fTemp795 = 0.15915494309189535 * (fTemp794 + 0.1308996938995747);
			double fTemp796 = fTemp795 - std::floor(fTemp795);
			double fTemp797 = std::sin(6.283185307179586 * fTemp796) + fSlow5 * std::sin(12.566370614359172 * fTemp796);
			double fTemp798 = fTemp786 - fTemp797;
			double fTemp799 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp797 + fSlow11 * fTemp792));
			double fTemp800 = 1.0 - mydsp_faustpower2_f(fTemp799);
			double fTemp801 = fTemp799 - fTemp792;
			double fTemp802 = fSlow12 * std::fabs(fTemp801) + 1.0;
			double fTemp803 = std::max<double>(-1.0, std::min<double>(1.0, fTemp792 - fSlow4 * (fTemp798 * (fSlow9 * fTemp800 + fTemp801 / (fTemp802 * (fSlow13 * ((-(fSlow4 * fTemp798) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp801 / fTemp802))))) / (1.000000001 - fSlow14 * fTemp800))));
			double fTemp804 = fTemp794 + 0.2617993877991494;
			double fTemp805 = ((fTemp804 >= 6.283185307179586) ? fTemp794 + -6.021385919380437 : fTemp804);
			double fTemp806 = 0.15915494309189535 * (fTemp805 + 0.1308996938995747);
			double fTemp807 = fTemp806 - std::floor(fTemp806);
			double fTemp808 = std::sin(6.283185307179586 * fTemp807) + fSlow5 * std::sin(12.566370614359172 * fTemp807);
			double fTemp809 = fTemp797 - fTemp808;
			double fTemp810 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp808 + fSlow11 * fTemp803));
			double fTemp811 = 1.0 - mydsp_faustpower2_f(fTemp810);
			double fTemp812 = fTemp810 - fTemp803;
			double fTemp813 = fSlow12 * std::fabs(fTemp812) + 1.0;
			double fTemp814 = std::max<double>(-1.0, std::min<double>(1.0, fTemp803 - fSlow4 * (fTemp809 * (fSlow9 * fTemp811 + fTemp812 / (fTemp813 * (fSlow13 * ((-(fSlow4 * fTemp809) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp812 / fTemp813))))) / (1.000000001 - fSlow14 * fTemp811))));
			double fTemp815 = fTemp805 + 0.2617993877991494;
			double fTemp816 = ((fTemp815 >= 6.283185307179586) ? fTemp805 + -6.021385919380437 : fTemp815);
			double fTemp817 = 0.15915494309189535 * (fTemp816 + 0.1308996938995747);
			double fTemp818 = fTemp817 - std::floor(fTemp817);
			double fTemp819 = std::sin(6.283185307179586 * fTemp818) + fSlow5 * std::sin(12.566370614359172 * fTemp818);
			double fTemp820 = fTemp808 - fTemp819;
			double fTemp821 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp819 + fSlow11 * fTemp814));
			double fTemp822 = 1.0 - mydsp_faustpower2_f(fTemp821);
			double fTemp823 = fTemp821 - fTemp814;
			double fTemp824 = fSlow12 * std::fabs(fTemp823) + 1.0;
			double fTemp825 = std::max<double>(-1.0, std::min<double>(1.0, fTemp814 - fSlow4 * (fTemp820 * (fSlow9 * fTemp822 + fTemp823 / (fTemp824 * (fSlow13 * ((-(fSlow4 * fTemp820) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp823 / fTemp824))))) / (1.000000001 - fSlow14 * fTemp822))));
			double fTemp826 = fTemp816 + 0.2617993877991494;
			double fTemp827 = ((fTemp826 >= 6.283185307179586) ? fTemp816 + -6.021385919380437 : fTemp826);
			double fTemp828 = 0.15915494309189535 * (fTemp827 + 0.1308996938995747);
			double fTemp829 = fTemp828 - std::floor(fTemp828);
			double fTemp830 = std::sin(6.283185307179586 * fTemp829) + fSlow5 * std::sin(12.566370614359172 * fTemp829);
			double fTemp831 = fTemp819 - fTemp830;
			double fTemp832 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp830 + fSlow11 * fTemp825));
			double fTemp833 = 1.0 - mydsp_faustpower2_f(fTemp832);
			double fTemp834 = fTemp832 - fTemp825;
			double fTemp835 = fSlow12 * std::fabs(fTemp834) + 1.0;
			double fTemp836 = std::max<double>(-1.0, std::min<double>(1.0, fTemp825 - fSlow4 * (fTemp831 * (fSlow9 * fTemp833 + fTemp834 / (fTemp835 * (fSlow13 * ((-(fSlow4 * fTemp831) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp834 / fTemp835))))) / (1.000000001 - fSlow14 * fTemp833))));
			double fTemp837 = fTemp827 + 0.2617993877991494;
			double fTemp838 = ((fTemp837 >= 6.283185307179586) ? fTemp827 + -6.021385919380437 : fTemp837);
			double fTemp839 = 0.15915494309189535 * (fTemp838 + 0.1308996938995747);
			double fTemp840 = fTemp839 - std::floor(fTemp839);
			double fTemp841 = std::sin(6.283185307179586 * fTemp840) + fSlow5 * std::sin(12.566370614359172 * fTemp840);
			double fTemp842 = fTemp830 - fTemp841;
			double fTemp843 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp841 + fSlow11 * fTemp836));
			double fTemp844 = 1.0 - mydsp_faustpower2_f(fTemp843);
			double fTemp845 = fTemp843 - fTemp836;
			double fTemp846 = fSlow12 * std::fabs(fTemp845) + 1.0;
			double fTemp847 = std::max<double>(-1.0, std::min<double>(1.0, fTemp836 - fSlow4 * (fTemp842 * (fSlow9 * fTemp844 + fTemp845 / (fTemp846 * (fSlow13 * ((-(fSlow4 * fTemp842) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp845 / fTemp846))))) / (1.000000001 - fSlow14 * fTemp844))));
			double fTemp848 = fTemp838 + 0.2617993877991494;
			double fTemp849 = ((fTemp848 >= 6.283185307179586) ? fTemp838 + -6.021385919380437 : fTemp848);
			double fTemp850 = 0.15915494309189535 * (fTemp849 + 0.1308996938995747);
			double fTemp851 = fTemp850 - std::floor(fTemp850);
			double fTemp852 = std::sin(6.283185307179586 * fTemp851) + fSlow5 * std::sin(12.566370614359172 * fTemp851);
			double fTemp853 = fTemp841 - fTemp852;
			double fTemp854 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp852 + fSlow11 * fTemp847));
			double fTemp855 = 1.0 - mydsp_faustpower2_f(fTemp854);
			double fTemp856 = fTemp854 - fTemp847;
			double fTemp857 = fSlow12 * std::fabs(fTemp856) + 1.0;
			double fTemp858 = std::max<double>(-1.0, std::min<double>(1.0, fTemp847 - fSlow4 * (fTemp853 * (fSlow9 * fTemp855 + fTemp856 / (fTemp857 * (fSlow13 * ((-(fSlow4 * fTemp853) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp856 / fTemp857))))) / (1.000000001 - fSlow14 * fTemp855))));
			double fTemp859 = fTemp849 + 0.2617993877991494;
			double fTemp860 = ((fTemp859 >= 6.283185307179586) ? fTemp849 + -6.021385919380437 : fTemp859);
			double fTemp861 = 0.15915494309189535 * (fTemp860 + 0.1308996938995747);
			double fTemp862 = fTemp861 - std::floor(fTemp861);
			double fTemp863 = std::sin(6.283185307179586 * fTemp862) + fSlow5 * std::sin(12.566370614359172 * fTemp862);
			double fTemp864 = fTemp852 - fTemp863;
			double fTemp865 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp863 + fSlow11 * fTemp858));
			double fTemp866 = 1.0 - mydsp_faustpower2_f(fTemp865);
			double fTemp867 = fTemp865 - fTemp858;
			double fTemp868 = fSlow12 * std::fabs(fTemp867) + 1.0;
			double fTemp869 = std::max<double>(-1.0, std::min<double>(1.0, fTemp858 - fSlow4 * (fTemp864 * (fSlow9 * fTemp866 + fTemp867 / (fTemp868 * (fSlow13 * ((-(fSlow4 * fTemp864) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp867 / fTemp868))))) / (1.000000001 - fSlow14 * fTemp866))));
			double fTemp870 = fTemp860 + 0.2617993877991494;
			double fTemp871 = ((fTemp870 >= 6.283185307179586) ? fTemp860 + -6.021385919380437 : fTemp870);
			double fTemp872 = 0.15915494309189535 * (fTemp871 + 0.1308996938995747);
			double fTemp873 = fTemp872 - std::floor(fTemp872);
			double fTemp874 = std::sin(6.283185307179586 * fTemp873) + fSlow5 * std::sin(12.566370614359172 * fTemp873);
			double fTemp875 = fTemp863 - fTemp874;
			double fTemp876 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp874 + fSlow11 * fTemp869));
			double fTemp877 = 1.0 - mydsp_faustpower2_f(fTemp876);
			double fTemp878 = fTemp876 - fTemp869;
			double fTemp879 = fSlow12 * std::fabs(fTemp878) + 1.0;
			double fTemp880 = std::max<double>(-1.0, std::min<double>(1.0, fTemp869 - fSlow4 * (fTemp875 * (fSlow9 * fTemp877 + fTemp878 / (fTemp879 * (fSlow13 * ((-(fSlow4 * fTemp875) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp878 / fTemp879))))) / (1.000000001 - fSlow14 * fTemp877))));
			double fTemp881 = fTemp871 + 0.2617993877991494;
			double fTemp882 = ((fTemp881 >= 6.283185307179586) ? fTemp871 + -6.021385919380437 : fTemp881);
			double fTemp883 = 0.15915494309189535 * (fTemp882 + 0.1308996938995747);
			double fTemp884 = fTemp883 - std::floor(fTemp883);
			double fTemp885 = std::sin(6.283185307179586 * fTemp884) + fSlow5 * std::sin(12.566370614359172 * fTemp884);
			double fTemp886 = fTemp874 - fTemp885;
			double fTemp887 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp885 + fSlow11 * fTemp880));
			double fTemp888 = 1.0 - mydsp_faustpower2_f(fTemp887);
			double fTemp889 = fTemp887 - fTemp880;
			double fTemp890 = fSlow12 * std::fabs(fTemp889) + 1.0;
			double fTemp891 = std::max<double>(-1.0, std::min<double>(1.0, fTemp880 - fSlow4 * (fTemp886 * (fSlow9 * fTemp888 + fTemp889 / (fTemp890 * (fSlow13 * ((-(fSlow4 * fTemp886) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp889 / fTemp890))))) / (1.000000001 - fSlow14 * fTemp888))));
			double fTemp892 = fTemp882 + 0.2617993877991494;
			double fTemp893 = ((fTemp892 >= 6.283185307179586) ? fTemp882 + -6.021385919380437 : fTemp892);
			double fTemp894 = 0.15915494309189535 * (fTemp893 + 0.1308996938995747);
			double fTemp895 = fTemp894 - std::floor(fTemp894);
			double fTemp896 = std::sin(6.283185307179586 * fTemp895) + fSlow5 * std::sin(12.566370614359172 * fTemp895);
			double fTemp897 = fTemp885 - fTemp896;
			double fTemp898 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp896 + fSlow11 * fTemp891));
			double fTemp899 = 1.0 - mydsp_faustpower2_f(fTemp898);
			double fTemp900 = fTemp898 - fTemp891;
			double fTemp901 = fSlow12 * std::fabs(fTemp900) + 1.0;
			double fTemp902 = std::max<double>(-1.0, std::min<double>(1.0, fTemp891 - fSlow4 * (fTemp897 * (fSlow9 * fTemp899 + fTemp900 / (fTemp901 * (fSlow13 * ((-(fSlow4 * fTemp897) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp900 / fTemp901))))) / (1.000000001 - fSlow14 * fTemp899))));
			double fTemp903 = fTemp893 + 0.2617993877991494;
			double fTemp904 = ((fTemp903 >= 6.283185307179586) ? fTemp893 + -6.021385919380437 : fTemp903);
			double fTemp905 = 0.15915494309189535 * (fTemp904 + 0.1308996938995747);
			double fTemp906 = fTemp905 - std::floor(fTemp905);
			double fTemp907 = std::sin(6.283185307179586 * fTemp906) + fSlow5 * std::sin(12.566370614359172 * fTemp906);
			double fTemp908 = fTemp896 - fTemp907;
			double fTemp909 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp907 + fSlow11 * fTemp902));
			double fTemp910 = 1.0 - mydsp_faustpower2_f(fTemp909);
			double fTemp911 = fTemp909 - fTemp902;
			double fTemp912 = fSlow12 * std::fabs(fTemp911) + 1.0;
			double fTemp913 = std::max<double>(-1.0, std::min<double>(1.0, fTemp902 - fSlow4 * (fTemp908 * (fSlow9 * fTemp910 + fTemp911 / (fTemp912 * (fSlow13 * ((-(fSlow4 * fTemp908) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp911 / fTemp912))))) / (1.000000001 - fSlow14 * fTemp910))));
			double fTemp914 = fTemp904 + 0.2617993877991494;
			double fTemp915 = ((fTemp914 >= 6.283185307179586) ? fTemp904 + -6.021385919380437 : fTemp914);
			double fTemp916 = 0.15915494309189535 * (fTemp915 + 0.1308996938995747);
			double fTemp917 = fTemp916 - std::floor(fTemp916);
			double fTemp918 = std::sin(6.283185307179586 * fTemp917) + fSlow5 * std::sin(12.566370614359172 * fTemp917);
			double fTemp919 = fTemp907 - fTemp918;
			double fTemp920 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp918 + fSlow11 * fTemp913));
			double fTemp921 = 1.0 - mydsp_faustpower2_f(fTemp920);
			double fTemp922 = fTemp920 - fTemp913;
			double fTemp923 = fSlow12 * std::fabs(fTemp922) + 1.0;
			double fTemp924 = std::max<double>(-1.0, std::min<double>(1.0, fTemp913 - fSlow4 * (fTemp919 * (fSlow9 * fTemp921 + fTemp922 / (fTemp923 * (fSlow13 * ((-(fSlow4 * fTemp919) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp922 / fTemp923))))) / (1.000000001 - fSlow14 * fTemp921))));
			double fTemp925 = fTemp915 + 0.2617993877991494;
			double fTemp926 = ((fTemp925 >= 6.283185307179586) ? fTemp915 + -6.021385919380437 : fTemp925);
			double fTemp927 = 0.15915494309189535 * (fTemp926 + 0.1308996938995747);
			double fTemp928 = fTemp927 - std::floor(fTemp927);
			double fTemp929 = std::sin(6.283185307179586 * fTemp928) + fSlow5 * std::sin(12.566370614359172 * fTemp928);
			double fTemp930 = fTemp918 - fTemp929;
			double fTemp931 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp929 + fSlow11 * fTemp924));
			double fTemp932 = 1.0 - mydsp_faustpower2_f(fTemp931);
			double fTemp933 = fTemp931 - fTemp924;
			double fTemp934 = fSlow12 * std::fabs(fTemp933) + 1.0;
			double fTemp935 = std::max<double>(-1.0, std::min<double>(1.0, fTemp924 - fSlow4 * (fTemp930 * (fSlow9 * fTemp932 + fTemp933 / (fTemp934 * (fSlow13 * ((-(fSlow4 * fTemp930) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp933 / fTemp934))))) / (1.000000001 - fSlow14 * fTemp932))));
			double fTemp936 = fTemp926 + 0.2617993877991494;
			double fTemp937 = ((fTemp936 >= 6.283185307179586) ? fTemp926 + -6.021385919380437 : fTemp936);
			double fTemp938 = 0.15915494309189535 * (fTemp937 + 0.1308996938995747);
			double fTemp939 = fTemp938 - std::floor(fTemp938);
			double fTemp940 = std::sin(6.283185307179586 * fTemp939) + fSlow5 * std::sin(12.566370614359172 * fTemp939);
			double fTemp941 = fTemp929 - fTemp940;
			double fTemp942 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp940 + fSlow11 * fTemp935));
			double fTemp943 = 1.0 - mydsp_faustpower2_f(fTemp942);
			double fTemp944 = fTemp942 - fTemp935;
			double fTemp945 = fSlow12 * std::fabs(fTemp944) + 1.0;
			double fTemp946 = std::max<double>(-1.0, std::min<double>(1.0, fTemp935 - fSlow4 * (fTemp941 * (fSlow9 * fTemp943 + fTemp944 / (fTemp945 * (fSlow13 * ((-(fSlow4 * fTemp941) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp944 / fTemp945))))) / (1.000000001 - fSlow14 * fTemp943))));
			double fTemp947 = fTemp937 + 0.2617993877991494;
			double fTemp948 = ((fTemp947 >= 6.283185307179586) ? fTemp937 + -6.021385919380437 : fTemp947);
			double fTemp949 = 0.15915494309189535 * (fTemp948 + 0.1308996938995747);
			double fTemp950 = fTemp949 - std::floor(fTemp949);
			double fTemp951 = std::sin(6.283185307179586 * fTemp950) + fSlow5 * std::sin(12.566370614359172 * fTemp950);
			double fTemp952 = fTemp940 - fTemp951;
			double fTemp953 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp951 + fSlow11 * fTemp946));
			double fTemp954 = 1.0 - mydsp_faustpower2_f(fTemp953);
			double fTemp955 = fTemp953 - fTemp946;
			double fTemp956 = fSlow12 * std::fabs(fTemp955) + 1.0;
			double fTemp957 = std::max<double>(-1.0, std::min<double>(1.0, fTemp946 - fSlow4 * (fTemp952 * (fSlow9 * fTemp954 + fTemp955 / (fTemp956 * (fSlow13 * ((-(fSlow4 * fTemp952) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp955 / fTemp956))))) / (1.000000001 - fSlow14 * fTemp954))));
			double fTemp958 = fTemp948 + 0.2617993877991494;
			double fTemp959 = ((fTemp958 >= 6.283185307179586) ? fTemp948 + -6.021385919380437 : fTemp958);
			double fTemp960 = 0.15915494309189535 * (fTemp959 + 0.1308996938995747);
			double fTemp961 = fTemp960 - std::floor(fTemp960);
			double fTemp962 = std::sin(6.283185307179586 * fTemp961) + fSlow5 * std::sin(12.566370614359172 * fTemp961);
			double fTemp963 = fTemp951 - fTemp962;
			double fTemp964 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp962 + fSlow11 * fTemp957));
			double fTemp965 = 1.0 - mydsp_faustpower2_f(fTemp964);
			double fTemp966 = fTemp964 - fTemp957;
			double fTemp967 = fSlow12 * std::fabs(fTemp966) + 1.0;
			double fTemp968 = std::max<double>(-1.0, std::min<double>(1.0, fTemp957 - fSlow4 * (fTemp963 * (fSlow9 * fTemp965 + fTemp966 / (fTemp967 * (fSlow13 * ((-(fSlow4 * fTemp963) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp966 / fTemp967))))) / (1.000000001 - fSlow14 * fTemp965))));
			double fTemp969 = fTemp959 + 0.2617993877991494;
			double fTemp970 = ((fTemp969 >= 6.283185307179586) ? fTemp959 + -6.021385919380437 : fTemp969);
			double fTemp971 = 0.15915494309189535 * (fTemp970 + 0.1308996938995747);
			double fTemp972 = fTemp971 - std::floor(fTemp971);
			double fTemp973 = std::sin(6.283185307179586 * fTemp972) + fSlow5 * std::sin(12.566370614359172 * fTemp972);
			double fTemp974 = fTemp962 - fTemp973;
			double fTemp975 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp973 + fSlow11 * fTemp968));
			double fTemp976 = 1.0 - mydsp_faustpower2_f(fTemp975);
			double fTemp977 = fTemp975 - fTemp968;
			double fTemp978 = fSlow12 * std::fabs(fTemp977) + 1.0;
			double fTemp979 = std::max<double>(-1.0, std::min<double>(1.0, fTemp968 - fSlow4 * (fTemp974 * (fSlow9 * fTemp976 + fTemp977 / (fTemp978 * (fSlow13 * ((-(fSlow4 * fTemp974) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp977 / fTemp978))))) / (1.000000001 - fSlow14 * fTemp976))));
			double fTemp980 = fTemp970 + 0.2617993877991494;
			double fTemp981 = ((fTemp980 >= 6.283185307179586) ? fTemp970 + -6.021385919380437 : fTemp980);
			double fTemp982 = 0.15915494309189535 * (fTemp981 + 0.1308996938995747);
			double fTemp983 = fTemp982 - std::floor(fTemp982);
			double fTemp984 = std::sin(6.283185307179586 * fTemp983) + fSlow5 * std::sin(12.566370614359172 * fTemp983);
			double fTemp985 = fTemp973 - fTemp984;
			double fTemp986 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp984 + fSlow11 * fTemp979));
			double fTemp987 = 1.0 - mydsp_faustpower2_f(fTemp986);
			double fTemp988 = fTemp986 - fTemp979;
			double fTemp989 = fSlow12 * std::fabs(fTemp988) + 1.0;
			double fTemp990 = std::max<double>(-1.0, std::min<double>(1.0, fTemp979 - fSlow4 * (fTemp985 * (fSlow9 * fTemp987 + fTemp988 / (fTemp989 * (fSlow13 * ((-(fSlow4 * fTemp985) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp988 / fTemp989))))) / (1.000000001 - fSlow14 * fTemp987))));
			double fTemp991 = fTemp981 + 0.2617993877991494;
			double fTemp992 = ((fTemp991 >= 6.283185307179586) ? fTemp981 + -6.021385919380437 : fTemp991);
			double fTemp993 = 0.15915494309189535 * (fTemp992 + 0.1308996938995747);
			double fTemp994 = fTemp993 - std::floor(fTemp993);
			double fTemp995 = std::sin(6.283185307179586 * fTemp994) + fSlow5 * std::sin(12.566370614359172 * fTemp994);
			double fTemp996 = fTemp984 - fTemp995;
			double fTemp997 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp995 + fSlow11 * fTemp990));
			double fTemp998 = 1.0 - mydsp_faustpower2_f(fTemp997);
			double fTemp999 = fTemp997 - fTemp990;
			double fTemp1000 = fSlow12 * std::fabs(fTemp999) + 1.0;
			double fTemp1001 = std::max<double>(-1.0, std::min<double>(1.0, fTemp990 - fSlow4 * (fTemp996 * (fSlow9 * fTemp998 + fTemp999 / (fTemp1000 * (fSlow13 * ((-(fSlow4 * fTemp996) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp999 / fTemp1000))))) / (1.000000001 - fSlow14 * fTemp998))));
			double fTemp1002 = fTemp992 + 0.2617993877991494;
			double fTemp1003 = ((fTemp1002 >= 6.283185307179586) ? fTemp992 + -6.021385919380437 : fTemp1002);
			double fTemp1004 = 0.15915494309189535 * (fTemp1003 + 0.1308996938995747);
			double fTemp1005 = fTemp1004 - std::floor(fTemp1004);
			double fTemp1006 = std::sin(6.283185307179586 * fTemp1005) + fSlow5 * std::sin(12.566370614359172 * fTemp1005);
			double fTemp1007 = fTemp995 - fTemp1006;
			double fTemp1008 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp1006 + fSlow11 * fTemp1001));
			double fTemp1009 = 1.0 - mydsp_faustpower2_f(fTemp1008);
			double fTemp1010 = fTemp1008 - fTemp1001;
			double fTemp1011 = fSlow12 * std::fabs(fTemp1010) + 1.0;
			double fTemp1012 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1001 - fSlow4 * (fTemp1007 * (fSlow9 * fTemp1009 + fTemp1010 / (fTemp1011 * (fSlow13 * ((-(fSlow4 * fTemp1007) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1010 / fTemp1011))))) / (1.000000001 - fSlow14 * fTemp1009))));
			double fTemp1013 = fTemp1003 + 0.2617993877991494;
			double fTemp1014 = ((fTemp1013 >= 6.283185307179586) ? fTemp1003 + -6.021385919380437 : fTemp1013);
			double fTemp1015 = 0.15915494309189535 * (fTemp1014 + 0.1308996938995747);
			double fTemp1016 = fTemp1015 - std::floor(fTemp1015);
			double fTemp1017 = std::sin(6.283185307179586 * fTemp1016) + fSlow5 * std::sin(12.566370614359172 * fTemp1016);
			double fTemp1018 = fTemp1006 - fTemp1017;
			double fTemp1019 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp1017 + fSlow11 * fTemp1012));
			double fTemp1020 = 1.0 - mydsp_faustpower2_f(fTemp1019);
			double fTemp1021 = fTemp1019 - fTemp1012;
			double fTemp1022 = fSlow12 * std::fabs(fTemp1021) + 1.0;
			double fTemp1023 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1012 - fSlow4 * (fTemp1018 * (fSlow9 * fTemp1020 + fTemp1021 / (fTemp1022 * (fSlow13 * ((-(fSlow4 * fTemp1018) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1021 / fTemp1022))))) / (1.000000001 - fSlow14 * fTemp1020))));
			double fTemp1024 = fTemp1014 + 0.2617993877991494;
			double fTemp1025 = ((fTemp1024 >= 6.283185307179586) ? fTemp1014 + -6.021385919380437 : fTemp1024);
			double fTemp1026 = 0.15915494309189535 * (fTemp1025 + 0.1308996938995747);
			double fTemp1027 = fTemp1026 - std::floor(fTemp1026);
			double fTemp1028 = std::sin(6.283185307179586 * fTemp1027) + fSlow5 * std::sin(12.566370614359172 * fTemp1027);
			double fTemp1029 = fTemp1017 - fTemp1028;
			double fTemp1030 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp1028 + fSlow11 * fTemp1023));
			double fTemp1031 = 1.0 - mydsp_faustpower2_f(fTemp1030);
			double fTemp1032 = fTemp1030 - fTemp1023;
			double fTemp1033 = fSlow12 * std::fabs(fTemp1032) + 1.0;
			double fTemp1034 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1023 - fSlow4 * (fTemp1029 * (fSlow9 * fTemp1031 + fTemp1032 / (fTemp1033 * (fSlow13 * ((-(fSlow4 * fTemp1029) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1032 / fTemp1033))))) / (1.000000001 - fSlow14 * fTemp1031))));
			double fTemp1035 = fTemp1025 + 0.2617993877991494;
			double fTemp1036 = ((fTemp1035 >= 6.283185307179586) ? fTemp1025 + -6.021385919380437 : fTemp1035);
			double fTemp1037 = 0.15915494309189535 * (fTemp1036 + 0.1308996938995747);
			double fTemp1038 = fTemp1037 - std::floor(fTemp1037);
			double fTemp1039 = std::sin(6.283185307179586 * fTemp1038) + fSlow5 * std::sin(12.566370614359172 * fTemp1038);
			double fTemp1040 = fTemp1028 - fTemp1039;
			double fTemp1041 = tanh(fSlow10 * (fTemp1 + fSlow4 * fTemp1039 + fSlow11 * fTemp1034));
			double fTemp1042 = 1.0 - mydsp_faustpower2_f(fTemp1041);
			double fTemp1043 = fTemp1041 - fTemp1034;
			double fTemp1044 = fSlow12 * std::fabs(fTemp1043) + 1.0;
			double fTemp1045 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1034 - fSlow4 * (fTemp1040 * (fSlow9 * fTemp1042 + fTemp1043 / (fTemp1044 * (fSlow13 * ((-(fSlow4 * fTemp1040) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1043 / fTemp1044))))) / (1.000000001 - fSlow14 * fTemp1042))));
			double fTemp1046 = fTemp1036 + 0.2617993877991494;
			double fTemp1047 = ((fTemp1046 >= 6.283185307179586) ? fTemp1036 + -6.021385919380437 : fTemp1046);
			double fTemp1048 = 0.15915494309189535 * (fTemp1047 + 0.1308996938995747);
			double fTemp1049 = fTemp1048 - std::floor(fTemp1048);
			double fTemp1050 = std::sin(6.283185307179586 * fTemp1049) + fSlow5 * std::sin(12.566370614359172 * fTemp1049);
			double fTemp1051 = fTemp1039 - fTemp1050;
			double fTemp1052 = fTemp1 + fSlow4 * fTemp1050;
			double fTemp1053 = tanh(fSlow10 * (fTemp1052 + fSlow11 * fTemp1045));
			double fTemp1054 = 1.0 - mydsp_faustpower2_f(fTemp1053);
			double fTemp1055 = fTemp1053 - fTemp1045;
			double fTemp1056 = fSlow12 * std::fabs(fTemp1055) + 1.0;
			double fTemp1057 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1045 - fSlow4 * (fTemp1051 * (fSlow9 * fTemp1054 + fTemp1055 / (fTemp1056 * (fSlow13 * ((-(fSlow4 * fTemp1051) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1055 / fTemp1056))))) / (1.000000001 - fSlow14 * fTemp1054))));
			fRec3[0] = fTemp1057;
			fRec4[0] = fTemp1052;
			double fTemp1058 = fTemp1047 + 0.2617993877991494;
			fRec5[0] = ((fTemp1058 >= 6.283185307179586) ? fTemp1047 + -6.021385919380437 : fTemp1058);
			double fRec6 = fTemp1057 + fTemp1045 + fTemp1034 + fTemp1023 + fTemp1012 + fTemp1001 + fTemp990 + fTemp979 + fTemp968 + fTemp957 + fTemp946 + fTemp935 + fTemp924 + fTemp913 + fTemp902 + fTemp891 + fTemp880 + fTemp869 + fTemp858 + fTemp847 + fTemp836 + fTemp825 + fTemp814 + fTemp803 + fTemp792 + fTemp781 + fTemp770 + fTemp759 + fTemp748 + fTemp737 + fTemp726 + fTemp715 + fTemp704 + fTemp693 + fTemp682 + fTemp671 + fTemp660 + fTemp649 + fTemp638 + fTemp627 + fTemp616 + fTemp605 + fTemp594 + fTemp583 + fTemp572 + fTemp561 + fTemp550 + fTemp539 + fTemp528 + fTemp517 + fTemp506 + fTemp495 + fTemp484 + fTemp473 + fTemp462 + fTemp451 + fTemp440 + fTemp429 + fTemp418 + fTemp407 + fTemp396 + fTemp385 + fTemp374 + fTemp363 + fTemp352 + fTemp341 + fTemp330 + fTemp319 + fTemp308 + fTemp297 + fTemp286 + fTemp275 + fTemp264 + fTemp253 + fTemp242 + fTemp231 + fTemp220 + fTemp209 + fTemp198 + fTemp187 + fTemp176 + fTemp165 + fTemp154 + fTemp143 + fTemp132 + fTemp121 + fTemp110 + fTemp99 + fTemp88 + fTemp77 + fTemp66 + fTemp55 + fTemp44 + fTemp33 + fTemp11 + fTemp22;
			double fTemp1059 = 0.010416666666666666 * fRec6;
			double fTemp1060 = fTemp1059 - (fConst2 * fRec0[1] + fRec1[1]);
			fRec0[0] = fRec0[1] + fConst5 * fTemp1060;
			double fTemp1061 = fRec0[1] + fConst4 * fTemp1060;
			fRec1[0] = fRec1[1] + fConst8 * fTemp1061;
			double fTemp1062 = fConst9 * fTemp1060;
			double fRec2 = fTemp1062;
			fRec9[0] = fSlow15 + fConst7 * fRec9[1];
			output0[i0] = static_cast<FAUSTFLOAT>(fSlow1 * (fRec2 * fRec9[0] * fSlow17 / fRec8[0]) + fSlow18 * fTemp0);
			double fTemp1063 = static_cast<double>(input1[i0]);
			double fTemp1064 = fTemp1063 * fRec7[0] * fRec8[0];
			double fTemp1065 = 0.15915494309189535 * (fRec15[2] + 0.1308996938995747);
			double fTemp1066 = fTemp1065 - std::floor(fTemp1065);
			double fTemp1067 = std::sin(6.283185307179586 * fTemp1066) + fSlow5 * std::sin(12.566370614359172 * fTemp1066);
			double fTemp1068 = fTemp1064 + fSlow4 * fTemp1067;
			double fTemp1069 = fTemp1068 - fRec14[2];
			double fTemp1070 = tanh(fSlow10 * (fTemp1068 + fSlow11 * fRec13[2]));
			double fTemp1071 = 1.0 - mydsp_faustpower2_f(fTemp1070);
			double fTemp1072 = fTemp1070 - fRec13[2];
			double fTemp1073 = fSlow12 * std::fabs(fTemp1072) + 1.0;
			double fTemp1074 = std::max<double>(-1.0, std::min<double>(1.0, fRec13[2] + fTemp1069 * (fSlow9 * fTemp1071 + fTemp1072 / (fTemp1073 * (fSlow13 * ((fTemp1069 >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1072 / fTemp1073))))) / (1.000000001 - fSlow14 * fTemp1071)));
			double fTemp1075 = fRec15[2] + 0.2617993877991494;
			double fTemp1076 = ((fTemp1075 >= 6.283185307179586) ? fRec15[2] + -6.021385919380437 : fTemp1075);
			double fTemp1077 = 0.15915494309189535 * (fTemp1076 + 0.1308996938995747);
			double fTemp1078 = fTemp1077 - std::floor(fTemp1077);
			double fTemp1079 = std::sin(6.283185307179586 * fTemp1078) + fSlow5 * std::sin(12.566370614359172 * fTemp1078);
			double fTemp1080 = fTemp1067 - fTemp1079;
			double fTemp1081 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1079 + fSlow11 * fTemp1074));
			double fTemp1082 = 1.0 - mydsp_faustpower2_f(fTemp1081);
			double fTemp1083 = fTemp1081 - fTemp1074;
			double fTemp1084 = fSlow12 * std::fabs(fTemp1083) + 1.0;
			double fTemp1085 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1074 - fSlow4 * (fTemp1080 * (fSlow9 * fTemp1082 + fTemp1083 / (fTemp1084 * (fSlow13 * ((-(fSlow4 * fTemp1080) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1083 / fTemp1084))))) / (1.000000001 - fSlow14 * fTemp1082))));
			double fTemp1086 = fTemp1076 + 0.2617993877991494;
			double fTemp1087 = ((fTemp1086 >= 6.283185307179586) ? fTemp1076 + -6.021385919380437 : fTemp1086);
			double fTemp1088 = 0.15915494309189535 * (fTemp1087 + 0.1308996938995747);
			double fTemp1089 = fTemp1088 - std::floor(fTemp1088);
			double fTemp1090 = std::sin(6.283185307179586 * fTemp1089) + fSlow5 * std::sin(12.566370614359172 * fTemp1089);
			double fTemp1091 = fTemp1079 - fTemp1090;
			double fTemp1092 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1090 + fSlow11 * fTemp1085));
			double fTemp1093 = 1.0 - mydsp_faustpower2_f(fTemp1092);
			double fTemp1094 = fTemp1092 - fTemp1085;
			double fTemp1095 = fSlow12 * std::fabs(fTemp1094) + 1.0;
			double fTemp1096 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1085 - fSlow4 * (fTemp1091 * (fSlow9 * fTemp1093 + fTemp1094 / (fTemp1095 * (fSlow13 * ((-(fSlow4 * fTemp1091) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1094 / fTemp1095))))) / (1.000000001 - fSlow14 * fTemp1093))));
			double fTemp1097 = fTemp1087 + 0.2617993877991494;
			double fTemp1098 = ((fTemp1097 >= 6.283185307179586) ? fTemp1087 + -6.021385919380437 : fTemp1097);
			double fTemp1099 = 0.15915494309189535 * (fTemp1098 + 0.1308996938995747);
			double fTemp1100 = fTemp1099 - std::floor(fTemp1099);
			double fTemp1101 = std::sin(6.283185307179586 * fTemp1100) + fSlow5 * std::sin(12.566370614359172 * fTemp1100);
			double fTemp1102 = fTemp1090 - fTemp1101;
			double fTemp1103 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1101 + fSlow11 * fTemp1096));
			double fTemp1104 = 1.0 - mydsp_faustpower2_f(fTemp1103);
			double fTemp1105 = fTemp1103 - fTemp1096;
			double fTemp1106 = fSlow12 * std::fabs(fTemp1105) + 1.0;
			double fTemp1107 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1096 - fSlow4 * (fTemp1102 * (fSlow9 * fTemp1104 + fTemp1105 / (fTemp1106 * (fSlow13 * ((-(fSlow4 * fTemp1102) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1105 / fTemp1106))))) / (1.000000001 - fSlow14 * fTemp1104))));
			double fTemp1108 = fTemp1098 + 0.2617993877991494;
			double fTemp1109 = ((fTemp1108 >= 6.283185307179586) ? fTemp1098 + -6.021385919380437 : fTemp1108);
			double fTemp1110 = 0.15915494309189535 * (fTemp1109 + 0.1308996938995747);
			double fTemp1111 = fTemp1110 - std::floor(fTemp1110);
			double fTemp1112 = std::sin(6.283185307179586 * fTemp1111) + fSlow5 * std::sin(12.566370614359172 * fTemp1111);
			double fTemp1113 = fTemp1101 - fTemp1112;
			double fTemp1114 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1112 + fSlow11 * fTemp1107));
			double fTemp1115 = 1.0 - mydsp_faustpower2_f(fTemp1114);
			double fTemp1116 = fTemp1114 - fTemp1107;
			double fTemp1117 = fSlow12 * std::fabs(fTemp1116) + 1.0;
			double fTemp1118 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1107 - fSlow4 * (fTemp1113 * (fSlow9 * fTemp1115 + fTemp1116 / (fTemp1117 * (fSlow13 * ((-(fSlow4 * fTemp1113) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1116 / fTemp1117))))) / (1.000000001 - fSlow14 * fTemp1115))));
			double fTemp1119 = fTemp1109 + 0.2617993877991494;
			double fTemp1120 = ((fTemp1119 >= 6.283185307179586) ? fTemp1109 + -6.021385919380437 : fTemp1119);
			double fTemp1121 = 0.15915494309189535 * (fTemp1120 + 0.1308996938995747);
			double fTemp1122 = fTemp1121 - std::floor(fTemp1121);
			double fTemp1123 = std::sin(6.283185307179586 * fTemp1122) + fSlow5 * std::sin(12.566370614359172 * fTemp1122);
			double fTemp1124 = fTemp1112 - fTemp1123;
			double fTemp1125 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1123 + fSlow11 * fTemp1118));
			double fTemp1126 = 1.0 - mydsp_faustpower2_f(fTemp1125);
			double fTemp1127 = fTemp1125 - fTemp1118;
			double fTemp1128 = fSlow12 * std::fabs(fTemp1127) + 1.0;
			double fTemp1129 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1118 - fSlow4 * (fTemp1124 * (fSlow9 * fTemp1126 + fTemp1127 / (fTemp1128 * (fSlow13 * ((-(fSlow4 * fTemp1124) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1127 / fTemp1128))))) / (1.000000001 - fSlow14 * fTemp1126))));
			double fTemp1130 = fTemp1120 + 0.2617993877991494;
			double fTemp1131 = ((fTemp1130 >= 6.283185307179586) ? fTemp1120 + -6.021385919380437 : fTemp1130);
			double fTemp1132 = 0.15915494309189535 * (fTemp1131 + 0.1308996938995747);
			double fTemp1133 = fTemp1132 - std::floor(fTemp1132);
			double fTemp1134 = std::sin(6.283185307179586 * fTemp1133) + fSlow5 * std::sin(12.566370614359172 * fTemp1133);
			double fTemp1135 = fTemp1123 - fTemp1134;
			double fTemp1136 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1134 + fSlow11 * fTemp1129));
			double fTemp1137 = 1.0 - mydsp_faustpower2_f(fTemp1136);
			double fTemp1138 = fTemp1136 - fTemp1129;
			double fTemp1139 = fSlow12 * std::fabs(fTemp1138) + 1.0;
			double fTemp1140 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1129 - fSlow4 * (fTemp1135 * (fSlow9 * fTemp1137 + fTemp1138 / (fTemp1139 * (fSlow13 * ((-(fSlow4 * fTemp1135) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1138 / fTemp1139))))) / (1.000000001 - fSlow14 * fTemp1137))));
			double fTemp1141 = fTemp1131 + 0.2617993877991494;
			double fTemp1142 = ((fTemp1141 >= 6.283185307179586) ? fTemp1131 + -6.021385919380437 : fTemp1141);
			double fTemp1143 = 0.15915494309189535 * (fTemp1142 + 0.1308996938995747);
			double fTemp1144 = fTemp1143 - std::floor(fTemp1143);
			double fTemp1145 = std::sin(6.283185307179586 * fTemp1144) + fSlow5 * std::sin(12.566370614359172 * fTemp1144);
			double fTemp1146 = fTemp1134 - fTemp1145;
			double fTemp1147 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1145 + fSlow11 * fTemp1140));
			double fTemp1148 = 1.0 - mydsp_faustpower2_f(fTemp1147);
			double fTemp1149 = fTemp1147 - fTemp1140;
			double fTemp1150 = fSlow12 * std::fabs(fTemp1149) + 1.0;
			double fTemp1151 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1140 - fSlow4 * (fTemp1146 * (fSlow9 * fTemp1148 + fTemp1149 / (fTemp1150 * (fSlow13 * ((-(fSlow4 * fTemp1146) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1149 / fTemp1150))))) / (1.000000001 - fSlow14 * fTemp1148))));
			double fTemp1152 = fTemp1142 + 0.2617993877991494;
			double fTemp1153 = ((fTemp1152 >= 6.283185307179586) ? fTemp1142 + -6.021385919380437 : fTemp1152);
			double fTemp1154 = 0.15915494309189535 * (fTemp1153 + 0.1308996938995747);
			double fTemp1155 = fTemp1154 - std::floor(fTemp1154);
			double fTemp1156 = std::sin(6.283185307179586 * fTemp1155) + fSlow5 * std::sin(12.566370614359172 * fTemp1155);
			double fTemp1157 = fTemp1145 - fTemp1156;
			double fTemp1158 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1156 + fSlow11 * fTemp1151));
			double fTemp1159 = 1.0 - mydsp_faustpower2_f(fTemp1158);
			double fTemp1160 = fTemp1158 - fTemp1151;
			double fTemp1161 = fSlow12 * std::fabs(fTemp1160) + 1.0;
			double fTemp1162 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1151 - fSlow4 * (fTemp1157 * (fSlow9 * fTemp1159 + fTemp1160 / (fTemp1161 * (fSlow13 * ((-(fSlow4 * fTemp1157) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1160 / fTemp1161))))) / (1.000000001 - fSlow14 * fTemp1159))));
			double fTemp1163 = fTemp1153 + 0.2617993877991494;
			double fTemp1164 = ((fTemp1163 >= 6.283185307179586) ? fTemp1153 + -6.021385919380437 : fTemp1163);
			double fTemp1165 = 0.15915494309189535 * (fTemp1164 + 0.1308996938995747);
			double fTemp1166 = fTemp1165 - std::floor(fTemp1165);
			double fTemp1167 = std::sin(6.283185307179586 * fTemp1166) + fSlow5 * std::sin(12.566370614359172 * fTemp1166);
			double fTemp1168 = fTemp1156 - fTemp1167;
			double fTemp1169 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1167 + fSlow11 * fTemp1162));
			double fTemp1170 = 1.0 - mydsp_faustpower2_f(fTemp1169);
			double fTemp1171 = fTemp1169 - fTemp1162;
			double fTemp1172 = fSlow12 * std::fabs(fTemp1171) + 1.0;
			double fTemp1173 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1162 - fSlow4 * (fTemp1168 * (fSlow9 * fTemp1170 + fTemp1171 / (fTemp1172 * (fSlow13 * ((-(fSlow4 * fTemp1168) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1171 / fTemp1172))))) / (1.000000001 - fSlow14 * fTemp1170))));
			double fTemp1174 = fTemp1164 + 0.2617993877991494;
			double fTemp1175 = ((fTemp1174 >= 6.283185307179586) ? fTemp1164 + -6.021385919380437 : fTemp1174);
			double fTemp1176 = 0.15915494309189535 * (fTemp1175 + 0.1308996938995747);
			double fTemp1177 = fTemp1176 - std::floor(fTemp1176);
			double fTemp1178 = std::sin(6.283185307179586 * fTemp1177) + fSlow5 * std::sin(12.566370614359172 * fTemp1177);
			double fTemp1179 = fTemp1167 - fTemp1178;
			double fTemp1180 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1178 + fSlow11 * fTemp1173));
			double fTemp1181 = 1.0 - mydsp_faustpower2_f(fTemp1180);
			double fTemp1182 = fTemp1180 - fTemp1173;
			double fTemp1183 = fSlow12 * std::fabs(fTemp1182) + 1.0;
			double fTemp1184 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1173 - fSlow4 * (fTemp1179 * (fSlow9 * fTemp1181 + fTemp1182 / (fTemp1183 * (fSlow13 * ((-(fSlow4 * fTemp1179) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1182 / fTemp1183))))) / (1.000000001 - fSlow14 * fTemp1181))));
			double fTemp1185 = fTemp1175 + 0.2617993877991494;
			double fTemp1186 = ((fTemp1185 >= 6.283185307179586) ? fTemp1175 + -6.021385919380437 : fTemp1185);
			double fTemp1187 = 0.15915494309189535 * (fTemp1186 + 0.1308996938995747);
			double fTemp1188 = fTemp1187 - std::floor(fTemp1187);
			double fTemp1189 = std::sin(6.283185307179586 * fTemp1188) + fSlow5 * std::sin(12.566370614359172 * fTemp1188);
			double fTemp1190 = fTemp1178 - fTemp1189;
			double fTemp1191 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1189 + fSlow11 * fTemp1184));
			double fTemp1192 = 1.0 - mydsp_faustpower2_f(fTemp1191);
			double fTemp1193 = fTemp1191 - fTemp1184;
			double fTemp1194 = fSlow12 * std::fabs(fTemp1193) + 1.0;
			double fTemp1195 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1184 - fSlow4 * (fTemp1190 * (fSlow9 * fTemp1192 + fTemp1193 / (fTemp1194 * (fSlow13 * ((-(fSlow4 * fTemp1190) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1193 / fTemp1194))))) / (1.000000001 - fSlow14 * fTemp1192))));
			double fTemp1196 = fTemp1186 + 0.2617993877991494;
			double fTemp1197 = ((fTemp1196 >= 6.283185307179586) ? fTemp1186 + -6.021385919380437 : fTemp1196);
			double fTemp1198 = 0.15915494309189535 * (fTemp1197 + 0.1308996938995747);
			double fTemp1199 = fTemp1198 - std::floor(fTemp1198);
			double fTemp1200 = std::sin(6.283185307179586 * fTemp1199) + fSlow5 * std::sin(12.566370614359172 * fTemp1199);
			double fTemp1201 = fTemp1189 - fTemp1200;
			double fTemp1202 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1200 + fSlow11 * fTemp1195));
			double fTemp1203 = 1.0 - mydsp_faustpower2_f(fTemp1202);
			double fTemp1204 = fTemp1202 - fTemp1195;
			double fTemp1205 = fSlow12 * std::fabs(fTemp1204) + 1.0;
			double fTemp1206 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1195 - fSlow4 * (fTemp1201 * (fSlow9 * fTemp1203 + fTemp1204 / (fTemp1205 * (fSlow13 * ((-(fSlow4 * fTemp1201) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1204 / fTemp1205))))) / (1.000000001 - fSlow14 * fTemp1203))));
			double fTemp1207 = fTemp1197 + 0.2617993877991494;
			double fTemp1208 = ((fTemp1207 >= 6.283185307179586) ? fTemp1197 + -6.021385919380437 : fTemp1207);
			double fTemp1209 = 0.15915494309189535 * (fTemp1208 + 0.1308996938995747);
			double fTemp1210 = fTemp1209 - std::floor(fTemp1209);
			double fTemp1211 = std::sin(6.283185307179586 * fTemp1210) + fSlow5 * std::sin(12.566370614359172 * fTemp1210);
			double fTemp1212 = fTemp1200 - fTemp1211;
			double fTemp1213 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1211 + fSlow11 * fTemp1206));
			double fTemp1214 = 1.0 - mydsp_faustpower2_f(fTemp1213);
			double fTemp1215 = fTemp1213 - fTemp1206;
			double fTemp1216 = fSlow12 * std::fabs(fTemp1215) + 1.0;
			double fTemp1217 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1206 - fSlow4 * (fTemp1212 * (fSlow9 * fTemp1214 + fTemp1215 / (fTemp1216 * (fSlow13 * ((-(fSlow4 * fTemp1212) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1215 / fTemp1216))))) / (1.000000001 - fSlow14 * fTemp1214))));
			double fTemp1218 = fTemp1208 + 0.2617993877991494;
			double fTemp1219 = ((fTemp1218 >= 6.283185307179586) ? fTemp1208 + -6.021385919380437 : fTemp1218);
			double fTemp1220 = 0.15915494309189535 * (fTemp1219 + 0.1308996938995747);
			double fTemp1221 = fTemp1220 - std::floor(fTemp1220);
			double fTemp1222 = std::sin(6.283185307179586 * fTemp1221) + fSlow5 * std::sin(12.566370614359172 * fTemp1221);
			double fTemp1223 = fTemp1211 - fTemp1222;
			double fTemp1224 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1222 + fSlow11 * fTemp1217));
			double fTemp1225 = 1.0 - mydsp_faustpower2_f(fTemp1224);
			double fTemp1226 = fTemp1224 - fTemp1217;
			double fTemp1227 = fSlow12 * std::fabs(fTemp1226) + 1.0;
			double fTemp1228 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1217 - fSlow4 * (fTemp1223 * (fSlow9 * fTemp1225 + fTemp1226 / (fTemp1227 * (fSlow13 * ((-(fSlow4 * fTemp1223) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1226 / fTemp1227))))) / (1.000000001 - fSlow14 * fTemp1225))));
			double fTemp1229 = fTemp1219 + 0.2617993877991494;
			double fTemp1230 = ((fTemp1229 >= 6.283185307179586) ? fTemp1219 + -6.021385919380437 : fTemp1229);
			double fTemp1231 = 0.15915494309189535 * (fTemp1230 + 0.1308996938995747);
			double fTemp1232 = fTemp1231 - std::floor(fTemp1231);
			double fTemp1233 = std::sin(6.283185307179586 * fTemp1232) + fSlow5 * std::sin(12.566370614359172 * fTemp1232);
			double fTemp1234 = fTemp1222 - fTemp1233;
			double fTemp1235 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1233 + fSlow11 * fTemp1228));
			double fTemp1236 = 1.0 - mydsp_faustpower2_f(fTemp1235);
			double fTemp1237 = fTemp1235 - fTemp1228;
			double fTemp1238 = fSlow12 * std::fabs(fTemp1237) + 1.0;
			double fTemp1239 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1228 - fSlow4 * (fTemp1234 * (fSlow9 * fTemp1236 + fTemp1237 / (fTemp1238 * (fSlow13 * ((-(fSlow4 * fTemp1234) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1237 / fTemp1238))))) / (1.000000001 - fSlow14 * fTemp1236))));
			double fTemp1240 = fTemp1230 + 0.2617993877991494;
			double fTemp1241 = ((fTemp1240 >= 6.283185307179586) ? fTemp1230 + -6.021385919380437 : fTemp1240);
			double fTemp1242 = 0.15915494309189535 * (fTemp1241 + 0.1308996938995747);
			double fTemp1243 = fTemp1242 - std::floor(fTemp1242);
			double fTemp1244 = std::sin(6.283185307179586 * fTemp1243) + fSlow5 * std::sin(12.566370614359172 * fTemp1243);
			double fTemp1245 = fTemp1233 - fTemp1244;
			double fTemp1246 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1244 + fSlow11 * fTemp1239));
			double fTemp1247 = 1.0 - mydsp_faustpower2_f(fTemp1246);
			double fTemp1248 = fTemp1246 - fTemp1239;
			double fTemp1249 = fSlow12 * std::fabs(fTemp1248) + 1.0;
			double fTemp1250 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1239 - fSlow4 * (fTemp1245 * (fSlow9 * fTemp1247 + fTemp1248 / (fTemp1249 * (fSlow13 * ((-(fSlow4 * fTemp1245) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1248 / fTemp1249))))) / (1.000000001 - fSlow14 * fTemp1247))));
			double fTemp1251 = fTemp1241 + 0.2617993877991494;
			double fTemp1252 = ((fTemp1251 >= 6.283185307179586) ? fTemp1241 + -6.021385919380437 : fTemp1251);
			double fTemp1253 = 0.15915494309189535 * (fTemp1252 + 0.1308996938995747);
			double fTemp1254 = fTemp1253 - std::floor(fTemp1253);
			double fTemp1255 = std::sin(6.283185307179586 * fTemp1254) + fSlow5 * std::sin(12.566370614359172 * fTemp1254);
			double fTemp1256 = fTemp1244 - fTemp1255;
			double fTemp1257 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1255 + fSlow11 * fTemp1250));
			double fTemp1258 = 1.0 - mydsp_faustpower2_f(fTemp1257);
			double fTemp1259 = fTemp1257 - fTemp1250;
			double fTemp1260 = fSlow12 * std::fabs(fTemp1259) + 1.0;
			double fTemp1261 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1250 - fSlow4 * (fTemp1256 * (fSlow9 * fTemp1258 + fTemp1259 / (fTemp1260 * (fSlow13 * ((-(fSlow4 * fTemp1256) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1259 / fTemp1260))))) / (1.000000001 - fSlow14 * fTemp1258))));
			double fTemp1262 = fTemp1252 + 0.2617993877991494;
			double fTemp1263 = ((fTemp1262 >= 6.283185307179586) ? fTemp1252 + -6.021385919380437 : fTemp1262);
			double fTemp1264 = 0.15915494309189535 * (fTemp1263 + 0.1308996938995747);
			double fTemp1265 = fTemp1264 - std::floor(fTemp1264);
			double fTemp1266 = std::sin(6.283185307179586 * fTemp1265) + fSlow5 * std::sin(12.566370614359172 * fTemp1265);
			double fTemp1267 = fTemp1255 - fTemp1266;
			double fTemp1268 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1266 + fSlow11 * fTemp1261));
			double fTemp1269 = 1.0 - mydsp_faustpower2_f(fTemp1268);
			double fTemp1270 = fTemp1268 - fTemp1261;
			double fTemp1271 = fSlow12 * std::fabs(fTemp1270) + 1.0;
			double fTemp1272 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1261 - fSlow4 * (fTemp1267 * (fSlow9 * fTemp1269 + fTemp1270 / (fTemp1271 * (fSlow13 * ((-(fSlow4 * fTemp1267) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1270 / fTemp1271))))) / (1.000000001 - fSlow14 * fTemp1269))));
			double fTemp1273 = fTemp1263 + 0.2617993877991494;
			double fTemp1274 = ((fTemp1273 >= 6.283185307179586) ? fTemp1263 + -6.021385919380437 : fTemp1273);
			double fTemp1275 = 0.15915494309189535 * (fTemp1274 + 0.1308996938995747);
			double fTemp1276 = fTemp1275 - std::floor(fTemp1275);
			double fTemp1277 = std::sin(6.283185307179586 * fTemp1276) + fSlow5 * std::sin(12.566370614359172 * fTemp1276);
			double fTemp1278 = fTemp1266 - fTemp1277;
			double fTemp1279 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1277 + fSlow11 * fTemp1272));
			double fTemp1280 = 1.0 - mydsp_faustpower2_f(fTemp1279);
			double fTemp1281 = fTemp1279 - fTemp1272;
			double fTemp1282 = fSlow12 * std::fabs(fTemp1281) + 1.0;
			double fTemp1283 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1272 - fSlow4 * (fTemp1278 * (fSlow9 * fTemp1280 + fTemp1281 / (fTemp1282 * (fSlow13 * ((-(fSlow4 * fTemp1278) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1281 / fTemp1282))))) / (1.000000001 - fSlow14 * fTemp1280))));
			double fTemp1284 = fTemp1274 + 0.2617993877991494;
			double fTemp1285 = ((fTemp1284 >= 6.283185307179586) ? fTemp1274 + -6.021385919380437 : fTemp1284);
			double fTemp1286 = 0.15915494309189535 * (fTemp1285 + 0.1308996938995747);
			double fTemp1287 = fTemp1286 - std::floor(fTemp1286);
			double fTemp1288 = std::sin(6.283185307179586 * fTemp1287) + fSlow5 * std::sin(12.566370614359172 * fTemp1287);
			double fTemp1289 = fTemp1277 - fTemp1288;
			double fTemp1290 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1288 + fSlow11 * fTemp1283));
			double fTemp1291 = 1.0 - mydsp_faustpower2_f(fTemp1290);
			double fTemp1292 = fTemp1290 - fTemp1283;
			double fTemp1293 = fSlow12 * std::fabs(fTemp1292) + 1.0;
			double fTemp1294 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1283 - fSlow4 * (fTemp1289 * (fSlow9 * fTemp1291 + fTemp1292 / (fTemp1293 * (fSlow13 * ((-(fSlow4 * fTemp1289) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1292 / fTemp1293))))) / (1.000000001 - fSlow14 * fTemp1291))));
			double fTemp1295 = fTemp1285 + 0.2617993877991494;
			double fTemp1296 = ((fTemp1295 >= 6.283185307179586) ? fTemp1285 + -6.021385919380437 : fTemp1295);
			double fTemp1297 = 0.15915494309189535 * (fTemp1296 + 0.1308996938995747);
			double fTemp1298 = fTemp1297 - std::floor(fTemp1297);
			double fTemp1299 = std::sin(6.283185307179586 * fTemp1298) + fSlow5 * std::sin(12.566370614359172 * fTemp1298);
			double fTemp1300 = fTemp1288 - fTemp1299;
			double fTemp1301 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1299 + fSlow11 * fTemp1294));
			double fTemp1302 = 1.0 - mydsp_faustpower2_f(fTemp1301);
			double fTemp1303 = fTemp1301 - fTemp1294;
			double fTemp1304 = fSlow12 * std::fabs(fTemp1303) + 1.0;
			double fTemp1305 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1294 - fSlow4 * (fTemp1300 * (fSlow9 * fTemp1302 + fTemp1303 / (fTemp1304 * (fSlow13 * ((-(fSlow4 * fTemp1300) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1303 / fTemp1304))))) / (1.000000001 - fSlow14 * fTemp1302))));
			double fTemp1306 = fTemp1296 + 0.2617993877991494;
			double fTemp1307 = ((fTemp1306 >= 6.283185307179586) ? fTemp1296 + -6.021385919380437 : fTemp1306);
			double fTemp1308 = 0.15915494309189535 * (fTemp1307 + 0.1308996938995747);
			double fTemp1309 = fTemp1308 - std::floor(fTemp1308);
			double fTemp1310 = std::sin(6.283185307179586 * fTemp1309) + fSlow5 * std::sin(12.566370614359172 * fTemp1309);
			double fTemp1311 = fTemp1299 - fTemp1310;
			double fTemp1312 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1310 + fSlow11 * fTemp1305));
			double fTemp1313 = 1.0 - mydsp_faustpower2_f(fTemp1312);
			double fTemp1314 = fTemp1312 - fTemp1305;
			double fTemp1315 = fSlow12 * std::fabs(fTemp1314) + 1.0;
			double fTemp1316 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1305 - fSlow4 * (fTemp1311 * (fSlow9 * fTemp1313 + fTemp1314 / (fTemp1315 * (fSlow13 * ((-(fSlow4 * fTemp1311) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1314 / fTemp1315))))) / (1.000000001 - fSlow14 * fTemp1313))));
			double fTemp1317 = fTemp1307 + 0.2617993877991494;
			double fTemp1318 = ((fTemp1317 >= 6.283185307179586) ? fTemp1307 + -6.021385919380437 : fTemp1317);
			double fTemp1319 = 0.15915494309189535 * (fTemp1318 + 0.1308996938995747);
			double fTemp1320 = fTemp1319 - std::floor(fTemp1319);
			double fTemp1321 = std::sin(6.283185307179586 * fTemp1320) + fSlow5 * std::sin(12.566370614359172 * fTemp1320);
			double fTemp1322 = fTemp1310 - fTemp1321;
			double fTemp1323 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1321 + fSlow11 * fTemp1316));
			double fTemp1324 = 1.0 - mydsp_faustpower2_f(fTemp1323);
			double fTemp1325 = fTemp1323 - fTemp1316;
			double fTemp1326 = fSlow12 * std::fabs(fTemp1325) + 1.0;
			double fTemp1327 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1316 - fSlow4 * (fTemp1322 * (fSlow9 * fTemp1324 + fTemp1325 / (fTemp1326 * (fSlow13 * ((-(fSlow4 * fTemp1322) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1325 / fTemp1326))))) / (1.000000001 - fSlow14 * fTemp1324))));
			double fTemp1328 = fTemp1318 + 0.2617993877991494;
			double fTemp1329 = ((fTemp1328 >= 6.283185307179586) ? fTemp1318 + -6.021385919380437 : fTemp1328);
			double fTemp1330 = 0.15915494309189535 * (fTemp1329 + 0.1308996938995747);
			double fTemp1331 = fTemp1330 - std::floor(fTemp1330);
			double fTemp1332 = std::sin(6.283185307179586 * fTemp1331) + fSlow5 * std::sin(12.566370614359172 * fTemp1331);
			double fTemp1333 = fTemp1321 - fTemp1332;
			double fTemp1334 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1332 + fSlow11 * fTemp1327));
			double fTemp1335 = 1.0 - mydsp_faustpower2_f(fTemp1334);
			double fTemp1336 = fTemp1334 - fTemp1327;
			double fTemp1337 = fSlow12 * std::fabs(fTemp1336) + 1.0;
			double fTemp1338 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1327 - fSlow4 * (fTemp1333 * (fSlow9 * fTemp1335 + fTemp1336 / (fTemp1337 * (fSlow13 * ((-(fSlow4 * fTemp1333) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1336 / fTemp1337))))) / (1.000000001 - fSlow14 * fTemp1335))));
			double fTemp1339 = fTemp1329 + 0.2617993877991494;
			double fTemp1340 = ((fTemp1339 >= 6.283185307179586) ? fTemp1329 + -6.021385919380437 : fTemp1339);
			double fTemp1341 = 0.15915494309189535 * (fTemp1340 + 0.1308996938995747);
			double fTemp1342 = fTemp1341 - std::floor(fTemp1341);
			double fTemp1343 = std::sin(6.283185307179586 * fTemp1342) + fSlow5 * std::sin(12.566370614359172 * fTemp1342);
			double fTemp1344 = fTemp1332 - fTemp1343;
			double fTemp1345 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1343 + fSlow11 * fTemp1338));
			double fTemp1346 = 1.0 - mydsp_faustpower2_f(fTemp1345);
			double fTemp1347 = fTemp1345 - fTemp1338;
			double fTemp1348 = fSlow12 * std::fabs(fTemp1347) + 1.0;
			double fTemp1349 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1338 - fSlow4 * (fTemp1344 * (fSlow9 * fTemp1346 + fTemp1347 / (fTemp1348 * (fSlow13 * ((-(fSlow4 * fTemp1344) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1347 / fTemp1348))))) / (1.000000001 - fSlow14 * fTemp1346))));
			double fTemp1350 = fTemp1340 + 0.2617993877991494;
			double fTemp1351 = ((fTemp1350 >= 6.283185307179586) ? fTemp1340 + -6.021385919380437 : fTemp1350);
			double fTemp1352 = 0.15915494309189535 * (fTemp1351 + 0.1308996938995747);
			double fTemp1353 = fTemp1352 - std::floor(fTemp1352);
			double fTemp1354 = std::sin(6.283185307179586 * fTemp1353) + fSlow5 * std::sin(12.566370614359172 * fTemp1353);
			double fTemp1355 = fTemp1343 - fTemp1354;
			double fTemp1356 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1354 + fSlow11 * fTemp1349));
			double fTemp1357 = 1.0 - mydsp_faustpower2_f(fTemp1356);
			double fTemp1358 = fTemp1356 - fTemp1349;
			double fTemp1359 = fSlow12 * std::fabs(fTemp1358) + 1.0;
			double fTemp1360 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1349 - fSlow4 * (fTemp1355 * (fSlow9 * fTemp1357 + fTemp1358 / (fTemp1359 * (fSlow13 * ((-(fSlow4 * fTemp1355) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1358 / fTemp1359))))) / (1.000000001 - fSlow14 * fTemp1357))));
			double fTemp1361 = fTemp1351 + 0.2617993877991494;
			double fTemp1362 = ((fTemp1361 >= 6.283185307179586) ? fTemp1351 + -6.021385919380437 : fTemp1361);
			double fTemp1363 = 0.15915494309189535 * (fTemp1362 + 0.1308996938995747);
			double fTemp1364 = fTemp1363 - std::floor(fTemp1363);
			double fTemp1365 = std::sin(6.283185307179586 * fTemp1364) + fSlow5 * std::sin(12.566370614359172 * fTemp1364);
			double fTemp1366 = fTemp1354 - fTemp1365;
			double fTemp1367 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1365 + fSlow11 * fTemp1360));
			double fTemp1368 = 1.0 - mydsp_faustpower2_f(fTemp1367);
			double fTemp1369 = fTemp1367 - fTemp1360;
			double fTemp1370 = fSlow12 * std::fabs(fTemp1369) + 1.0;
			double fTemp1371 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1360 - fSlow4 * (fTemp1366 * (fSlow9 * fTemp1368 + fTemp1369 / (fTemp1370 * (fSlow13 * ((-(fSlow4 * fTemp1366) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1369 / fTemp1370))))) / (1.000000001 - fSlow14 * fTemp1368))));
			double fTemp1372 = fTemp1362 + 0.2617993877991494;
			double fTemp1373 = ((fTemp1372 >= 6.283185307179586) ? fTemp1362 + -6.021385919380437 : fTemp1372);
			double fTemp1374 = 0.15915494309189535 * (fTemp1373 + 0.1308996938995747);
			double fTemp1375 = fTemp1374 - std::floor(fTemp1374);
			double fTemp1376 = std::sin(6.283185307179586 * fTemp1375) + fSlow5 * std::sin(12.566370614359172 * fTemp1375);
			double fTemp1377 = fTemp1365 - fTemp1376;
			double fTemp1378 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1376 + fSlow11 * fTemp1371));
			double fTemp1379 = 1.0 - mydsp_faustpower2_f(fTemp1378);
			double fTemp1380 = fTemp1378 - fTemp1371;
			double fTemp1381 = fSlow12 * std::fabs(fTemp1380) + 1.0;
			double fTemp1382 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1371 - fSlow4 * (fTemp1377 * (fSlow9 * fTemp1379 + fTemp1380 / (fTemp1381 * (fSlow13 * ((-(fSlow4 * fTemp1377) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1380 / fTemp1381))))) / (1.000000001 - fSlow14 * fTemp1379))));
			double fTemp1383 = fTemp1373 + 0.2617993877991494;
			double fTemp1384 = ((fTemp1383 >= 6.283185307179586) ? fTemp1373 + -6.021385919380437 : fTemp1383);
			double fTemp1385 = 0.15915494309189535 * (fTemp1384 + 0.1308996938995747);
			double fTemp1386 = fTemp1385 - std::floor(fTemp1385);
			double fTemp1387 = std::sin(6.283185307179586 * fTemp1386) + fSlow5 * std::sin(12.566370614359172 * fTemp1386);
			double fTemp1388 = fTemp1376 - fTemp1387;
			double fTemp1389 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1387 + fSlow11 * fTemp1382));
			double fTemp1390 = 1.0 - mydsp_faustpower2_f(fTemp1389);
			double fTemp1391 = fTemp1389 - fTemp1382;
			double fTemp1392 = fSlow12 * std::fabs(fTemp1391) + 1.0;
			double fTemp1393 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1382 - fSlow4 * (fTemp1388 * (fSlow9 * fTemp1390 + fTemp1391 / (fTemp1392 * (fSlow13 * ((-(fSlow4 * fTemp1388) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1391 / fTemp1392))))) / (1.000000001 - fSlow14 * fTemp1390))));
			double fTemp1394 = fTemp1384 + 0.2617993877991494;
			double fTemp1395 = ((fTemp1394 >= 6.283185307179586) ? fTemp1384 + -6.021385919380437 : fTemp1394);
			double fTemp1396 = 0.15915494309189535 * (fTemp1395 + 0.1308996938995747);
			double fTemp1397 = fTemp1396 - std::floor(fTemp1396);
			double fTemp1398 = std::sin(6.283185307179586 * fTemp1397) + fSlow5 * std::sin(12.566370614359172 * fTemp1397);
			double fTemp1399 = fTemp1387 - fTemp1398;
			double fTemp1400 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1398 + fSlow11 * fTemp1393));
			double fTemp1401 = 1.0 - mydsp_faustpower2_f(fTemp1400);
			double fTemp1402 = fTemp1400 - fTemp1393;
			double fTemp1403 = fSlow12 * std::fabs(fTemp1402) + 1.0;
			double fTemp1404 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1393 - fSlow4 * (fTemp1399 * (fSlow9 * fTemp1401 + fTemp1402 / (fTemp1403 * (fSlow13 * ((-(fSlow4 * fTemp1399) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1402 / fTemp1403))))) / (1.000000001 - fSlow14 * fTemp1401))));
			double fTemp1405 = fTemp1395 + 0.2617993877991494;
			double fTemp1406 = ((fTemp1405 >= 6.283185307179586) ? fTemp1395 + -6.021385919380437 : fTemp1405);
			double fTemp1407 = 0.15915494309189535 * (fTemp1406 + 0.1308996938995747);
			double fTemp1408 = fTemp1407 - std::floor(fTemp1407);
			double fTemp1409 = std::sin(6.283185307179586 * fTemp1408) + fSlow5 * std::sin(12.566370614359172 * fTemp1408);
			double fTemp1410 = fTemp1398 - fTemp1409;
			double fTemp1411 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1409 + fSlow11 * fTemp1404));
			double fTemp1412 = 1.0 - mydsp_faustpower2_f(fTemp1411);
			double fTemp1413 = fTemp1411 - fTemp1404;
			double fTemp1414 = fSlow12 * std::fabs(fTemp1413) + 1.0;
			double fTemp1415 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1404 - fSlow4 * (fTemp1410 * (fSlow9 * fTemp1412 + fTemp1413 / (fTemp1414 * (fSlow13 * ((-(fSlow4 * fTemp1410) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1413 / fTemp1414))))) / (1.000000001 - fSlow14 * fTemp1412))));
			double fTemp1416 = fTemp1406 + 0.2617993877991494;
			double fTemp1417 = ((fTemp1416 >= 6.283185307179586) ? fTemp1406 + -6.021385919380437 : fTemp1416);
			double fTemp1418 = 0.15915494309189535 * (fTemp1417 + 0.1308996938995747);
			double fTemp1419 = fTemp1418 - std::floor(fTemp1418);
			double fTemp1420 = std::sin(6.283185307179586 * fTemp1419) + fSlow5 * std::sin(12.566370614359172 * fTemp1419);
			double fTemp1421 = fTemp1409 - fTemp1420;
			double fTemp1422 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1420 + fSlow11 * fTemp1415));
			double fTemp1423 = 1.0 - mydsp_faustpower2_f(fTemp1422);
			double fTemp1424 = fTemp1422 - fTemp1415;
			double fTemp1425 = fSlow12 * std::fabs(fTemp1424) + 1.0;
			double fTemp1426 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1415 - fSlow4 * (fTemp1421 * (fSlow9 * fTemp1423 + fTemp1424 / (fTemp1425 * (fSlow13 * ((-(fSlow4 * fTemp1421) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1424 / fTemp1425))))) / (1.000000001 - fSlow14 * fTemp1423))));
			double fTemp1427 = fTemp1417 + 0.2617993877991494;
			double fTemp1428 = ((fTemp1427 >= 6.283185307179586) ? fTemp1417 + -6.021385919380437 : fTemp1427);
			double fTemp1429 = 0.15915494309189535 * (fTemp1428 + 0.1308996938995747);
			double fTemp1430 = fTemp1429 - std::floor(fTemp1429);
			double fTemp1431 = std::sin(6.283185307179586 * fTemp1430) + fSlow5 * std::sin(12.566370614359172 * fTemp1430);
			double fTemp1432 = fTemp1420 - fTemp1431;
			double fTemp1433 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1431 + fSlow11 * fTemp1426));
			double fTemp1434 = 1.0 - mydsp_faustpower2_f(fTemp1433);
			double fTemp1435 = fTemp1433 - fTemp1426;
			double fTemp1436 = fSlow12 * std::fabs(fTemp1435) + 1.0;
			double fTemp1437 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1426 - fSlow4 * (fTemp1432 * (fSlow9 * fTemp1434 + fTemp1435 / (fTemp1436 * (fSlow13 * ((-(fSlow4 * fTemp1432) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1435 / fTemp1436))))) / (1.000000001 - fSlow14 * fTemp1434))));
			double fTemp1438 = fTemp1428 + 0.2617993877991494;
			double fTemp1439 = ((fTemp1438 >= 6.283185307179586) ? fTemp1428 + -6.021385919380437 : fTemp1438);
			double fTemp1440 = 0.15915494309189535 * (fTemp1439 + 0.1308996938995747);
			double fTemp1441 = fTemp1440 - std::floor(fTemp1440);
			double fTemp1442 = std::sin(6.283185307179586 * fTemp1441) + fSlow5 * std::sin(12.566370614359172 * fTemp1441);
			double fTemp1443 = fTemp1431 - fTemp1442;
			double fTemp1444 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1442 + fSlow11 * fTemp1437));
			double fTemp1445 = 1.0 - mydsp_faustpower2_f(fTemp1444);
			double fTemp1446 = fTemp1444 - fTemp1437;
			double fTemp1447 = fSlow12 * std::fabs(fTemp1446) + 1.0;
			double fTemp1448 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1437 - fSlow4 * (fTemp1443 * (fSlow9 * fTemp1445 + fTemp1446 / (fTemp1447 * (fSlow13 * ((-(fSlow4 * fTemp1443) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1446 / fTemp1447))))) / (1.000000001 - fSlow14 * fTemp1445))));
			double fTemp1449 = fTemp1439 + 0.2617993877991494;
			double fTemp1450 = ((fTemp1449 >= 6.283185307179586) ? fTemp1439 + -6.021385919380437 : fTemp1449);
			double fTemp1451 = 0.15915494309189535 * (fTemp1450 + 0.1308996938995747);
			double fTemp1452 = fTemp1451 - std::floor(fTemp1451);
			double fTemp1453 = std::sin(6.283185307179586 * fTemp1452) + fSlow5 * std::sin(12.566370614359172 * fTemp1452);
			double fTemp1454 = fTemp1442 - fTemp1453;
			double fTemp1455 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1453 + fSlow11 * fTemp1448));
			double fTemp1456 = 1.0 - mydsp_faustpower2_f(fTemp1455);
			double fTemp1457 = fTemp1455 - fTemp1448;
			double fTemp1458 = fSlow12 * std::fabs(fTemp1457) + 1.0;
			double fTemp1459 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1448 - fSlow4 * (fTemp1454 * (fSlow9 * fTemp1456 + fTemp1457 / (fTemp1458 * (fSlow13 * ((-(fSlow4 * fTemp1454) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1457 / fTemp1458))))) / (1.000000001 - fSlow14 * fTemp1456))));
			double fTemp1460 = fTemp1450 + 0.2617993877991494;
			double fTemp1461 = ((fTemp1460 >= 6.283185307179586) ? fTemp1450 + -6.021385919380437 : fTemp1460);
			double fTemp1462 = 0.15915494309189535 * (fTemp1461 + 0.1308996938995747);
			double fTemp1463 = fTemp1462 - std::floor(fTemp1462);
			double fTemp1464 = std::sin(6.283185307179586 * fTemp1463) + fSlow5 * std::sin(12.566370614359172 * fTemp1463);
			double fTemp1465 = fTemp1453 - fTemp1464;
			double fTemp1466 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1464 + fSlow11 * fTemp1459));
			double fTemp1467 = 1.0 - mydsp_faustpower2_f(fTemp1466);
			double fTemp1468 = fTemp1466 - fTemp1459;
			double fTemp1469 = fSlow12 * std::fabs(fTemp1468) + 1.0;
			double fTemp1470 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1459 - fSlow4 * (fTemp1465 * (fSlow9 * fTemp1467 + fTemp1468 / (fTemp1469 * (fSlow13 * ((-(fSlow4 * fTemp1465) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1468 / fTemp1469))))) / (1.000000001 - fSlow14 * fTemp1467))));
			double fTemp1471 = fTemp1461 + 0.2617993877991494;
			double fTemp1472 = ((fTemp1471 >= 6.283185307179586) ? fTemp1461 + -6.021385919380437 : fTemp1471);
			double fTemp1473 = 0.15915494309189535 * (fTemp1472 + 0.1308996938995747);
			double fTemp1474 = fTemp1473 - std::floor(fTemp1473);
			double fTemp1475 = std::sin(6.283185307179586 * fTemp1474) + fSlow5 * std::sin(12.566370614359172 * fTemp1474);
			double fTemp1476 = fTemp1464 - fTemp1475;
			double fTemp1477 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1475 + fSlow11 * fTemp1470));
			double fTemp1478 = 1.0 - mydsp_faustpower2_f(fTemp1477);
			double fTemp1479 = fTemp1477 - fTemp1470;
			double fTemp1480 = fSlow12 * std::fabs(fTemp1479) + 1.0;
			double fTemp1481 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1470 - fSlow4 * (fTemp1476 * (fSlow9 * fTemp1478 + fTemp1479 / (fTemp1480 * (fSlow13 * ((-(fSlow4 * fTemp1476) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1479 / fTemp1480))))) / (1.000000001 - fSlow14 * fTemp1478))));
			double fTemp1482 = fTemp1472 + 0.2617993877991494;
			double fTemp1483 = ((fTemp1482 >= 6.283185307179586) ? fTemp1472 + -6.021385919380437 : fTemp1482);
			double fTemp1484 = 0.15915494309189535 * (fTemp1483 + 0.1308996938995747);
			double fTemp1485 = fTemp1484 - std::floor(fTemp1484);
			double fTemp1486 = std::sin(6.283185307179586 * fTemp1485) + fSlow5 * std::sin(12.566370614359172 * fTemp1485);
			double fTemp1487 = fTemp1475 - fTemp1486;
			double fTemp1488 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1486 + fSlow11 * fTemp1481));
			double fTemp1489 = 1.0 - mydsp_faustpower2_f(fTemp1488);
			double fTemp1490 = fTemp1488 - fTemp1481;
			double fTemp1491 = fSlow12 * std::fabs(fTemp1490) + 1.0;
			double fTemp1492 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1481 - fSlow4 * (fTemp1487 * (fSlow9 * fTemp1489 + fTemp1490 / (fTemp1491 * (fSlow13 * ((-(fSlow4 * fTemp1487) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1490 / fTemp1491))))) / (1.000000001 - fSlow14 * fTemp1489))));
			double fTemp1493 = fTemp1483 + 0.2617993877991494;
			double fTemp1494 = ((fTemp1493 >= 6.283185307179586) ? fTemp1483 + -6.021385919380437 : fTemp1493);
			double fTemp1495 = 0.15915494309189535 * (fTemp1494 + 0.1308996938995747);
			double fTemp1496 = fTemp1495 - std::floor(fTemp1495);
			double fTemp1497 = std::sin(6.283185307179586 * fTemp1496) + fSlow5 * std::sin(12.566370614359172 * fTemp1496);
			double fTemp1498 = fTemp1486 - fTemp1497;
			double fTemp1499 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1497 + fSlow11 * fTemp1492));
			double fTemp1500 = 1.0 - mydsp_faustpower2_f(fTemp1499);
			double fTemp1501 = fTemp1499 - fTemp1492;
			double fTemp1502 = fSlow12 * std::fabs(fTemp1501) + 1.0;
			double fTemp1503 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1492 - fSlow4 * (fTemp1498 * (fSlow9 * fTemp1500 + fTemp1501 / (fTemp1502 * (fSlow13 * ((-(fSlow4 * fTemp1498) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1501 / fTemp1502))))) / (1.000000001 - fSlow14 * fTemp1500))));
			double fTemp1504 = fTemp1494 + 0.2617993877991494;
			double fTemp1505 = ((fTemp1504 >= 6.283185307179586) ? fTemp1494 + -6.021385919380437 : fTemp1504);
			double fTemp1506 = 0.15915494309189535 * (fTemp1505 + 0.1308996938995747);
			double fTemp1507 = fTemp1506 - std::floor(fTemp1506);
			double fTemp1508 = std::sin(6.283185307179586 * fTemp1507) + fSlow5 * std::sin(12.566370614359172 * fTemp1507);
			double fTemp1509 = fTemp1497 - fTemp1508;
			double fTemp1510 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1508 + fSlow11 * fTemp1503));
			double fTemp1511 = 1.0 - mydsp_faustpower2_f(fTemp1510);
			double fTemp1512 = fTemp1510 - fTemp1503;
			double fTemp1513 = fSlow12 * std::fabs(fTemp1512) + 1.0;
			double fTemp1514 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1503 - fSlow4 * (fTemp1509 * (fSlow9 * fTemp1511 + fTemp1512 / (fTemp1513 * (fSlow13 * ((-(fSlow4 * fTemp1509) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1512 / fTemp1513))))) / (1.000000001 - fSlow14 * fTemp1511))));
			double fTemp1515 = fTemp1505 + 0.2617993877991494;
			double fTemp1516 = ((fTemp1515 >= 6.283185307179586) ? fTemp1505 + -6.021385919380437 : fTemp1515);
			double fTemp1517 = 0.15915494309189535 * (fTemp1516 + 0.1308996938995747);
			double fTemp1518 = fTemp1517 - std::floor(fTemp1517);
			double fTemp1519 = std::sin(6.283185307179586 * fTemp1518) + fSlow5 * std::sin(12.566370614359172 * fTemp1518);
			double fTemp1520 = fTemp1508 - fTemp1519;
			double fTemp1521 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1519 + fSlow11 * fTemp1514));
			double fTemp1522 = 1.0 - mydsp_faustpower2_f(fTemp1521);
			double fTemp1523 = fTemp1521 - fTemp1514;
			double fTemp1524 = fSlow12 * std::fabs(fTemp1523) + 1.0;
			double fTemp1525 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1514 - fSlow4 * (fTemp1520 * (fSlow9 * fTemp1522 + fTemp1523 / (fTemp1524 * (fSlow13 * ((-(fSlow4 * fTemp1520) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1523 / fTemp1524))))) / (1.000000001 - fSlow14 * fTemp1522))));
			double fTemp1526 = fTemp1516 + 0.2617993877991494;
			double fTemp1527 = ((fTemp1526 >= 6.283185307179586) ? fTemp1516 + -6.021385919380437 : fTemp1526);
			double fTemp1528 = 0.15915494309189535 * (fTemp1527 + 0.1308996938995747);
			double fTemp1529 = fTemp1528 - std::floor(fTemp1528);
			double fTemp1530 = std::sin(6.283185307179586 * fTemp1529) + fSlow5 * std::sin(12.566370614359172 * fTemp1529);
			double fTemp1531 = fTemp1519 - fTemp1530;
			double fTemp1532 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1530 + fSlow11 * fTemp1525));
			double fTemp1533 = 1.0 - mydsp_faustpower2_f(fTemp1532);
			double fTemp1534 = fTemp1532 - fTemp1525;
			double fTemp1535 = fSlow12 * std::fabs(fTemp1534) + 1.0;
			double fTemp1536 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1525 - fSlow4 * (fTemp1531 * (fSlow9 * fTemp1533 + fTemp1534 / (fTemp1535 * (fSlow13 * ((-(fSlow4 * fTemp1531) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1534 / fTemp1535))))) / (1.000000001 - fSlow14 * fTemp1533))));
			double fTemp1537 = fTemp1527 + 0.2617993877991494;
			double fTemp1538 = ((fTemp1537 >= 6.283185307179586) ? fTemp1527 + -6.021385919380437 : fTemp1537);
			double fTemp1539 = 0.15915494309189535 * (fTemp1538 + 0.1308996938995747);
			double fTemp1540 = fTemp1539 - std::floor(fTemp1539);
			double fTemp1541 = std::sin(6.283185307179586 * fTemp1540) + fSlow5 * std::sin(12.566370614359172 * fTemp1540);
			double fTemp1542 = fTemp1530 - fTemp1541;
			double fTemp1543 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1541 + fSlow11 * fTemp1536));
			double fTemp1544 = 1.0 - mydsp_faustpower2_f(fTemp1543);
			double fTemp1545 = fTemp1543 - fTemp1536;
			double fTemp1546 = fSlow12 * std::fabs(fTemp1545) + 1.0;
			double fTemp1547 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1536 - fSlow4 * (fTemp1542 * (fSlow9 * fTemp1544 + fTemp1545 / (fTemp1546 * (fSlow13 * ((-(fSlow4 * fTemp1542) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1545 / fTemp1546))))) / (1.000000001 - fSlow14 * fTemp1544))));
			double fTemp1548 = fTemp1538 + 0.2617993877991494;
			double fTemp1549 = ((fTemp1548 >= 6.283185307179586) ? fTemp1538 + -6.021385919380437 : fTemp1548);
			double fTemp1550 = 0.15915494309189535 * (fTemp1549 + 0.1308996938995747);
			double fTemp1551 = fTemp1550 - std::floor(fTemp1550);
			double fTemp1552 = std::sin(6.283185307179586 * fTemp1551) + fSlow5 * std::sin(12.566370614359172 * fTemp1551);
			double fTemp1553 = fTemp1541 - fTemp1552;
			double fTemp1554 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1552 + fSlow11 * fTemp1547));
			double fTemp1555 = 1.0 - mydsp_faustpower2_f(fTemp1554);
			double fTemp1556 = fTemp1554 - fTemp1547;
			double fTemp1557 = fSlow12 * std::fabs(fTemp1556) + 1.0;
			double fTemp1558 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1547 - fSlow4 * (fTemp1553 * (fSlow9 * fTemp1555 + fTemp1556 / (fTemp1557 * (fSlow13 * ((-(fSlow4 * fTemp1553) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1556 / fTemp1557))))) / (1.000000001 - fSlow14 * fTemp1555))));
			double fTemp1559 = fTemp1549 + 0.2617993877991494;
			double fTemp1560 = ((fTemp1559 >= 6.283185307179586) ? fTemp1549 + -6.021385919380437 : fTemp1559);
			double fTemp1561 = 0.15915494309189535 * (fTemp1560 + 0.1308996938995747);
			double fTemp1562 = fTemp1561 - std::floor(fTemp1561);
			double fTemp1563 = std::sin(6.283185307179586 * fTemp1562) + fSlow5 * std::sin(12.566370614359172 * fTemp1562);
			double fTemp1564 = fTemp1552 - fTemp1563;
			double fTemp1565 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1563 + fSlow11 * fTemp1558));
			double fTemp1566 = 1.0 - mydsp_faustpower2_f(fTemp1565);
			double fTemp1567 = fTemp1565 - fTemp1558;
			double fTemp1568 = fSlow12 * std::fabs(fTemp1567) + 1.0;
			double fTemp1569 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1558 - fSlow4 * (fTemp1564 * (fSlow9 * fTemp1566 + fTemp1567 / (fTemp1568 * (fSlow13 * ((-(fSlow4 * fTemp1564) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1567 / fTemp1568))))) / (1.000000001 - fSlow14 * fTemp1566))));
			double fTemp1570 = fTemp1560 + 0.2617993877991494;
			double fTemp1571 = ((fTemp1570 >= 6.283185307179586) ? fTemp1560 + -6.021385919380437 : fTemp1570);
			double fTemp1572 = 0.15915494309189535 * (fTemp1571 + 0.1308996938995747);
			double fTemp1573 = fTemp1572 - std::floor(fTemp1572);
			double fTemp1574 = std::sin(6.283185307179586 * fTemp1573) + fSlow5 * std::sin(12.566370614359172 * fTemp1573);
			double fTemp1575 = fTemp1563 - fTemp1574;
			double fTemp1576 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1574 + fSlow11 * fTemp1569));
			double fTemp1577 = 1.0 - mydsp_faustpower2_f(fTemp1576);
			double fTemp1578 = fTemp1576 - fTemp1569;
			double fTemp1579 = fSlow12 * std::fabs(fTemp1578) + 1.0;
			double fTemp1580 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1569 - fSlow4 * (fTemp1575 * (fSlow9 * fTemp1577 + fTemp1578 / (fTemp1579 * (fSlow13 * ((-(fSlow4 * fTemp1575) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1578 / fTemp1579))))) / (1.000000001 - fSlow14 * fTemp1577))));
			double fTemp1581 = fTemp1571 + 0.2617993877991494;
			double fTemp1582 = ((fTemp1581 >= 6.283185307179586) ? fTemp1571 + -6.021385919380437 : fTemp1581);
			double fTemp1583 = 0.15915494309189535 * (fTemp1582 + 0.1308996938995747);
			double fTemp1584 = fTemp1583 - std::floor(fTemp1583);
			double fTemp1585 = std::sin(6.283185307179586 * fTemp1584) + fSlow5 * std::sin(12.566370614359172 * fTemp1584);
			double fTemp1586 = fTemp1574 - fTemp1585;
			double fTemp1587 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1585 + fSlow11 * fTemp1580));
			double fTemp1588 = 1.0 - mydsp_faustpower2_f(fTemp1587);
			double fTemp1589 = fTemp1587 - fTemp1580;
			double fTemp1590 = fSlow12 * std::fabs(fTemp1589) + 1.0;
			double fTemp1591 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1580 - fSlow4 * (fTemp1586 * (fSlow9 * fTemp1588 + fTemp1589 / (fTemp1590 * (fSlow13 * ((-(fSlow4 * fTemp1586) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1589 / fTemp1590))))) / (1.000000001 - fSlow14 * fTemp1588))));
			double fTemp1592 = fTemp1582 + 0.2617993877991494;
			double fTemp1593 = ((fTemp1592 >= 6.283185307179586) ? fTemp1582 + -6.021385919380437 : fTemp1592);
			double fTemp1594 = 0.15915494309189535 * (fTemp1593 + 0.1308996938995747);
			double fTemp1595 = fTemp1594 - std::floor(fTemp1594);
			double fTemp1596 = std::sin(6.283185307179586 * fTemp1595) + fSlow5 * std::sin(12.566370614359172 * fTemp1595);
			double fTemp1597 = fTemp1585 - fTemp1596;
			double fTemp1598 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1596 + fSlow11 * fTemp1591));
			double fTemp1599 = 1.0 - mydsp_faustpower2_f(fTemp1598);
			double fTemp1600 = fTemp1598 - fTemp1591;
			double fTemp1601 = fSlow12 * std::fabs(fTemp1600) + 1.0;
			double fTemp1602 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1591 - fSlow4 * (fTemp1597 * (fSlow9 * fTemp1599 + fTemp1600 / (fTemp1601 * (fSlow13 * ((-(fSlow4 * fTemp1597) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1600 / fTemp1601))))) / (1.000000001 - fSlow14 * fTemp1599))));
			double fTemp1603 = fTemp1593 + 0.2617993877991494;
			double fTemp1604 = ((fTemp1603 >= 6.283185307179586) ? fTemp1593 + -6.021385919380437 : fTemp1603);
			double fTemp1605 = 0.15915494309189535 * (fTemp1604 + 0.1308996938995747);
			double fTemp1606 = fTemp1605 - std::floor(fTemp1605);
			double fTemp1607 = std::sin(6.283185307179586 * fTemp1606) + fSlow5 * std::sin(12.566370614359172 * fTemp1606);
			double fTemp1608 = fTemp1596 - fTemp1607;
			double fTemp1609 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1607 + fSlow11 * fTemp1602));
			double fTemp1610 = 1.0 - mydsp_faustpower2_f(fTemp1609);
			double fTemp1611 = fTemp1609 - fTemp1602;
			double fTemp1612 = fSlow12 * std::fabs(fTemp1611) + 1.0;
			double fTemp1613 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1602 - fSlow4 * (fTemp1608 * (fSlow9 * fTemp1610 + fTemp1611 / (fTemp1612 * (fSlow13 * ((-(fSlow4 * fTemp1608) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1611 / fTemp1612))))) / (1.000000001 - fSlow14 * fTemp1610))));
			double fTemp1614 = fTemp1604 + 0.2617993877991494;
			double fTemp1615 = ((fTemp1614 >= 6.283185307179586) ? fTemp1604 + -6.021385919380437 : fTemp1614);
			double fTemp1616 = 0.15915494309189535 * (fTemp1615 + 0.1308996938995747);
			double fTemp1617 = fTemp1616 - std::floor(fTemp1616);
			double fTemp1618 = std::sin(6.283185307179586 * fTemp1617) + fSlow5 * std::sin(12.566370614359172 * fTemp1617);
			double fTemp1619 = fTemp1607 - fTemp1618;
			double fTemp1620 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1618 + fSlow11 * fTemp1613));
			double fTemp1621 = 1.0 - mydsp_faustpower2_f(fTemp1620);
			double fTemp1622 = fTemp1620 - fTemp1613;
			double fTemp1623 = fSlow12 * std::fabs(fTemp1622) + 1.0;
			double fTemp1624 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1613 - fSlow4 * (fTemp1619 * (fSlow9 * fTemp1621 + fTemp1622 / (fTemp1623 * (fSlow13 * ((-(fSlow4 * fTemp1619) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1622 / fTemp1623))))) / (1.000000001 - fSlow14 * fTemp1621))));
			double fTemp1625 = fTemp1615 + 0.2617993877991494;
			double fTemp1626 = ((fTemp1625 >= 6.283185307179586) ? fTemp1615 + -6.021385919380437 : fTemp1625);
			double fTemp1627 = 0.15915494309189535 * (fTemp1626 + 0.1308996938995747);
			double fTemp1628 = fTemp1627 - std::floor(fTemp1627);
			double fTemp1629 = std::sin(6.283185307179586 * fTemp1628) + fSlow5 * std::sin(12.566370614359172 * fTemp1628);
			double fTemp1630 = fTemp1618 - fTemp1629;
			double fTemp1631 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1629 + fSlow11 * fTemp1624));
			double fTemp1632 = 1.0 - mydsp_faustpower2_f(fTemp1631);
			double fTemp1633 = fTemp1631 - fTemp1624;
			double fTemp1634 = fSlow12 * std::fabs(fTemp1633) + 1.0;
			double fTemp1635 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1624 - fSlow4 * (fTemp1630 * (fSlow9 * fTemp1632 + fTemp1633 / (fTemp1634 * (fSlow13 * ((-(fSlow4 * fTemp1630) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1633 / fTemp1634))))) / (1.000000001 - fSlow14 * fTemp1632))));
			double fTemp1636 = fTemp1626 + 0.2617993877991494;
			double fTemp1637 = ((fTemp1636 >= 6.283185307179586) ? fTemp1626 + -6.021385919380437 : fTemp1636);
			double fTemp1638 = 0.15915494309189535 * (fTemp1637 + 0.1308996938995747);
			double fTemp1639 = fTemp1638 - std::floor(fTemp1638);
			double fTemp1640 = std::sin(6.283185307179586 * fTemp1639) + fSlow5 * std::sin(12.566370614359172 * fTemp1639);
			double fTemp1641 = fTemp1629 - fTemp1640;
			double fTemp1642 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1640 + fSlow11 * fTemp1635));
			double fTemp1643 = 1.0 - mydsp_faustpower2_f(fTemp1642);
			double fTemp1644 = fTemp1642 - fTemp1635;
			double fTemp1645 = fSlow12 * std::fabs(fTemp1644) + 1.0;
			double fTemp1646 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1635 - fSlow4 * (fTemp1641 * (fSlow9 * fTemp1643 + fTemp1644 / (fTemp1645 * (fSlow13 * ((-(fSlow4 * fTemp1641) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1644 / fTemp1645))))) / (1.000000001 - fSlow14 * fTemp1643))));
			double fTemp1647 = fTemp1637 + 0.2617993877991494;
			double fTemp1648 = ((fTemp1647 >= 6.283185307179586) ? fTemp1637 + -6.021385919380437 : fTemp1647);
			double fTemp1649 = 0.15915494309189535 * (fTemp1648 + 0.1308996938995747);
			double fTemp1650 = fTemp1649 - std::floor(fTemp1649);
			double fTemp1651 = std::sin(6.283185307179586 * fTemp1650) + fSlow5 * std::sin(12.566370614359172 * fTemp1650);
			double fTemp1652 = fTemp1640 - fTemp1651;
			double fTemp1653 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1651 + fSlow11 * fTemp1646));
			double fTemp1654 = 1.0 - mydsp_faustpower2_f(fTemp1653);
			double fTemp1655 = fTemp1653 - fTemp1646;
			double fTemp1656 = fSlow12 * std::fabs(fTemp1655) + 1.0;
			double fTemp1657 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1646 - fSlow4 * (fTemp1652 * (fSlow9 * fTemp1654 + fTemp1655 / (fTemp1656 * (fSlow13 * ((-(fSlow4 * fTemp1652) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1655 / fTemp1656))))) / (1.000000001 - fSlow14 * fTemp1654))));
			double fTemp1658 = fTemp1648 + 0.2617993877991494;
			double fTemp1659 = ((fTemp1658 >= 6.283185307179586) ? fTemp1648 + -6.021385919380437 : fTemp1658);
			double fTemp1660 = 0.15915494309189535 * (fTemp1659 + 0.1308996938995747);
			double fTemp1661 = fTemp1660 - std::floor(fTemp1660);
			double fTemp1662 = std::sin(6.283185307179586 * fTemp1661) + fSlow5 * std::sin(12.566370614359172 * fTemp1661);
			double fTemp1663 = fTemp1651 - fTemp1662;
			double fTemp1664 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1662 + fSlow11 * fTemp1657));
			double fTemp1665 = 1.0 - mydsp_faustpower2_f(fTemp1664);
			double fTemp1666 = fTemp1664 - fTemp1657;
			double fTemp1667 = fSlow12 * std::fabs(fTemp1666) + 1.0;
			double fTemp1668 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1657 - fSlow4 * (fTemp1663 * (fSlow9 * fTemp1665 + fTemp1666 / (fTemp1667 * (fSlow13 * ((-(fSlow4 * fTemp1663) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1666 / fTemp1667))))) / (1.000000001 - fSlow14 * fTemp1665))));
			double fTemp1669 = fTemp1659 + 0.2617993877991494;
			double fTemp1670 = ((fTemp1669 >= 6.283185307179586) ? fTemp1659 + -6.021385919380437 : fTemp1669);
			double fTemp1671 = 0.15915494309189535 * (fTemp1670 + 0.1308996938995747);
			double fTemp1672 = fTemp1671 - std::floor(fTemp1671);
			double fTemp1673 = std::sin(6.283185307179586 * fTemp1672) + fSlow5 * std::sin(12.566370614359172 * fTemp1672);
			double fTemp1674 = fTemp1662 - fTemp1673;
			double fTemp1675 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1673 + fSlow11 * fTemp1668));
			double fTemp1676 = 1.0 - mydsp_faustpower2_f(fTemp1675);
			double fTemp1677 = fTemp1675 - fTemp1668;
			double fTemp1678 = fSlow12 * std::fabs(fTemp1677) + 1.0;
			double fTemp1679 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1668 - fSlow4 * (fTemp1674 * (fSlow9 * fTemp1676 + fTemp1677 / (fTemp1678 * (fSlow13 * ((-(fSlow4 * fTemp1674) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1677 / fTemp1678))))) / (1.000000001 - fSlow14 * fTemp1676))));
			double fTemp1680 = fTemp1670 + 0.2617993877991494;
			double fTemp1681 = ((fTemp1680 >= 6.283185307179586) ? fTemp1670 + -6.021385919380437 : fTemp1680);
			double fTemp1682 = 0.15915494309189535 * (fTemp1681 + 0.1308996938995747);
			double fTemp1683 = fTemp1682 - std::floor(fTemp1682);
			double fTemp1684 = std::sin(6.283185307179586 * fTemp1683) + fSlow5 * std::sin(12.566370614359172 * fTemp1683);
			double fTemp1685 = fTemp1673 - fTemp1684;
			double fTemp1686 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1684 + fSlow11 * fTemp1679));
			double fTemp1687 = 1.0 - mydsp_faustpower2_f(fTemp1686);
			double fTemp1688 = fTemp1686 - fTemp1679;
			double fTemp1689 = fSlow12 * std::fabs(fTemp1688) + 1.0;
			double fTemp1690 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1679 - fSlow4 * (fTemp1685 * (fSlow9 * fTemp1687 + fTemp1688 / (fTemp1689 * (fSlow13 * ((-(fSlow4 * fTemp1685) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1688 / fTemp1689))))) / (1.000000001 - fSlow14 * fTemp1687))));
			double fTemp1691 = fTemp1681 + 0.2617993877991494;
			double fTemp1692 = ((fTemp1691 >= 6.283185307179586) ? fTemp1681 + -6.021385919380437 : fTemp1691);
			double fTemp1693 = 0.15915494309189535 * (fTemp1692 + 0.1308996938995747);
			double fTemp1694 = fTemp1693 - std::floor(fTemp1693);
			double fTemp1695 = std::sin(6.283185307179586 * fTemp1694) + fSlow5 * std::sin(12.566370614359172 * fTemp1694);
			double fTemp1696 = fTemp1684 - fTemp1695;
			double fTemp1697 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1695 + fSlow11 * fTemp1690));
			double fTemp1698 = 1.0 - mydsp_faustpower2_f(fTemp1697);
			double fTemp1699 = fTemp1697 - fTemp1690;
			double fTemp1700 = fSlow12 * std::fabs(fTemp1699) + 1.0;
			double fTemp1701 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1690 - fSlow4 * (fTemp1696 * (fSlow9 * fTemp1698 + fTemp1699 / (fTemp1700 * (fSlow13 * ((-(fSlow4 * fTemp1696) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1699 / fTemp1700))))) / (1.000000001 - fSlow14 * fTemp1698))));
			double fTemp1702 = fTemp1692 + 0.2617993877991494;
			double fTemp1703 = ((fTemp1702 >= 6.283185307179586) ? fTemp1692 + -6.021385919380437 : fTemp1702);
			double fTemp1704 = 0.15915494309189535 * (fTemp1703 + 0.1308996938995747);
			double fTemp1705 = fTemp1704 - std::floor(fTemp1704);
			double fTemp1706 = std::sin(6.283185307179586 * fTemp1705) + fSlow5 * std::sin(12.566370614359172 * fTemp1705);
			double fTemp1707 = fTemp1695 - fTemp1706;
			double fTemp1708 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1706 + fSlow11 * fTemp1701));
			double fTemp1709 = 1.0 - mydsp_faustpower2_f(fTemp1708);
			double fTemp1710 = fTemp1708 - fTemp1701;
			double fTemp1711 = fSlow12 * std::fabs(fTemp1710) + 1.0;
			double fTemp1712 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1701 - fSlow4 * (fTemp1707 * (fSlow9 * fTemp1709 + fTemp1710 / (fTemp1711 * (fSlow13 * ((-(fSlow4 * fTemp1707) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1710 / fTemp1711))))) / (1.000000001 - fSlow14 * fTemp1709))));
			double fTemp1713 = fTemp1703 + 0.2617993877991494;
			double fTemp1714 = ((fTemp1713 >= 6.283185307179586) ? fTemp1703 + -6.021385919380437 : fTemp1713);
			double fTemp1715 = 0.15915494309189535 * (fTemp1714 + 0.1308996938995747);
			double fTemp1716 = fTemp1715 - std::floor(fTemp1715);
			double fTemp1717 = std::sin(6.283185307179586 * fTemp1716) + fSlow5 * std::sin(12.566370614359172 * fTemp1716);
			double fTemp1718 = fTemp1706 - fTemp1717;
			double fTemp1719 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1717 + fSlow11 * fTemp1712));
			double fTemp1720 = 1.0 - mydsp_faustpower2_f(fTemp1719);
			double fTemp1721 = fTemp1719 - fTemp1712;
			double fTemp1722 = fSlow12 * std::fabs(fTemp1721) + 1.0;
			double fTemp1723 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1712 - fSlow4 * (fTemp1718 * (fSlow9 * fTemp1720 + fTemp1721 / (fTemp1722 * (fSlow13 * ((-(fSlow4 * fTemp1718) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1721 / fTemp1722))))) / (1.000000001 - fSlow14 * fTemp1720))));
			double fTemp1724 = fTemp1714 + 0.2617993877991494;
			double fTemp1725 = ((fTemp1724 >= 6.283185307179586) ? fTemp1714 + -6.021385919380437 : fTemp1724);
			double fTemp1726 = 0.15915494309189535 * (fTemp1725 + 0.1308996938995747);
			double fTemp1727 = fTemp1726 - std::floor(fTemp1726);
			double fTemp1728 = std::sin(6.283185307179586 * fTemp1727) + fSlow5 * std::sin(12.566370614359172 * fTemp1727);
			double fTemp1729 = fTemp1717 - fTemp1728;
			double fTemp1730 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1728 + fSlow11 * fTemp1723));
			double fTemp1731 = 1.0 - mydsp_faustpower2_f(fTemp1730);
			double fTemp1732 = fTemp1730 - fTemp1723;
			double fTemp1733 = fSlow12 * std::fabs(fTemp1732) + 1.0;
			double fTemp1734 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1723 - fSlow4 * (fTemp1729 * (fSlow9 * fTemp1731 + fTemp1732 / (fTemp1733 * (fSlow13 * ((-(fSlow4 * fTemp1729) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1732 / fTemp1733))))) / (1.000000001 - fSlow14 * fTemp1731))));
			double fTemp1735 = fTemp1725 + 0.2617993877991494;
			double fTemp1736 = ((fTemp1735 >= 6.283185307179586) ? fTemp1725 + -6.021385919380437 : fTemp1735);
			double fTemp1737 = 0.15915494309189535 * (fTemp1736 + 0.1308996938995747);
			double fTemp1738 = fTemp1737 - std::floor(fTemp1737);
			double fTemp1739 = std::sin(6.283185307179586 * fTemp1738) + fSlow5 * std::sin(12.566370614359172 * fTemp1738);
			double fTemp1740 = fTemp1728 - fTemp1739;
			double fTemp1741 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1739 + fSlow11 * fTemp1734));
			double fTemp1742 = 1.0 - mydsp_faustpower2_f(fTemp1741);
			double fTemp1743 = fTemp1741 - fTemp1734;
			double fTemp1744 = fSlow12 * std::fabs(fTemp1743) + 1.0;
			double fTemp1745 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1734 - fSlow4 * (fTemp1740 * (fSlow9 * fTemp1742 + fTemp1743 / (fTemp1744 * (fSlow13 * ((-(fSlow4 * fTemp1740) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1743 / fTemp1744))))) / (1.000000001 - fSlow14 * fTemp1742))));
			double fTemp1746 = fTemp1736 + 0.2617993877991494;
			double fTemp1747 = ((fTemp1746 >= 6.283185307179586) ? fTemp1736 + -6.021385919380437 : fTemp1746);
			double fTemp1748 = 0.15915494309189535 * (fTemp1747 + 0.1308996938995747);
			double fTemp1749 = fTemp1748 - std::floor(fTemp1748);
			double fTemp1750 = std::sin(6.283185307179586 * fTemp1749) + fSlow5 * std::sin(12.566370614359172 * fTemp1749);
			double fTemp1751 = fTemp1739 - fTemp1750;
			double fTemp1752 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1750 + fSlow11 * fTemp1745));
			double fTemp1753 = 1.0 - mydsp_faustpower2_f(fTemp1752);
			double fTemp1754 = fTemp1752 - fTemp1745;
			double fTemp1755 = fSlow12 * std::fabs(fTemp1754) + 1.0;
			double fTemp1756 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1745 - fSlow4 * (fTemp1751 * (fSlow9 * fTemp1753 + fTemp1754 / (fTemp1755 * (fSlow13 * ((-(fSlow4 * fTemp1751) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1754 / fTemp1755))))) / (1.000000001 - fSlow14 * fTemp1753))));
			double fTemp1757 = fTemp1747 + 0.2617993877991494;
			double fTemp1758 = ((fTemp1757 >= 6.283185307179586) ? fTemp1747 + -6.021385919380437 : fTemp1757);
			double fTemp1759 = 0.15915494309189535 * (fTemp1758 + 0.1308996938995747);
			double fTemp1760 = fTemp1759 - std::floor(fTemp1759);
			double fTemp1761 = std::sin(6.283185307179586 * fTemp1760) + fSlow5 * std::sin(12.566370614359172 * fTemp1760);
			double fTemp1762 = fTemp1750 - fTemp1761;
			double fTemp1763 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1761 + fSlow11 * fTemp1756));
			double fTemp1764 = 1.0 - mydsp_faustpower2_f(fTemp1763);
			double fTemp1765 = fTemp1763 - fTemp1756;
			double fTemp1766 = fSlow12 * std::fabs(fTemp1765) + 1.0;
			double fTemp1767 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1756 - fSlow4 * (fTemp1762 * (fSlow9 * fTemp1764 + fTemp1765 / (fTemp1766 * (fSlow13 * ((-(fSlow4 * fTemp1762) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1765 / fTemp1766))))) / (1.000000001 - fSlow14 * fTemp1764))));
			double fTemp1768 = fTemp1758 + 0.2617993877991494;
			double fTemp1769 = ((fTemp1768 >= 6.283185307179586) ? fTemp1758 + -6.021385919380437 : fTemp1768);
			double fTemp1770 = 0.15915494309189535 * (fTemp1769 + 0.1308996938995747);
			double fTemp1771 = fTemp1770 - std::floor(fTemp1770);
			double fTemp1772 = std::sin(6.283185307179586 * fTemp1771) + fSlow5 * std::sin(12.566370614359172 * fTemp1771);
			double fTemp1773 = fTemp1761 - fTemp1772;
			double fTemp1774 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1772 + fSlow11 * fTemp1767));
			double fTemp1775 = 1.0 - mydsp_faustpower2_f(fTemp1774);
			double fTemp1776 = fTemp1774 - fTemp1767;
			double fTemp1777 = fSlow12 * std::fabs(fTemp1776) + 1.0;
			double fTemp1778 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1767 - fSlow4 * (fTemp1773 * (fSlow9 * fTemp1775 + fTemp1776 / (fTemp1777 * (fSlow13 * ((-(fSlow4 * fTemp1773) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1776 / fTemp1777))))) / (1.000000001 - fSlow14 * fTemp1775))));
			double fTemp1779 = fTemp1769 + 0.2617993877991494;
			double fTemp1780 = ((fTemp1779 >= 6.283185307179586) ? fTemp1769 + -6.021385919380437 : fTemp1779);
			double fTemp1781 = 0.15915494309189535 * (fTemp1780 + 0.1308996938995747);
			double fTemp1782 = fTemp1781 - std::floor(fTemp1781);
			double fTemp1783 = std::sin(6.283185307179586 * fTemp1782) + fSlow5 * std::sin(12.566370614359172 * fTemp1782);
			double fTemp1784 = fTemp1772 - fTemp1783;
			double fTemp1785 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1783 + fSlow11 * fTemp1778));
			double fTemp1786 = 1.0 - mydsp_faustpower2_f(fTemp1785);
			double fTemp1787 = fTemp1785 - fTemp1778;
			double fTemp1788 = fSlow12 * std::fabs(fTemp1787) + 1.0;
			double fTemp1789 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1778 - fSlow4 * (fTemp1784 * (fSlow9 * fTemp1786 + fTemp1787 / (fTemp1788 * (fSlow13 * ((-(fSlow4 * fTemp1784) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1787 / fTemp1788))))) / (1.000000001 - fSlow14 * fTemp1786))));
			double fTemp1790 = fTemp1780 + 0.2617993877991494;
			double fTemp1791 = ((fTemp1790 >= 6.283185307179586) ? fTemp1780 + -6.021385919380437 : fTemp1790);
			double fTemp1792 = 0.15915494309189535 * (fTemp1791 + 0.1308996938995747);
			double fTemp1793 = fTemp1792 - std::floor(fTemp1792);
			double fTemp1794 = std::sin(6.283185307179586 * fTemp1793) + fSlow5 * std::sin(12.566370614359172 * fTemp1793);
			double fTemp1795 = fTemp1783 - fTemp1794;
			double fTemp1796 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1794 + fSlow11 * fTemp1789));
			double fTemp1797 = 1.0 - mydsp_faustpower2_f(fTemp1796);
			double fTemp1798 = fTemp1796 - fTemp1789;
			double fTemp1799 = fSlow12 * std::fabs(fTemp1798) + 1.0;
			double fTemp1800 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1789 - fSlow4 * (fTemp1795 * (fSlow9 * fTemp1797 + fTemp1798 / (fTemp1799 * (fSlow13 * ((-(fSlow4 * fTemp1795) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1798 / fTemp1799))))) / (1.000000001 - fSlow14 * fTemp1797))));
			double fTemp1801 = fTemp1791 + 0.2617993877991494;
			double fTemp1802 = ((fTemp1801 >= 6.283185307179586) ? fTemp1791 + -6.021385919380437 : fTemp1801);
			double fTemp1803 = 0.15915494309189535 * (fTemp1802 + 0.1308996938995747);
			double fTemp1804 = fTemp1803 - std::floor(fTemp1803);
			double fTemp1805 = std::sin(6.283185307179586 * fTemp1804) + fSlow5 * std::sin(12.566370614359172 * fTemp1804);
			double fTemp1806 = fTemp1794 - fTemp1805;
			double fTemp1807 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1805 + fSlow11 * fTemp1800));
			double fTemp1808 = 1.0 - mydsp_faustpower2_f(fTemp1807);
			double fTemp1809 = fTemp1807 - fTemp1800;
			double fTemp1810 = fSlow12 * std::fabs(fTemp1809) + 1.0;
			double fTemp1811 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1800 - fSlow4 * (fTemp1806 * (fSlow9 * fTemp1808 + fTemp1809 / (fTemp1810 * (fSlow13 * ((-(fSlow4 * fTemp1806) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1809 / fTemp1810))))) / (1.000000001 - fSlow14 * fTemp1808))));
			double fTemp1812 = fTemp1802 + 0.2617993877991494;
			double fTemp1813 = ((fTemp1812 >= 6.283185307179586) ? fTemp1802 + -6.021385919380437 : fTemp1812);
			double fTemp1814 = 0.15915494309189535 * (fTemp1813 + 0.1308996938995747);
			double fTemp1815 = fTemp1814 - std::floor(fTemp1814);
			double fTemp1816 = std::sin(6.283185307179586 * fTemp1815) + fSlow5 * std::sin(12.566370614359172 * fTemp1815);
			double fTemp1817 = fTemp1805 - fTemp1816;
			double fTemp1818 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1816 + fSlow11 * fTemp1811));
			double fTemp1819 = 1.0 - mydsp_faustpower2_f(fTemp1818);
			double fTemp1820 = fTemp1818 - fTemp1811;
			double fTemp1821 = fSlow12 * std::fabs(fTemp1820) + 1.0;
			double fTemp1822 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1811 - fSlow4 * (fTemp1817 * (fSlow9 * fTemp1819 + fTemp1820 / (fTemp1821 * (fSlow13 * ((-(fSlow4 * fTemp1817) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1820 / fTemp1821))))) / (1.000000001 - fSlow14 * fTemp1819))));
			double fTemp1823 = fTemp1813 + 0.2617993877991494;
			double fTemp1824 = ((fTemp1823 >= 6.283185307179586) ? fTemp1813 + -6.021385919380437 : fTemp1823);
			double fTemp1825 = 0.15915494309189535 * (fTemp1824 + 0.1308996938995747);
			double fTemp1826 = fTemp1825 - std::floor(fTemp1825);
			double fTemp1827 = std::sin(6.283185307179586 * fTemp1826) + fSlow5 * std::sin(12.566370614359172 * fTemp1826);
			double fTemp1828 = fTemp1816 - fTemp1827;
			double fTemp1829 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1827 + fSlow11 * fTemp1822));
			double fTemp1830 = 1.0 - mydsp_faustpower2_f(fTemp1829);
			double fTemp1831 = fTemp1829 - fTemp1822;
			double fTemp1832 = fSlow12 * std::fabs(fTemp1831) + 1.0;
			double fTemp1833 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1822 - fSlow4 * (fTemp1828 * (fSlow9 * fTemp1830 + fTemp1831 / (fTemp1832 * (fSlow13 * ((-(fSlow4 * fTemp1828) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1831 / fTemp1832))))) / (1.000000001 - fSlow14 * fTemp1830))));
			double fTemp1834 = fTemp1824 + 0.2617993877991494;
			double fTemp1835 = ((fTemp1834 >= 6.283185307179586) ? fTemp1824 + -6.021385919380437 : fTemp1834);
			double fTemp1836 = 0.15915494309189535 * (fTemp1835 + 0.1308996938995747);
			double fTemp1837 = fTemp1836 - std::floor(fTemp1836);
			double fTemp1838 = std::sin(6.283185307179586 * fTemp1837) + fSlow5 * std::sin(12.566370614359172 * fTemp1837);
			double fTemp1839 = fTemp1827 - fTemp1838;
			double fTemp1840 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1838 + fSlow11 * fTemp1833));
			double fTemp1841 = 1.0 - mydsp_faustpower2_f(fTemp1840);
			double fTemp1842 = fTemp1840 - fTemp1833;
			double fTemp1843 = fSlow12 * std::fabs(fTemp1842) + 1.0;
			double fTemp1844 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1833 - fSlow4 * (fTemp1839 * (fSlow9 * fTemp1841 + fTemp1842 / (fTemp1843 * (fSlow13 * ((-(fSlow4 * fTemp1839) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1842 / fTemp1843))))) / (1.000000001 - fSlow14 * fTemp1841))));
			double fTemp1845 = fTemp1835 + 0.2617993877991494;
			double fTemp1846 = ((fTemp1845 >= 6.283185307179586) ? fTemp1835 + -6.021385919380437 : fTemp1845);
			double fTemp1847 = 0.15915494309189535 * (fTemp1846 + 0.1308996938995747);
			double fTemp1848 = fTemp1847 - std::floor(fTemp1847);
			double fTemp1849 = std::sin(6.283185307179586 * fTemp1848) + fSlow5 * std::sin(12.566370614359172 * fTemp1848);
			double fTemp1850 = fTemp1838 - fTemp1849;
			double fTemp1851 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1849 + fSlow11 * fTemp1844));
			double fTemp1852 = 1.0 - mydsp_faustpower2_f(fTemp1851);
			double fTemp1853 = fTemp1851 - fTemp1844;
			double fTemp1854 = fSlow12 * std::fabs(fTemp1853) + 1.0;
			double fTemp1855 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1844 - fSlow4 * (fTemp1850 * (fSlow9 * fTemp1852 + fTemp1853 / (fTemp1854 * (fSlow13 * ((-(fSlow4 * fTemp1850) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1853 / fTemp1854))))) / (1.000000001 - fSlow14 * fTemp1852))));
			double fTemp1856 = fTemp1846 + 0.2617993877991494;
			double fTemp1857 = ((fTemp1856 >= 6.283185307179586) ? fTemp1846 + -6.021385919380437 : fTemp1856);
			double fTemp1858 = 0.15915494309189535 * (fTemp1857 + 0.1308996938995747);
			double fTemp1859 = fTemp1858 - std::floor(fTemp1858);
			double fTemp1860 = std::sin(6.283185307179586 * fTemp1859) + fSlow5 * std::sin(12.566370614359172 * fTemp1859);
			double fTemp1861 = fTemp1849 - fTemp1860;
			double fTemp1862 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1860 + fSlow11 * fTemp1855));
			double fTemp1863 = 1.0 - mydsp_faustpower2_f(fTemp1862);
			double fTemp1864 = fTemp1862 - fTemp1855;
			double fTemp1865 = fSlow12 * std::fabs(fTemp1864) + 1.0;
			double fTemp1866 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1855 - fSlow4 * (fTemp1861 * (fSlow9 * fTemp1863 + fTemp1864 / (fTemp1865 * (fSlow13 * ((-(fSlow4 * fTemp1861) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1864 / fTemp1865))))) / (1.000000001 - fSlow14 * fTemp1863))));
			double fTemp1867 = fTemp1857 + 0.2617993877991494;
			double fTemp1868 = ((fTemp1867 >= 6.283185307179586) ? fTemp1857 + -6.021385919380437 : fTemp1867);
			double fTemp1869 = 0.15915494309189535 * (fTemp1868 + 0.1308996938995747);
			double fTemp1870 = fTemp1869 - std::floor(fTemp1869);
			double fTemp1871 = std::sin(6.283185307179586 * fTemp1870) + fSlow5 * std::sin(12.566370614359172 * fTemp1870);
			double fTemp1872 = fTemp1860 - fTemp1871;
			double fTemp1873 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1871 + fSlow11 * fTemp1866));
			double fTemp1874 = 1.0 - mydsp_faustpower2_f(fTemp1873);
			double fTemp1875 = fTemp1873 - fTemp1866;
			double fTemp1876 = fSlow12 * std::fabs(fTemp1875) + 1.0;
			double fTemp1877 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1866 - fSlow4 * (fTemp1872 * (fSlow9 * fTemp1874 + fTemp1875 / (fTemp1876 * (fSlow13 * ((-(fSlow4 * fTemp1872) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1875 / fTemp1876))))) / (1.000000001 - fSlow14 * fTemp1874))));
			double fTemp1878 = fTemp1868 + 0.2617993877991494;
			double fTemp1879 = ((fTemp1878 >= 6.283185307179586) ? fTemp1868 + -6.021385919380437 : fTemp1878);
			double fTemp1880 = 0.15915494309189535 * (fTemp1879 + 0.1308996938995747);
			double fTemp1881 = fTemp1880 - std::floor(fTemp1880);
			double fTemp1882 = std::sin(6.283185307179586 * fTemp1881) + fSlow5 * std::sin(12.566370614359172 * fTemp1881);
			double fTemp1883 = fTemp1871 - fTemp1882;
			double fTemp1884 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1882 + fSlow11 * fTemp1877));
			double fTemp1885 = 1.0 - mydsp_faustpower2_f(fTemp1884);
			double fTemp1886 = fTemp1884 - fTemp1877;
			double fTemp1887 = fSlow12 * std::fabs(fTemp1886) + 1.0;
			double fTemp1888 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1877 - fSlow4 * (fTemp1883 * (fSlow9 * fTemp1885 + fTemp1886 / (fTemp1887 * (fSlow13 * ((-(fSlow4 * fTemp1883) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1886 / fTemp1887))))) / (1.000000001 - fSlow14 * fTemp1885))));
			double fTemp1889 = fTemp1879 + 0.2617993877991494;
			double fTemp1890 = ((fTemp1889 >= 6.283185307179586) ? fTemp1879 + -6.021385919380437 : fTemp1889);
			double fTemp1891 = 0.15915494309189535 * (fTemp1890 + 0.1308996938995747);
			double fTemp1892 = fTemp1891 - std::floor(fTemp1891);
			double fTemp1893 = std::sin(6.283185307179586 * fTemp1892) + fSlow5 * std::sin(12.566370614359172 * fTemp1892);
			double fTemp1894 = fTemp1882 - fTemp1893;
			double fTemp1895 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1893 + fSlow11 * fTemp1888));
			double fTemp1896 = 1.0 - mydsp_faustpower2_f(fTemp1895);
			double fTemp1897 = fTemp1895 - fTemp1888;
			double fTemp1898 = fSlow12 * std::fabs(fTemp1897) + 1.0;
			double fTemp1899 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1888 - fSlow4 * (fTemp1894 * (fSlow9 * fTemp1896 + fTemp1897 / (fTemp1898 * (fSlow13 * ((-(fSlow4 * fTemp1894) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1897 / fTemp1898))))) / (1.000000001 - fSlow14 * fTemp1896))));
			double fTemp1900 = fTemp1890 + 0.2617993877991494;
			double fTemp1901 = ((fTemp1900 >= 6.283185307179586) ? fTemp1890 + -6.021385919380437 : fTemp1900);
			double fTemp1902 = 0.15915494309189535 * (fTemp1901 + 0.1308996938995747);
			double fTemp1903 = fTemp1902 - std::floor(fTemp1902);
			double fTemp1904 = std::sin(6.283185307179586 * fTemp1903) + fSlow5 * std::sin(12.566370614359172 * fTemp1903);
			double fTemp1905 = fTemp1893 - fTemp1904;
			double fTemp1906 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1904 + fSlow11 * fTemp1899));
			double fTemp1907 = 1.0 - mydsp_faustpower2_f(fTemp1906);
			double fTemp1908 = fTemp1906 - fTemp1899;
			double fTemp1909 = fSlow12 * std::fabs(fTemp1908) + 1.0;
			double fTemp1910 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1899 - fSlow4 * (fTemp1905 * (fSlow9 * fTemp1907 + fTemp1908 / (fTemp1909 * (fSlow13 * ((-(fSlow4 * fTemp1905) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1908 / fTemp1909))))) / (1.000000001 - fSlow14 * fTemp1907))));
			double fTemp1911 = fTemp1901 + 0.2617993877991494;
			double fTemp1912 = ((fTemp1911 >= 6.283185307179586) ? fTemp1901 + -6.021385919380437 : fTemp1911);
			double fTemp1913 = 0.15915494309189535 * (fTemp1912 + 0.1308996938995747);
			double fTemp1914 = fTemp1913 - std::floor(fTemp1913);
			double fTemp1915 = std::sin(6.283185307179586 * fTemp1914) + fSlow5 * std::sin(12.566370614359172 * fTemp1914);
			double fTemp1916 = fTemp1904 - fTemp1915;
			double fTemp1917 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1915 + fSlow11 * fTemp1910));
			double fTemp1918 = 1.0 - mydsp_faustpower2_f(fTemp1917);
			double fTemp1919 = fTemp1917 - fTemp1910;
			double fTemp1920 = fSlow12 * std::fabs(fTemp1919) + 1.0;
			double fTemp1921 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1910 - fSlow4 * (fTemp1916 * (fSlow9 * fTemp1918 + fTemp1919 / (fTemp1920 * (fSlow13 * ((-(fSlow4 * fTemp1916) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1919 / fTemp1920))))) / (1.000000001 - fSlow14 * fTemp1918))));
			double fTemp1922 = fTemp1912 + 0.2617993877991494;
			double fTemp1923 = ((fTemp1922 >= 6.283185307179586) ? fTemp1912 + -6.021385919380437 : fTemp1922);
			double fTemp1924 = 0.15915494309189535 * (fTemp1923 + 0.1308996938995747);
			double fTemp1925 = fTemp1924 - std::floor(fTemp1924);
			double fTemp1926 = std::sin(6.283185307179586 * fTemp1925) + fSlow5 * std::sin(12.566370614359172 * fTemp1925);
			double fTemp1927 = fTemp1915 - fTemp1926;
			double fTemp1928 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1926 + fSlow11 * fTemp1921));
			double fTemp1929 = 1.0 - mydsp_faustpower2_f(fTemp1928);
			double fTemp1930 = fTemp1928 - fTemp1921;
			double fTemp1931 = fSlow12 * std::fabs(fTemp1930) + 1.0;
			double fTemp1932 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1921 - fSlow4 * (fTemp1927 * (fSlow9 * fTemp1929 + fTemp1930 / (fTemp1931 * (fSlow13 * ((-(fSlow4 * fTemp1927) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1930 / fTemp1931))))) / (1.000000001 - fSlow14 * fTemp1929))));
			double fTemp1933 = fTemp1923 + 0.2617993877991494;
			double fTemp1934 = ((fTemp1933 >= 6.283185307179586) ? fTemp1923 + -6.021385919380437 : fTemp1933);
			double fTemp1935 = 0.15915494309189535 * (fTemp1934 + 0.1308996938995747);
			double fTemp1936 = fTemp1935 - std::floor(fTemp1935);
			double fTemp1937 = std::sin(6.283185307179586 * fTemp1936) + fSlow5 * std::sin(12.566370614359172 * fTemp1936);
			double fTemp1938 = fTemp1926 - fTemp1937;
			double fTemp1939 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1937 + fSlow11 * fTemp1932));
			double fTemp1940 = 1.0 - mydsp_faustpower2_f(fTemp1939);
			double fTemp1941 = fTemp1939 - fTemp1932;
			double fTemp1942 = fSlow12 * std::fabs(fTemp1941) + 1.0;
			double fTemp1943 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1932 - fSlow4 * (fTemp1938 * (fSlow9 * fTemp1940 + fTemp1941 / (fTemp1942 * (fSlow13 * ((-(fSlow4 * fTemp1938) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1941 / fTemp1942))))) / (1.000000001 - fSlow14 * fTemp1940))));
			double fTemp1944 = fTemp1934 + 0.2617993877991494;
			double fTemp1945 = ((fTemp1944 >= 6.283185307179586) ? fTemp1934 + -6.021385919380437 : fTemp1944);
			double fTemp1946 = 0.15915494309189535 * (fTemp1945 + 0.1308996938995747);
			double fTemp1947 = fTemp1946 - std::floor(fTemp1946);
			double fTemp1948 = std::sin(6.283185307179586 * fTemp1947) + fSlow5 * std::sin(12.566370614359172 * fTemp1947);
			double fTemp1949 = fTemp1937 - fTemp1948;
			double fTemp1950 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1948 + fSlow11 * fTemp1943));
			double fTemp1951 = 1.0 - mydsp_faustpower2_f(fTemp1950);
			double fTemp1952 = fTemp1950 - fTemp1943;
			double fTemp1953 = fSlow12 * std::fabs(fTemp1952) + 1.0;
			double fTemp1954 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1943 - fSlow4 * (fTemp1949 * (fSlow9 * fTemp1951 + fTemp1952 / (fTemp1953 * (fSlow13 * ((-(fSlow4 * fTemp1949) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1952 / fTemp1953))))) / (1.000000001 - fSlow14 * fTemp1951))));
			double fTemp1955 = fTemp1945 + 0.2617993877991494;
			double fTemp1956 = ((fTemp1955 >= 6.283185307179586) ? fTemp1945 + -6.021385919380437 : fTemp1955);
			double fTemp1957 = 0.15915494309189535 * (fTemp1956 + 0.1308996938995747);
			double fTemp1958 = fTemp1957 - std::floor(fTemp1957);
			double fTemp1959 = std::sin(6.283185307179586 * fTemp1958) + fSlow5 * std::sin(12.566370614359172 * fTemp1958);
			double fTemp1960 = fTemp1948 - fTemp1959;
			double fTemp1961 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1959 + fSlow11 * fTemp1954));
			double fTemp1962 = 1.0 - mydsp_faustpower2_f(fTemp1961);
			double fTemp1963 = fTemp1961 - fTemp1954;
			double fTemp1964 = fSlow12 * std::fabs(fTemp1963) + 1.0;
			double fTemp1965 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1954 - fSlow4 * (fTemp1960 * (fSlow9 * fTemp1962 + fTemp1963 / (fTemp1964 * (fSlow13 * ((-(fSlow4 * fTemp1960) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1963 / fTemp1964))))) / (1.000000001 - fSlow14 * fTemp1962))));
			double fTemp1966 = fTemp1956 + 0.2617993877991494;
			double fTemp1967 = ((fTemp1966 >= 6.283185307179586) ? fTemp1956 + -6.021385919380437 : fTemp1966);
			double fTemp1968 = 0.15915494309189535 * (fTemp1967 + 0.1308996938995747);
			double fTemp1969 = fTemp1968 - std::floor(fTemp1968);
			double fTemp1970 = std::sin(6.283185307179586 * fTemp1969) + fSlow5 * std::sin(12.566370614359172 * fTemp1969);
			double fTemp1971 = fTemp1959 - fTemp1970;
			double fTemp1972 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1970 + fSlow11 * fTemp1965));
			double fTemp1973 = 1.0 - mydsp_faustpower2_f(fTemp1972);
			double fTemp1974 = fTemp1972 - fTemp1965;
			double fTemp1975 = fSlow12 * std::fabs(fTemp1974) + 1.0;
			double fTemp1976 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1965 - fSlow4 * (fTemp1971 * (fSlow9 * fTemp1973 + fTemp1974 / (fTemp1975 * (fSlow13 * ((-(fSlow4 * fTemp1971) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1974 / fTemp1975))))) / (1.000000001 - fSlow14 * fTemp1973))));
			double fTemp1977 = fTemp1967 + 0.2617993877991494;
			double fTemp1978 = ((fTemp1977 >= 6.283185307179586) ? fTemp1967 + -6.021385919380437 : fTemp1977);
			double fTemp1979 = 0.15915494309189535 * (fTemp1978 + 0.1308996938995747);
			double fTemp1980 = fTemp1979 - std::floor(fTemp1979);
			double fTemp1981 = std::sin(6.283185307179586 * fTemp1980) + fSlow5 * std::sin(12.566370614359172 * fTemp1980);
			double fTemp1982 = fTemp1970 - fTemp1981;
			double fTemp1983 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1981 + fSlow11 * fTemp1976));
			double fTemp1984 = 1.0 - mydsp_faustpower2_f(fTemp1983);
			double fTemp1985 = fTemp1983 - fTemp1976;
			double fTemp1986 = fSlow12 * std::fabs(fTemp1985) + 1.0;
			double fTemp1987 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1976 - fSlow4 * (fTemp1982 * (fSlow9 * fTemp1984 + fTemp1985 / (fTemp1986 * (fSlow13 * ((-(fSlow4 * fTemp1982) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1985 / fTemp1986))))) / (1.000000001 - fSlow14 * fTemp1984))));
			double fTemp1988 = fTemp1978 + 0.2617993877991494;
			double fTemp1989 = ((fTemp1988 >= 6.283185307179586) ? fTemp1978 + -6.021385919380437 : fTemp1988);
			double fTemp1990 = 0.15915494309189535 * (fTemp1989 + 0.1308996938995747);
			double fTemp1991 = fTemp1990 - std::floor(fTemp1990);
			double fTemp1992 = std::sin(6.283185307179586 * fTemp1991) + fSlow5 * std::sin(12.566370614359172 * fTemp1991);
			double fTemp1993 = fTemp1981 - fTemp1992;
			double fTemp1994 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp1992 + fSlow11 * fTemp1987));
			double fTemp1995 = 1.0 - mydsp_faustpower2_f(fTemp1994);
			double fTemp1996 = fTemp1994 - fTemp1987;
			double fTemp1997 = fSlow12 * std::fabs(fTemp1996) + 1.0;
			double fTemp1998 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1987 - fSlow4 * (fTemp1993 * (fSlow9 * fTemp1995 + fTemp1996 / (fTemp1997 * (fSlow13 * ((-(fSlow4 * fTemp1993) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp1996 / fTemp1997))))) / (1.000000001 - fSlow14 * fTemp1995))));
			double fTemp1999 = fTemp1989 + 0.2617993877991494;
			double fTemp2000 = ((fTemp1999 >= 6.283185307179586) ? fTemp1989 + -6.021385919380437 : fTemp1999);
			double fTemp2001 = 0.15915494309189535 * (fTemp2000 + 0.1308996938995747);
			double fTemp2002 = fTemp2001 - std::floor(fTemp2001);
			double fTemp2003 = std::sin(6.283185307179586 * fTemp2002) + fSlow5 * std::sin(12.566370614359172 * fTemp2002);
			double fTemp2004 = fTemp1992 - fTemp2003;
			double fTemp2005 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2003 + fSlow11 * fTemp1998));
			double fTemp2006 = 1.0 - mydsp_faustpower2_f(fTemp2005);
			double fTemp2007 = fTemp2005 - fTemp1998;
			double fTemp2008 = fSlow12 * std::fabs(fTemp2007) + 1.0;
			double fTemp2009 = std::max<double>(-1.0, std::min<double>(1.0, fTemp1998 - fSlow4 * (fTemp2004 * (fSlow9 * fTemp2006 + fTemp2007 / (fTemp2008 * (fSlow13 * ((-(fSlow4 * fTemp2004) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2007 / fTemp2008))))) / (1.000000001 - fSlow14 * fTemp2006))));
			double fTemp2010 = fTemp2000 + 0.2617993877991494;
			double fTemp2011 = ((fTemp2010 >= 6.283185307179586) ? fTemp2000 + -6.021385919380437 : fTemp2010);
			double fTemp2012 = 0.15915494309189535 * (fTemp2011 + 0.1308996938995747);
			double fTemp2013 = fTemp2012 - std::floor(fTemp2012);
			double fTemp2014 = std::sin(6.283185307179586 * fTemp2013) + fSlow5 * std::sin(12.566370614359172 * fTemp2013);
			double fTemp2015 = fTemp2003 - fTemp2014;
			double fTemp2016 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2014 + fSlow11 * fTemp2009));
			double fTemp2017 = 1.0 - mydsp_faustpower2_f(fTemp2016);
			double fTemp2018 = fTemp2016 - fTemp2009;
			double fTemp2019 = fSlow12 * std::fabs(fTemp2018) + 1.0;
			double fTemp2020 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2009 - fSlow4 * (fTemp2015 * (fSlow9 * fTemp2017 + fTemp2018 / (fTemp2019 * (fSlow13 * ((-(fSlow4 * fTemp2015) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2018 / fTemp2019))))) / (1.000000001 - fSlow14 * fTemp2017))));
			double fTemp2021 = fTemp2011 + 0.2617993877991494;
			double fTemp2022 = ((fTemp2021 >= 6.283185307179586) ? fTemp2011 + -6.021385919380437 : fTemp2021);
			double fTemp2023 = 0.15915494309189535 * (fTemp2022 + 0.1308996938995747);
			double fTemp2024 = fTemp2023 - std::floor(fTemp2023);
			double fTemp2025 = std::sin(6.283185307179586 * fTemp2024) + fSlow5 * std::sin(12.566370614359172 * fTemp2024);
			double fTemp2026 = fTemp2014 - fTemp2025;
			double fTemp2027 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2025 + fSlow11 * fTemp2020));
			double fTemp2028 = 1.0 - mydsp_faustpower2_f(fTemp2027);
			double fTemp2029 = fTemp2027 - fTemp2020;
			double fTemp2030 = fSlow12 * std::fabs(fTemp2029) + 1.0;
			double fTemp2031 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2020 - fSlow4 * (fTemp2026 * (fSlow9 * fTemp2028 + fTemp2029 / (fTemp2030 * (fSlow13 * ((-(fSlow4 * fTemp2026) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2029 / fTemp2030))))) / (1.000000001 - fSlow14 * fTemp2028))));
			double fTemp2032 = fTemp2022 + 0.2617993877991494;
			double fTemp2033 = ((fTemp2032 >= 6.283185307179586) ? fTemp2022 + -6.021385919380437 : fTemp2032);
			double fTemp2034 = 0.15915494309189535 * (fTemp2033 + 0.1308996938995747);
			double fTemp2035 = fTemp2034 - std::floor(fTemp2034);
			double fTemp2036 = std::sin(6.283185307179586 * fTemp2035) + fSlow5 * std::sin(12.566370614359172 * fTemp2035);
			double fTemp2037 = fTemp2025 - fTemp2036;
			double fTemp2038 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2036 + fSlow11 * fTemp2031));
			double fTemp2039 = 1.0 - mydsp_faustpower2_f(fTemp2038);
			double fTemp2040 = fTemp2038 - fTemp2031;
			double fTemp2041 = fSlow12 * std::fabs(fTemp2040) + 1.0;
			double fTemp2042 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2031 - fSlow4 * (fTemp2037 * (fSlow9 * fTemp2039 + fTemp2040 / (fTemp2041 * (fSlow13 * ((-(fSlow4 * fTemp2037) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2040 / fTemp2041))))) / (1.000000001 - fSlow14 * fTemp2039))));
			double fTemp2043 = fTemp2033 + 0.2617993877991494;
			double fTemp2044 = ((fTemp2043 >= 6.283185307179586) ? fTemp2033 + -6.021385919380437 : fTemp2043);
			double fTemp2045 = 0.15915494309189535 * (fTemp2044 + 0.1308996938995747);
			double fTemp2046 = fTemp2045 - std::floor(fTemp2045);
			double fTemp2047 = std::sin(6.283185307179586 * fTemp2046) + fSlow5 * std::sin(12.566370614359172 * fTemp2046);
			double fTemp2048 = fTemp2036 - fTemp2047;
			double fTemp2049 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2047 + fSlow11 * fTemp2042));
			double fTemp2050 = 1.0 - mydsp_faustpower2_f(fTemp2049);
			double fTemp2051 = fTemp2049 - fTemp2042;
			double fTemp2052 = fSlow12 * std::fabs(fTemp2051) + 1.0;
			double fTemp2053 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2042 - fSlow4 * (fTemp2048 * (fSlow9 * fTemp2050 + fTemp2051 / (fTemp2052 * (fSlow13 * ((-(fSlow4 * fTemp2048) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2051 / fTemp2052))))) / (1.000000001 - fSlow14 * fTemp2050))));
			double fTemp2054 = fTemp2044 + 0.2617993877991494;
			double fTemp2055 = ((fTemp2054 >= 6.283185307179586) ? fTemp2044 + -6.021385919380437 : fTemp2054);
			double fTemp2056 = 0.15915494309189535 * (fTemp2055 + 0.1308996938995747);
			double fTemp2057 = fTemp2056 - std::floor(fTemp2056);
			double fTemp2058 = std::sin(6.283185307179586 * fTemp2057) + fSlow5 * std::sin(12.566370614359172 * fTemp2057);
			double fTemp2059 = fTemp2047 - fTemp2058;
			double fTemp2060 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2058 + fSlow11 * fTemp2053));
			double fTemp2061 = 1.0 - mydsp_faustpower2_f(fTemp2060);
			double fTemp2062 = fTemp2060 - fTemp2053;
			double fTemp2063 = fSlow12 * std::fabs(fTemp2062) + 1.0;
			double fTemp2064 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2053 - fSlow4 * (fTemp2059 * (fSlow9 * fTemp2061 + fTemp2062 / (fTemp2063 * (fSlow13 * ((-(fSlow4 * fTemp2059) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2062 / fTemp2063))))) / (1.000000001 - fSlow14 * fTemp2061))));
			double fTemp2065 = fTemp2055 + 0.2617993877991494;
			double fTemp2066 = ((fTemp2065 >= 6.283185307179586) ? fTemp2055 + -6.021385919380437 : fTemp2065);
			double fTemp2067 = 0.15915494309189535 * (fTemp2066 + 0.1308996938995747);
			double fTemp2068 = fTemp2067 - std::floor(fTemp2067);
			double fTemp2069 = std::sin(6.283185307179586 * fTemp2068) + fSlow5 * std::sin(12.566370614359172 * fTemp2068);
			double fTemp2070 = fTemp2058 - fTemp2069;
			double fTemp2071 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2069 + fSlow11 * fTemp2064));
			double fTemp2072 = 1.0 - mydsp_faustpower2_f(fTemp2071);
			double fTemp2073 = fTemp2071 - fTemp2064;
			double fTemp2074 = fSlow12 * std::fabs(fTemp2073) + 1.0;
			double fTemp2075 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2064 - fSlow4 * (fTemp2070 * (fSlow9 * fTemp2072 + fTemp2073 / (fTemp2074 * (fSlow13 * ((-(fSlow4 * fTemp2070) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2073 / fTemp2074))))) / (1.000000001 - fSlow14 * fTemp2072))));
			double fTemp2076 = fTemp2066 + 0.2617993877991494;
			double fTemp2077 = ((fTemp2076 >= 6.283185307179586) ? fTemp2066 + -6.021385919380437 : fTemp2076);
			double fTemp2078 = 0.15915494309189535 * (fTemp2077 + 0.1308996938995747);
			double fTemp2079 = fTemp2078 - std::floor(fTemp2078);
			double fTemp2080 = std::sin(6.283185307179586 * fTemp2079) + fSlow5 * std::sin(12.566370614359172 * fTemp2079);
			double fTemp2081 = fTemp2069 - fTemp2080;
			double fTemp2082 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2080 + fSlow11 * fTemp2075));
			double fTemp2083 = 1.0 - mydsp_faustpower2_f(fTemp2082);
			double fTemp2084 = fTemp2082 - fTemp2075;
			double fTemp2085 = fSlow12 * std::fabs(fTemp2084) + 1.0;
			double fTemp2086 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2075 - fSlow4 * (fTemp2081 * (fSlow9 * fTemp2083 + fTemp2084 / (fTemp2085 * (fSlow13 * ((-(fSlow4 * fTemp2081) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2084 / fTemp2085))))) / (1.000000001 - fSlow14 * fTemp2083))));
			double fTemp2087 = fTemp2077 + 0.2617993877991494;
			double fTemp2088 = ((fTemp2087 >= 6.283185307179586) ? fTemp2077 + -6.021385919380437 : fTemp2087);
			double fTemp2089 = 0.15915494309189535 * (fTemp2088 + 0.1308996938995747);
			double fTemp2090 = fTemp2089 - std::floor(fTemp2089);
			double fTemp2091 = std::sin(6.283185307179586 * fTemp2090) + fSlow5 * std::sin(12.566370614359172 * fTemp2090);
			double fTemp2092 = fTemp2080 - fTemp2091;
			double fTemp2093 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2091 + fSlow11 * fTemp2086));
			double fTemp2094 = 1.0 - mydsp_faustpower2_f(fTemp2093);
			double fTemp2095 = fTemp2093 - fTemp2086;
			double fTemp2096 = fSlow12 * std::fabs(fTemp2095) + 1.0;
			double fTemp2097 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2086 - fSlow4 * (fTemp2092 * (fSlow9 * fTemp2094 + fTemp2095 / (fTemp2096 * (fSlow13 * ((-(fSlow4 * fTemp2092) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2095 / fTemp2096))))) / (1.000000001 - fSlow14 * fTemp2094))));
			double fTemp2098 = fTemp2088 + 0.2617993877991494;
			double fTemp2099 = ((fTemp2098 >= 6.283185307179586) ? fTemp2088 + -6.021385919380437 : fTemp2098);
			double fTemp2100 = 0.15915494309189535 * (fTemp2099 + 0.1308996938995747);
			double fTemp2101 = fTemp2100 - std::floor(fTemp2100);
			double fTemp2102 = std::sin(6.283185307179586 * fTemp2101) + fSlow5 * std::sin(12.566370614359172 * fTemp2101);
			double fTemp2103 = fTemp2091 - fTemp2102;
			double fTemp2104 = tanh(fSlow10 * (fTemp1064 + fSlow4 * fTemp2102 + fSlow11 * fTemp2097));
			double fTemp2105 = 1.0 - mydsp_faustpower2_f(fTemp2104);
			double fTemp2106 = fTemp2104 - fTemp2097;
			double fTemp2107 = fSlow12 * std::fabs(fTemp2106) + 1.0;
			double fTemp2108 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2097 - fSlow4 * (fTemp2103 * (fSlow9 * fTemp2105 + fTemp2106 / (fTemp2107 * (fSlow13 * ((-(fSlow4 * fTemp2103) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2106 / fTemp2107))))) / (1.000000001 - fSlow14 * fTemp2105))));
			double fTemp2109 = fTemp2099 + 0.2617993877991494;
			double fTemp2110 = ((fTemp2109 >= 6.283185307179586) ? fTemp2099 + -6.021385919380437 : fTemp2109);
			double fTemp2111 = 0.15915494309189535 * (fTemp2110 + 0.1308996938995747);
			double fTemp2112 = fTemp2111 - std::floor(fTemp2111);
			double fTemp2113 = std::sin(6.283185307179586 * fTemp2112) + fSlow5 * std::sin(12.566370614359172 * fTemp2112);
			double fTemp2114 = fTemp2102 - fTemp2113;
			double fTemp2115 = fTemp1064 + fSlow4 * fTemp2113;
			double fTemp2116 = tanh(fSlow10 * (fTemp2115 + fSlow11 * fTemp2108));
			double fTemp2117 = 1.0 - mydsp_faustpower2_f(fTemp2116);
			double fTemp2118 = fTemp2116 - fTemp2108;
			double fTemp2119 = fSlow12 * std::fabs(fTemp2118) + 1.0;
			double fTemp2120 = std::max<double>(-1.0, std::min<double>(1.0, fTemp2108 - fSlow4 * (fTemp2114 * (fSlow9 * fTemp2117 + fTemp2118 / (fTemp2119 * (fSlow13 * ((-(fSlow4 * fTemp2114) >= 0.0) ? 1.0 : -1.0) + (0.001 - fSlow11 * (fTemp2118 / fTemp2119))))) / (1.000000001 - fSlow14 * fTemp2117))));
			fRec13[0] = fTemp2120;
			fRec14[0] = fTemp2115;
			double fTemp2121 = fTemp2110 + 0.2617993877991494;
			fRec15[0] = ((fTemp2121 >= 6.283185307179586) ? fTemp2110 + -6.021385919380437 : fTemp2121);
			double fRec16 = fTemp2120 + fTemp2108 + fTemp2097 + fTemp2086 + fTemp2075 + fTemp2064 + fTemp2053 + fTemp2042 + fTemp2031 + fTemp2020 + fTemp2009 + fTemp1998 + fTemp1987 + fTemp1976 + fTemp1965 + fTemp1954 + fTemp1943 + fTemp1932 + fTemp1921 + fTemp1910 + fTemp1899 + fTemp1888 + fTemp1877 + fTemp1866 + fTemp1855 + fTemp1844 + fTemp1833 + fTemp1822 + fTemp1811 + fTemp1800 + fTemp1789 + fTemp1778 + fTemp1767 + fTemp1756 + fTemp1745 + fTemp1734 + fTemp1723 + fTemp1712 + fTemp1701 + fTemp1690 + fTemp1679 + fTemp1668 + fTemp1657 + fTemp1646 + fTemp1635 + fTemp1624 + fTemp1613 + fTemp1602 + fTemp1591 + fTemp1580 + fTemp1569 + fTemp1558 + fTemp1547 + fTemp1536 + fTemp1525 + fTemp1514 + fTemp1503 + fTemp1492 + fTemp1481 + fTemp1470 + fTemp1459 + fTemp1448 + fTemp1437 + fTemp1426 + fTemp1415 + fTemp1404 + fTemp1393 + fTemp1382 + fTemp1371 + fTemp1360 + fTemp1349 + fTemp1338 + fTemp1327 + fTemp1316 + fTemp1305 + fTemp1294 + fTemp1283 + fTemp1272 + fTemp1261 + fTemp1250 + fTemp1239 + fTemp1228 + fTemp1217 + fTemp1206 + fTemp1195 + fTemp1184 + fTemp1173 + fTemp1162 + fTemp1151 + fTemp1140 + fTemp1129 + fTemp1118 + fTemp1107 + fTemp1096 + fTemp1074 + fTemp1085;
			double fTemp2122 = 0.010416666666666666 * fRec16;
			double fTemp2123 = fTemp2122 - (fConst2 * fRec10[1] + fRec11[1]);
			fRec10[0] = fRec10[1] + fConst5 * fTemp2123;
			double fTemp2124 = fRec10[1] + fConst4 * fTemp2123;
			fRec11[0] = fRec11[1] + fConst8 * fTemp2124;
			double fTemp2125 = fConst9 * fTemp2123;
			double fRec12 = fTemp2125;
			output1[i0] = static_cast<FAUSTFLOAT>(fSlow1 * (fRec9[0] * fRec12 * fSlow17 / fRec8[0]) + fSlow18 * fTemp1063);
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec3[2] = fRec3[1];
			fRec3[1] = fRec3[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec0[1] = fRec0[0];
			fRec1[1] = fRec1[0];
			fRec9[1] = fRec9[0];
			fRec13[2] = fRec13[1];
			fRec13[1] = fRec13[0];
			fRec14[2] = fRec14[1];
			fRec14[1] = fRec14[0];
			fRec15[2] = fRec15[1];
			fRec15[1] = fRec15[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
		}
	}

};

#if defined(JUCE_POLY)

struct FaustSound : public juce::SynthesiserSound {
    
    bool appliesToNote (int /*midiNoteNumber*/) override        { return true; }
    bool appliesToChannel (int /*midiChannel*/) override        { return true; }
};

// An hybrid JUCE and Faust voice

class FaustVoice : public juce::SynthesiserVoice, public dsp_voice {
    
    private:
        
        std::unique_ptr<juce::AudioBuffer<FAUSTFLOAT>> fBuffer;
        
    public:
        
        FaustVoice(dsp* dsp):dsp_voice(dsp)
        {
            // Allocate buffer for mixing
            fBuffer = std::make_unique<juce::AudioBuffer<FAUSTFLOAT>>(dsp->getNumOutputs(), 8192);
            fDSP->init(juce::SynthesiserVoice::getSampleRate());
        }
        
        bool canPlaySound (juce::SynthesiserSound* sound) override
        {
            return dynamic_cast<FaustSound*> (sound) != nullptr;
        }
        
        void startNote (int midiNoteNumber,
                        float velocity,
                        juce::SynthesiserSound* s,
                        int currentPitchWheelPosition) override
        {
            // Note is triggered
            keyOn(midiNoteNumber, velocity);
        }
        
        void stopNote (float velocity, bool allowTailOff) override
        {
            keyOff(!allowTailOff);
        }
        
        void pitchWheelMoved (int newPitchWheelValue) override
        {
            // not implemented for now
        }
        
        void controllerMoved (int controllerNumber, int newControllerValue) override
        {
            // not implemented for now
        }
        
        void renderNextBlock (juce::AudioBuffer<FAUSTFLOAT>& outputBuffer,
                              int startSample,
                              int numSamples) override
        {
            // Only plays when the voice is active
            if (isVoiceActive()) {
                
                // Play the voice
                compute(numSamples, nullptr, (FAUSTFLOAT**)fBuffer->getArrayOfWritePointers());
                
                // Mix it in outputs
                for (int i = 0; i < fDSP->getNumOutputs(); i++) {
                    outputBuffer.addFrom(i, startSample, *fBuffer, i, 0, numSamples);
                }
            }
        }
    
};

// Decorates the JUCE Synthesiser and adds Faust polyphonic code for GUI handling

class FaustSynthesiser : public juce::Synthesiser, public dsp_voice_group {
    
    private:
        
        juce::Synthesiser fSynth;
    
        static void panic(float val, void* arg)
        {
            static_cast<FaustSynthesiser*>(arg)->allNotesOff(0, false); // 0 stops all voices
        }
  
    public:
        
        FaustSynthesiser():dsp_voice_group(panic, this, true, true)
        {
            setNoteStealingEnabled(true);
        }
        
        virtual ~FaustSynthesiser()
        {
            // Voices will be deallocated by fSynth
            dsp_voice_group::clearVoices();
        }
        
        void addVoice(FaustVoice* voice)
        {
            fSynth.addVoice(voice);
            dsp_voice_group::addVoice(voice);
        }
        
        void addSound(juce::SynthesiserSound* sound)
        {
            fSynth.addSound(sound);
        }
        
        void allNotesOff(int midiChannel, bool allowTailOff)
        {
            fSynth.allNotesOff(midiChannel, allowTailOff);
        }
        
        void setCurrentPlaybackSampleRate (double newRate)
        {
            fSynth.setCurrentPlaybackSampleRate(newRate);
        }
        
        void renderNextBlock (juce::AudioBuffer<float>& outputAudio,
                              const juce::MidiBuffer& inputMidi,
                              int startSample,
                              int numSamples)
        {
            fSynth.renderNextBlock(outputAudio, inputMidi, startSample, numSamples);
        }
        
        void renderNextBlock (juce::AudioBuffer<double>& outputAudio,
                              const juce::MidiBuffer& inputMidi,
                              int startSample,
                              int numSamples)
        {
            fSynth.renderNextBlock(outputAudio, inputMidi, startSample, numSamples);
        }
    
};

#endif

// Using the PluginGuiMagic project (https://foleysfinest.com/developer/pluginguimagic/)

#if defined(PLUGIN_MAGIC)

class FaustPlugInAudioProcessor : public foleys::MagicProcessor, private juce::Timer
{
    
    public:
        FaustPlugInAudioProcessor();
        virtual ~FaustPlugInAudioProcessor() {}
        
        void prepareToPlay (double sampleRate, int samplesPerBlock) override;
        
        bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
        
        void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
        {
            jassert (! isUsingDoublePrecision());
            process (buffer, midiMessages);
    #ifdef MAGIC_LEVEL_SOURCE
            fOutputMeter->pushSamples(buffer);
    #endif
        }
        
        void processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override
        {
            jassert (isUsingDoublePrecision());
            process (buffer, midiMessages);
        }
        
        const juce::String getName() const override;
        
        bool acceptsMidi() const override;
        bool producesMidi() const override;
        double getTailLengthSeconds() const override;
        
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram (int index) override;
        const juce::String getProgramName (int index) override;
        void changeProgramName (int index, const juce::String& newName) override;
        
        void releaseResources() override
        {}
        
        void timerCallback() override;
        
        juce::AudioProcessor::BusesProperties getBusesProperties();
        bool supportsDoublePrecisionProcessing() const override;
      
    #ifdef MAGIC_LEVEL_SOURCE
        foleys::MagicLevelSource* fOutputMeter = nullptr;
    #endif
        juce::AudioProcessorValueTreeState treeState{ *this, nullptr };
    
    #ifdef JUCE_POLY
        std::unique_ptr<FaustSynthesiser> fSynth;
    #else
    #if defined(MIDICTRL)
        std::unique_ptr<juce_midi_handler> fMIDIHandler;
        std::unique_ptr<MidiUI> fMIDIUI;
    #endif
        std::unique_ptr<dsp> fDSP;
    #endif
        
    #if defined(OSCCTRL)
        std::unique_ptr<JuceOSCUI> fOSCUI;
    #endif
        
    #if defined(SOUNDFILE)
        std::unique_ptr<SoundUI> fSoundUI;
    #endif
        
        JuceStateUI fStateUI;
        JuceParameterUI fParameterUI;
        
        std::atomic<bool> fFirstCall = true;
        
    private:
        
        template <typename FloatType>
        void process (juce::AudioBuffer<FloatType>& buffer, juce::MidiBuffer& midiMessages);
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaustPlugInAudioProcessor)
    
};

#else

class FaustPlugInAudioProcessor : public juce::AudioProcessor, private juce::Timer
{

    public:
        
        FaustPlugInAudioProcessor();
        virtual ~FaustPlugInAudioProcessor() {}
        
        void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    
        bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
        
        void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
        {
            jassert (! isUsingDoublePrecision());
            process (buffer, midiMessages);
        }
        
        void processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override
        {
            jassert (isUsingDoublePrecision());
            process (buffer, midiMessages);
        }
    
        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;
        
        const juce::String getName() const override;
        
        bool acceptsMidi() const override;
        bool producesMidi() const override;
        double getTailLengthSeconds() const override;
        
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram (int index) override;
        const juce::String getProgramName (int index) override;
        void changeProgramName (int index, const juce::String& newName) override;
        
        void getStateInformation (juce::MemoryBlock& destData) override;
        void setStateInformation (const void* data, int sizeInBytes) override;
    
        void releaseResources() override
        {}
        
        void timerCallback() override;
    
        juce::AudioProcessor::BusesProperties getBusesProperties();
        bool supportsDoublePrecisionProcessing() const override;
    
    #ifdef JUCE_POLY
        std::unique_ptr<FaustSynthesiser> fSynth;
    #else
    #if defined(MIDICTRL)
        std::unique_ptr<juce_midi_handler> fMIDIHandler;
        std::unique_ptr<MidiUI> fMIDIUI;
    #endif
        std::unique_ptr<dsp> fDSP;
    #endif
        
    #if defined(OSCCTRL)
        std::unique_ptr<JuceOSCUI> fOSCUI;
    #endif
    
    #if defined(SOUNDFILE)
        std::unique_ptr<SoundUI> fSoundUI;
    #endif
    
        JuceStateUI fStateUI;
        JuceParameterUI fParameterUI;
    
        std::atomic<bool> fFirstCall = true;
    
    private:
    
        template <typename FloatType>
        void process (juce::AudioBuffer<FloatType>& buffer, juce::MidiBuffer& midiMessages);
    
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaustPlugInAudioProcessor)
    
};

#endif

class FaustPlugInAudioProcessorEditor : public juce::AudioProcessorEditor
{
    
    public:
        
        FaustPlugInAudioProcessorEditor (FaustPlugInAudioProcessor&);
        virtual ~FaustPlugInAudioProcessorEditor() {}
        
        void paint (juce::Graphics&) override;
        void resized() override;
        
    private:
        
        // This reference is provided as a quick way for your editor to
        // access the processor object that created it.
        FaustPlugInAudioProcessor& processor;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaustPlugInAudioProcessorEditor)
#ifndef PLUGIN_MAGIC        
        JuceGUI fJuceGUI;
#endif    
};

#ifndef PLUGIN_MAGIC
FaustPlugInAudioProcessor::FaustPlugInAudioProcessor()
: juce::AudioProcessor (getBusesProperties()), fParameterUI(this)
#else
FaustPlugInAudioProcessor::FaustPlugInAudioProcessor()
: foleys::MagicProcessor (getBusesProperties()), fParameterUI(this)	
#endif
{
    bool midi_sync = false;
    bool midi = false;
    int nvoices = 0;
    
    mydsp* tmp_dsp = new mydsp();
    MidiMeta::analyse(tmp_dsp, midi, midi_sync, nvoices);
    delete tmp_dsp;
	
#ifdef PLUGIN_MAGIC
#ifdef MAGIC_LOAD_BINARY
    // change magic_xml and magic_xmlSize to match the name of your included
    // XML file from Plugin GUI Magic
    magicState.setGuiValueTree(BinaryData::magic_xml, BinaryData::magic_xmlSize);
#endif
// put other GUI Magic sources here, similar to expression below.
#ifdef MAGIC_LEVEL_SOURCE
    fOutputMeter = magicState.createAndAddObject<foleys::MagicLevelSource>("output");
#endif
#endif
   
#ifdef JUCE_POLY
    assert(nvoices > 0);
    fSynth = std::make_unique<FaustSynthesiser>();
    for (int i = 0; i < nvoices; i++) {
        fSynth->addVoice(new FaustVoice(new mydsp()));
    }
    fSynth->init();
    fSynth->addSound(new FaustSound());
#else
    
    bool group = true;
    
#ifdef POLY2
    assert(nvoices > 0);
    std::cout << "Started with " << nvoices << " voices\n";
    dsp* dsp = new mydsp_poly(new mydsp(), nvoices, true, group);
    
#if MIDICTRL
    if (midi_sync) {
        fDSP = std::make_unique<timed_dsp>(new dsp_sequencer(dsp, new effect()));
    } else {
        fDSP = std::make_unique<dsp_sequencer>(dsp, new effect());
    }
#else
    fDSP = std::make_unique<dsp_sequencer>(dsp, new effect());
#endif
    
#else
    if (nvoices > 0) {
        std::cout << "Started with " << nvoices << " voices\n";
        dsp* dsp = new mydsp_poly(new mydsp(), nvoices, true, group);
        
#if MIDICTRL
        if (midi_sync) {
            fDSP = std::make_unique<timed_dsp>(dsp);
        } else {
            fDSP = std::make_unique<decorator_dsp>(dsp);
        }
#else
        fDSP = std::make_unique<decorator_dsp>(dsp);
#endif
    } else {
#if MIDICTRL
        if (midi_sync) {
            fDSP = std::make_unique<timed_dsp>(new mydsp());
        } else {
            fDSP = std::make_unique<mydsp>();
        }
#else
        fDSP = std::make_unique<mydsp>();
#endif
    }
    
#endif
    
#if defined(MIDICTRL)
    fMIDIHandler = std::make_unique<juce_midi_handler>();
    fMIDIUI = std::make_unique<MidiUI>(fMIDIHandler.get());
    fDSP->buildUserInterface(fMIDIUI.get());
    if (!fMIDIUI->run()) {
        std::cerr << "JUCE MIDI handler cannot be started..." << std::endl;
    }
#endif
    
#endif
    
#if defined(OSCCTRL)
    fOSCUI = std::make_unique<JuceOSCUI>("127.0.0.1", 5510, 5511);
#ifdef JUCE_POLY
    fSynth->buildUserInterface(fOSCUI.get());
#else
    fDSP->buildUserInterface(fOSCUI.get());
#endif
    if (!fOSCUI->run()) {
        std::cerr << "JUCE OSC handler cannot be started..." << std::endl;
    }
#endif
    
#if defined(SOUNDFILE)
    // Use bundle path
    auto file = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory().getParentDirectory().getChildFile("Resources");
    fSoundUI = std::make_unique<SoundUI>(file.getFullPathName().toStdString());
    fDSP->buildUserInterface(fSoundUI.get());
#endif
    
#ifdef JUCE_POLY
    fSynth->buildUserInterface(&fStateUI);
    fSynth->buildUserInterface(&fParameterUI);
    // When no previous state was restored, init DSP controllers with their default values
    if (!fStateUI.fRestored) {
        fSynth->instanceResetUserInterface();
    }
#else
    fDSP->buildUserInterface(&fStateUI);
    fDSP->buildUserInterface(&fParameterUI);
    // When no previous state was restored, init DSP controllers with their default values
    if (!fStateUI.fRestored) {
        fDSP->instanceResetUserInterface();
    }
#endif
    
    startTimerHz(25);
}

juce::AudioProcessor::BusesProperties FaustPlugInAudioProcessor::getBusesProperties()
{
    if (juce::PluginHostType::getPluginLoadedAs() == wrapperType_Standalone) {
        if (FAUST_INPUTS == 0) {
            return BusesProperties().withOutput("Output", juce::AudioChannelSet::canonicalChannelSet(std::min<int>(2, FAUST_OUTPUTS)), true);
        } else {
            return BusesProperties()
            .withInput("Input", juce::AudioChannelSet::canonicalChannelSet(std::min<int>(2, FAUST_INPUTS)), true)
            .withOutput("Output", juce::AudioChannelSet::canonicalChannelSet(std::min<int>(2, FAUST_OUTPUTS)), true);
        }
    } else {
        if (FAUST_INPUTS == 0) {
            return BusesProperties().withOutput("Output", juce::AudioChannelSet::canonicalChannelSet(FAUST_OUTPUTS), true);
        } else {
            return BusesProperties()
            .withInput("Input", juce::AudioChannelSet::canonicalChannelSet(FAUST_INPUTS), true)
            .withOutput("Output", juce::AudioChannelSet::canonicalChannelSet(FAUST_OUTPUTS), true);
        }
    }
}

void FaustPlugInAudioProcessor::timerCallback()
{
    GUI::updateAllGuis();
}

//==============================================================================
const juce::String FaustPlugInAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FaustPlugInAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FaustPlugInAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double FaustPlugInAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FaustPlugInAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int FaustPlugInAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FaustPlugInAudioProcessor::setCurrentProgram (int index)
{}

const juce::String FaustPlugInAudioProcessor::getProgramName (int index)
{
    return juce::String();
}

void FaustPlugInAudioProcessor::changeProgramName (int index, const juce::String& newName)
{}

bool FaustPlugInAudioProcessor::supportsDoublePrecisionProcessing() const
{
    return sizeof(FAUSTFLOAT) == 8;
}

//==============================================================================
void FaustPlugInAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Reset DSP adaptation
    fFirstCall = true;
    
#ifdef JUCE_POLY
    fSynth->setCurrentPlaybackSampleRate (sampleRate);
#else
    
    // Setting the DSP control values has already been done
    // by 'buildUserInterface(&fStateUI)', using the saved values or the default ones.
    // What has to be done to finish the DSP initialization is done now.
    mydsp::classInit(int(sampleRate));
    fDSP->instanceConstants(int(sampleRate));
    fDSP->instanceClear();
    
    // Get latency metadata
    struct LatencyMeta : public Meta {
        
        float fLatencyFrames = -1.f;
        float fLatencySec = -1.f;
        
        void declare(const char* key, const char* value)
        {
            if (std::string(key) == "latency_frames" || std::string(key) == "latency_samples") {
                fLatencyFrames = std::atof(value);
            } else if (std::string(key) == "latency_sec") {
                fLatencySec = std::atof(value);
            }
        }
    };
    
    LatencyMeta meta;
    fDSP->metadata(&meta);
    if (meta.fLatencyFrames > 0) {
        setLatencySamples(meta.fLatencyFrames);
    } else if (meta.fLatencySec > 0) {
        setLatencySamples(meta.fLatencySec * sampleRate);
    }
    
#endif
#ifdef MAGIC_LEVEL_SOURCE
    magicState.prepareToPlay(sampleRate, samplesPerBlock);
    fOutputMeter->setupSource(getMainBusNumOutputChannels(), sampleRate, 500, 200);
#endif
}

bool FaustPlugInAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Always return true and have the DSP adapts its buffer layout with a dsp_adapter (see 'prepareToPlay' and 'process')
    return true;
}

template <typename FloatType>
void FaustPlugInAudioProcessor::process (juce::AudioBuffer<FloatType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    /*
        prepareToPlay is possibly called several times with different values for sampleRate
        and isUsingDoublePrecision() state (this has been seen in particular with VTS3),
        making proper sample format (float/double) and the inputs/outputs layout adaptation
        more complex at this stage.
        
        So adapting the sample format (float/double) and the inputs/outputs layout is done
        once at first process call even if this possibly allocates memory, which is not RT safe.
    */
    if (fFirstCall) {
        fFirstCall = false;
        
        // Possible sample size adaptation
        if (supportsDoublePrecisionProcessing()) {
            if (isUsingDoublePrecision()) {
                // Nothing to do
            } else {
                fDSP = std::make_unique<dsp_sample_adapter<double, float>>(fDSP.release());
            }
        } else {
            if (isUsingDoublePrecision()) {
                fDSP = std::make_unique<dsp_sample_adapter<float, double>>(fDSP.release());
            } else {
                // Nothing to do
            }
        }
        
        // Possibly adapt DSP inputs/outputs number
        if (fDSP->getNumInputs() > getTotalNumInputChannels() || fDSP->getNumOutputs() > getTotalNumOutputChannels()) {
            fDSP = std::make_unique<dsp_adapter>(fDSP.release(), getTotalNumInputChannels(), getTotalNumOutputChannels(), 4096);
        }
    }
    
#ifdef JUCE_POLY
    fSynth->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
#else
#if defined(MIDICTRL)
    // Read MIDI input events from midiMessages
    fMIDIHandler->decodeBuffer(midiMessages);
    // Then write MIDI output events to midiMessages
    fMIDIHandler->encodeBuffer(midiMessages);
#endif
    // MIDI timestamp is expressed in frames
    fDSP->compute(-1, buffer.getNumSamples(),
                  (FAUSTFLOAT**)buffer.getArrayOfReadPointers(),
                  (FAUSTFLOAT**)buffer.getArrayOfWritePointers());
#endif
}

//==============================================================================
#ifndef PLUGIN_MAGIC
bool FaustPlugInAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* FaustPlugInAudioProcessor::createEditor()
{
    return new FaustPlugInAudioProcessorEditor (*this);
}

//==============================================================================
void FaustPlugInAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    fStateUI.getStateInformation(destData);
}

void FaustPlugInAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    fStateUI.setStateInformation(data, sizeInBytes);
}
#endif
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FaustPlugInAudioProcessor();
}

//==============================================================================
#ifndef PLUGIN_MAGIC
FaustPlugInAudioProcessorEditor::FaustPlugInAudioProcessorEditor (FaustPlugInAudioProcessor& p)
: juce::AudioProcessorEditor (&p), processor (p)
{
#ifdef JUCE_POLY
    p.fSynth->buildUserInterface(&fJuceGUI);
#else
    p.fDSP->buildUserInterface(&fJuceGUI);
#endif
    
    addAndMakeVisible(fJuceGUI);
    
    juce::Rectangle<int> recommendedSize = fJuceGUI.getSize();
    setSize (recommendedSize.getWidth(), recommendedSize.getHeight());
}

//==============================================================================
void FaustPlugInAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);
}

void FaustPlugInAudioProcessorEditor::resized()
{
    fJuceGUI.setBounds(getLocalBounds());
}

#endif
// Globals
std::list<GUI*> GUI::fGuiList;
ztimedmap GUI::gTimedZoneMap;

#endif
