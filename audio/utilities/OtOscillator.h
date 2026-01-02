//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://github.com/martinfinke/PolyBLEP


#pragma once


//
//	Include files
//

#include <algorithm>

#include "OtWaveTable.h"


//
//	OtOscillator
//

class OtOscillator {
public:
	// waveform types
	enum class WaveForm {
		sine,
		square,
		triangle,
		sawtooth,
		wavetable
	};

	static constexpr const char* waveForms[] = {
		"Sine",
		"Square",
		"Triangle",
		"Saw Tooth",
		"Wave Table"
	};

	static constexpr size_t waveFormCount = sizeof(waveForms) / sizeof(*waveForms);

	// set shape table
	inline void setWaveTable(OtWaveTable* wt) { wavetable = wt; }

	// get the next sample
	float get(WaveForm waveForm, float pitch, float pulseWidth, float shape);

	// synchronize phase
	void synchronize(float phase=0.0f);

private:
	// properties
	OtWaveTable* wavetable;

	// work variables
	float t = 0.0f;
};
