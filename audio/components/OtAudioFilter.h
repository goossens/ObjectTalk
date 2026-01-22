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

	// set of parameters that drive the filter
	class Parameters {
	public:
		// see if filter is on
		bool isOn() { return mode != Mode::off; }

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
		float drive = 0.0f;

		// work variables
		bool hasChanged = true;
	};

	// state of the filter allowing multiple instances with identical parameters
	class State {
	public:
		// set frequency modulation (1v/octave, negative is down positive is up)
		inline void setFrequencyModulation(float fm) {
			if (frequencyModulation != fm) {
				frequencyModulation = fm;
				hasChanged = true;
			}
		}

		//	process a sample
		float process(Parameters& parameters, float sample);

		//	filter an entire buffer
		inline void process(Parameters& parameters, float* buffer, size_t size) {
			for (size_t i = 0; i < size; i++) {
				*buffer = process(parameters, *buffer);
				buffer++;
			}
		}

	private:
		// work variables
		float frequencyModulation = 0.0f;
		bool hasChanged = true;
		float frequency;

		float gain;
		float damping;
		float inverseDenominator;

		float cutoff;
		float res;
		float state1;
		float state2 ;
		float stage[4];
		float delay[4];
		float p;
		float k;
	};
};
