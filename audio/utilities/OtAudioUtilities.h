//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <numbers>
#include <string>


//
//	OtAudioUtilities
//

class OtAudioUtilities {
public:
	inline static float tune(float frequency, float cents) {
		auto ratio = std::pow(2.0f, cents / 1200.0f);
		return frequency * ratio;
	}

	inline static float linearToDbv(float v) {
		return (v <= 0) ? -100.0f : 20.0f * std::log10(v);
	}

	inline static float dbvToLinear(float x) {
		return std::pow(10.0f, x / 20.0f);
	}

	inline static float linearToDbu(float v) {
		return (v <= 0) ? -100.0f : 20.0f * std::log10(v / 0.775f);
	}

	inline static float dbuToLinear(float x) {
		return std::pow(10.0f, x / 20.0f) * 0.775f;
	}

	inline static float frequencyToCv(float frequency) {
		return std::log2(frequency / 440.0f);
	}

	inline static float cvToFrequency(float cv) {
		return 440.0f * std::pow(2.0f, cv);
	}

	inline static float midiNoteToFrequency(int midiNote) {
		float semitonesAway = static_cast<float>(midiNote - 69);
		return 440.0f * std::pow(2.0f, (semitonesAway / 12.0f));
	}

	inline static int frequencyToClosestMidiNote(float frequency) {
		auto note = std::log2(frequency / 440.0f) * 12.0f + 69.0f;
		auto midiNote = static_cast<int>(std::round(note));
		return std::clamp(static_cast<int>(std::round(midiNote)), 12, 127);
	}

	inline static std::string midiNoteToText(int midiNote) {
		static constexpr const char* names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
		int index = midiNote % 12;
		int octave = (midiNote / 12) - 1;
		return names[index] + std::to_string(octave);
	}

	inline static float differenceInCents(float frequency1, float frequency2) {
	    return 1200.0f * std::log2(frequency2 / frequency1);
	}
};
