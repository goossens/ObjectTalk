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
#include <string>

#include "imgui.h"
#include "nlohmann/json_fwd.hpp"

#include "OtAudioFilter.h"
#include "OtEnvelope.h"
#include "OtOscillator.h"


//
//	OtVoice
//

class OtVoice {
public:
	// configurable number of oscillators per voice
	inline static constexpr size_t numberOfOscillators = 3;

	// set of parameters that drive a voice
	struct Parameters {
		// UI to change envelope parameters
		bool renderUI(float itemWidth);
		inline float getRenderWidth() { return 250.0f; }
		inline float getRenderHeight() { return ImGui::GetFrameHeightWithSpacing() * (numberOfOscillators + 2); }

		// (de)serialize envelope parameters
		void serialize(nlohmann::json* data, std::string* basedir);
		void deserialize(nlohmann::json* data, std::string* basedir);

		// local types
		struct Oscillator {
			bool power = false;
			OtOscillator::Parameters parameters;
		};

		struct Filter {
			bool power = false;
			OtAudioFilter::Parameters parameters;
		};

		struct Amp {
			bool power = false;
			OtEnvelope::Parameters parameters;
		};

		// properties
		std::array<Oscillator, numberOfOscillators> oscillators;
		Filter filter;
		Amp amp;


		// support functions
		bool renderOscillator(struct Oscillator& oscillator, int id);
		bool renderFilter();
		bool renderAmp();
	};

	// state of a voice allowing multiple instances with identical parameters
	struct State {
		// properties
		Parameters* parameters;
		int note;
		float velocity;

		std::array<OtOscillator::State, numberOfOscillators> oscillators;
		OtAudioFilter::State state;

		// voice state
		void noteOn(Parameters* parameters, int note, int velocity);
		void noteOff(int note, int velocity);
		void cancel();

		// voice status
		bool isActive();
		inline int getNote() { return note; }
		inline int getVelocity() { return note; }

		// get the next sample(s)
		static float get(Parameters& parameters, State& state);
		static void get(Parameters& parameters, State& state, float* buffer, size_t size);
	};
};
