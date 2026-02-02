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
	template<typename T>
	inline static T fraction(T n) {
		// get decimal part of floating point number
		T whole;
		return std::modf(n, &whole);
	}

	template<typename T>
	inline static T square(T n) {
		// calculate square of number
		return n * n;
	}

	template<typename T>
	inline static T blep(T t, T dt) {
		// determine Band-Limited Step
		// adapted from "Phaseshaping Oscillator Algorithms for Musical SoundSynthesis"
		// by Jari Kleimola, Victor Lazzarini, Joseph Timoney, and Vesa Valimaki
		// http://www.acoustics.hut.fi/publications/papers/smc2010-phaseshaping/

		if (t < dt) {
			return -square(t / dt - static_cast<T>(1));

		} else if (t > 1.0 - dt) {
			return square((t - static_cast<T>(1)) / dt + static_cast<T>(1));

		} else {
			return static_cast<T>(0);
		}
	}

	template<typename T>
	inline static T blamp(T t, T dt) {
		if (t < dt) {
			t = t / dt - static_cast<T>(1);
			return -static_cast<T>(1)/ static_cast<T>(3) * square(t) * t;

		} else if (t > static_cast<T>(1) - dt) {
			t = (t - static_cast<T>(1)) / dt + static_cast<T>(1);
			return static_cast<T>(1) / static_cast<T>(3) * square(t) * t;

		} else {
			return static_cast<T>(0);
		}
	}

	inline static float tune(float freq, float octaves, float semitones=0.0f, float cents=0.0f) {
		auto totalCents = (octaves * 1200.0f) + (semitones * 100.0f) + cents;
		auto ratio = std::pow(2.0f, totalCents / 1200.0f);
		return freq * ratio;
	}

	inline static float tuneCV(float cv, float octaves, float semitones=0.0f, float cents=0.0f) {
		auto totalCents = (octaves * 1200.0f) + (semitones * 100.0f) + cents;
		auto offset = totalCents / 1200.0f;
		return cv + offset;
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
