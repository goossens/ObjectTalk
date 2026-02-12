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


//
//	OtAudioUtilities
//

class OtAudioUtilities {
public:
	inline static float tune(float freq, float cents) {
		auto ratio = std::pow(2.0f, cents / 1200.0f);
		return freq * ratio;
	}

	inline static float linearToDbv(float v) {
		if (v <= 0) {
			return -100.0f;

		} else {
			return 20.0f * std::log10(v);
		}
	}

	inline static float dbvToLinear(float x) {
		return std::pow(10.0f, x / 20.0f);
	}

	inline static float linearToDbu(float v) {
		if (v <= 0) {
			return -100.0f;

		} else {
			return 20.0f * std::log10(v / 0.775f);
		}
	}

	inline static float dbuToLinear(float x) {
		return std::pow(10.0f, x / 20.0f) * 0.775f;
	}

	inline static float freqToCv(float f) {
		return std::log2(f / 440.0f);
	}

	inline static float cvToFreq(float cv) {
		return 440.0f * std::pow(2.0f, cv);
	}

	inline static float midiNoteToPitch(int midiNote) {
		float semitonesAway = static_cast<float>(midiNote - 69);
		return 440.0f * std::pow(2.0f, (semitonesAway / 12.0f));
	}
};
