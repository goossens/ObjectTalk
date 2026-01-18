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

#include "nlohmann/json_fwd.hpp"

#include "OtNumbers.h"

#include "OtAudioSettings.h"


//
//	OtAudioFilter
//

class OtAudioFilter {
public:
	// filter modes
	enum class Mode {
		off,
		lowPass,
		bandPass,
		highPass,
		moogLadder
	};

	static constexpr const char* modes[] = {
		"Off",
		"Low Pass",
		"Band Pass",
		"High Pass",
		"Moog Ladder"
	};

	static constexpr size_t modesCount = sizeof(modes) / sizeof(*modes);

	// set of parameters that drive the filter
	class Parameters {
	public:
		// UI to change filter properties
		bool renderUI();
		float getLabelWidth();
		float getRenderWidth();
		float getRenderHeight();

		// (de)serialize filter parameters
		void serialize(nlohmann::json* data, std::string* basedir);
		void deserialize(nlohmann::json* data, std::string* basedir);

		// properties
		Mode mode = Mode::off;
		float frequency = 1000.0f;
		float resonance = 0.5f;
	};

	// state of the filter allowing multiple instances with identical parameters
	class State {
	public:
		Mode mode = Mode::off;
		float frequency = 1000.0f;
		float resonance = 0.5f;

		float gain = 0.0f;
		float damping = 0.0f;
		float inverseDenominator = 0.0f;

		float cutoff = 0.0f;
		float res = 0.0f;

		float state1 = 0.0f;
		float state2 = 0.0f;

		double stage[4] = {0.0f};
		double delay[4] = {0.0f};

		double p;
		double k;
		double t1;
		double t2;
	};

	//	process a sample
	static float process(Parameters& parameters, State&state, float sample);

	//	filter an entire buffer
	inline static void process(Parameters& parameters, State& state, float* buffer, size_t size) {
		for (size_t i = 0; i < size; i++) {
			*buffer = process(parameters, state, *buffer);
			buffer++;
		}
	}
};
