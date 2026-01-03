//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cmath>

#include "OtNumbers.h"

#include "OtAudioSettings.h"


//
//	OtVirtualAnalogFilter
//

class OtVirtualAnalogFilter {
public:
	// filter modes
	enum class Mode {
		off,
		lowPass,
		bandPass,
		highPass
	};

	static constexpr const char* modes[] = {
		"Off",
		"Low Pass",
		"Band Pass",
		"High Pass"
	};

	static constexpr size_t modesCount = sizeof(modes) / sizeof(*modes);

	// set properties
	inline void set(Mode m, float frequency, float resonance) {
		mode = m;
		gain = std::tan(OtAudioSettings::pi * frequency * OtAudioSettings::dt);
		damping = std::min(1.0f - resonance, 0.999f);
		inverseDenominator = 1.0f / (1.0f + (2.0f * damping * gain) + gain * gain);
	}

	//	process the next sample
	inline float process(float sample) {
		if (mode != Mode::off) {
			auto highPass = (sample - (2.0f * damping + gain) * state1 - state2) * inverseDenominator;
			auto bandPass = highPass * gain + state1;
			auto lowPass = bandPass * gain + state2;

			state1 = gain * highPass + bandPass;
			state2 = gain * bandPass + lowPass;

			switch (mode) {
				case Mode::lowPass:  return lowPass;
				case Mode::bandPass: return bandPass;
				case Mode::highPass: return highPass;
				default: break;
			}
		}

		return sample;
	}

private:
	// properties
	Mode mode = Mode::off;
	float gain = 0.0f;
	float damping = 0.0f;
	float inverseDenominator = 0.0f;

	// work variables
	float state1 = 0.0f;
	float state2 = 0.0f;
};
