//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>

#include "nlohmann/json_fwd.hpp"

#include "OtAudioFilter.h"
#include "OtMidi.h"
#include "OtOscillator.h"


//
//	OtSynth
//

class OtSynth {
public:
	// UI to change synth properties
	bool renderUI(float itemWidth);
	inline float getRenderWidth() { return midi.getRenderWidth(); }
	float getRenderHeight();

	// (de)serialize data
	void serialize(nlohmann::json* data, std::string* basedir);
	void deserialize(nlohmann::json* data, std::string* basedir);

private:
	// properties
	inline static constexpr size_t numberOfOscillators = 4;

	struct Oscillator {
		bool power = false;
		OtOscillator::Parameters parameters;
		OtOscillator::State state;
	};

	struct Filter {
		bool power = false;
		OtAudioFilter::Parameters parameters;
		OtAudioFilter::State state;
	};

	std::array<Oscillator, numberOfOscillators> oscillators;
	Filter filter;
	OtMidi midi;

	// support functions
	bool renderOscillator(size_t id);
	bool renderFilter();
};
