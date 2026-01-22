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
#include "OtFaust.h"
#include "OtNoise.h"
#include "OtOscillator.h"


//
//	OtVoice
//

class OtVoice {
public:
	// configurable number of oscillators per voice
	static constexpr size_t numberOfOscillators = 3;

	// set of parameters that drive a voice
	class Parameters {
	public:
		// UI to change envelope parameters
		bool renderUI(float itemWidth);
		inline float getRenderWidth() { return 250.0f; }
		inline float getRenderHeight() { return ImGui::GetFrameHeightWithSpacing() * (numberOfOscillators + 2); }

		// (de)serialize envelope parameters
		void serialize(nlohmann::json* data, std::string* basedir);
		void deserialize(nlohmann::json* data, std::string* basedir);

		// local types
		struct Filter {
			OtAudioFilter::Parameters filterParameters;
			bool envelopePower = false;
			OtEnvelope::Parameters envelopeParameters;
		};

		struct Amp {
			bool power = true;
			OtEnvelope::Parameters parameters;
		};

		// properties
		friend class State;
		std::array<OtOscillator::Parameters, numberOfOscillators> oscillators;
		OtFaustComponent<OtNoise>::Parameters noise;
		Filter filter;
		Amp amp;
	};

	// state of a voice allowing multiple instances with identical parameters
	class State {
	public:
		// voice state
		void noteOn(Parameters& parameters, int note, int velocity);
		void noteOff(Parameters& parameters);
		void cancel(Parameters& parameters);

		// get voice status
		inline bool isActive() { return filterEnvelopeState.isActive() || ampEnvelopeState.isActive(); }
		inline int getNote() { return note; }
		inline int getVelocity() { return velocity; }

		// get the next sample(s)
		float get(Parameters& parameters);

		inline void get(Parameters& parameters, float* buffer, size_t size) {
			for (size_t i = 0; i < size; i++) {
				*buffer = get(parameters);
				buffer++;
			}
		}

	private:
		// properties
		std::array<OtOscillator::State, numberOfOscillators> oscillators;
		OtFaustComponent<OtNoise>::State noise;

		OtAudioFilter::State filter;
		OtEnvelope::State filterEnvelopeState;
		OtEnvelope::State ampEnvelopeState;
		int note;
		int velocity;
	};
};
