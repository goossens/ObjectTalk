//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cmath>


//
//	OtAudioUtilities
//

class OtAudioUtilities {
public:
	inline static float detune(float pitch, float octaves, float semitones=0, float cents=0) {
		double totalCents = (octaves * 1200.0) + (semitones * 100.0) + cents;
		double result = std::pow(2.0, totalCents / 1200.0);
		return static_cast<float>(result * pitch);
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

	inline static float pitchToCv(float f) {
		return std::log2(f / 440.0f);
	}

	inline static float cvToPitch(float cv) {
		return 440.0f * std::pow(2.0f, cv);
	}

	inline static float midiNoteToPitch(int midiNote) {
		float semitonesAway = static_cast<float>(midiNote - 69);
		return 440.0f * std::pow(2.0f, (semitonesAway / 12.0f));
	}
};
