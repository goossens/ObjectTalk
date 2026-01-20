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
#include <array>
#include <cmath>
#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtAudioSettings.h"


//
//	OtEnvelope
//

class OtEnvelope {
public:
	// envelope phase
	enum class Phase {
		idle,
		attack,
		decay,
		sustain,
		release
	};

	// set of parameters that drive the envelope generator
	class Parameters {
	public:
		// UI to change envelope parameters
		bool renderUI();
		float getRenderWidth();
		float getRenderHeight();

		// (de)serialize envelope parameters
		void serialize(nlohmann::json* data, std::string* basedir);
		void deserialize(nlohmann::json* data, std::string* basedir);

		// properties
		float attackTime = 1.0f;
		float decayTime = 1.0f;
		float sustainLevel = 0.8f;
		float releaseTime = 0.3f;

		// work variables
	// configuration
	static constexpr size_t envelopeDataSize = 1024;
	static constexpr float envelopeHeight = 100.0f;
		std::array<float, envelopeDataSize> graph;
		bool update = true;
	};

	// state of the envelope generator allowing multiple instances with identical parameters
	class State {
	public:
		Phase phase = Phase::idle;
		int duration;
		int currentSample;
		float startValue;
		float valueRange;
		float value = 0.0f;

		// set state
		inline void noteOn(Parameters& parameters) { enterPhase(parameters, Phase::attack); }
		inline void noteOff(Parameters& parameters) { enterPhase(parameters, Phase::release); }
		inline void cancel(Parameters& parameters) { enterPhase(parameters, Phase::idle); }

		// get the next envelope sample
		inline float process(Parameters& parameters) {
			switch (phase) {
				case Phase::idle: break;
				case Phase::attack: updatePhase(parameters, Phase::decay); break;
				case Phase::decay: updatePhase(parameters, Phase::sustain); break;
				case Phase::sustain: break;
				case Phase::release: updatePhase(parameters, Phase::idle); break;
			}

			return value;
		}

		//	process an entire buffer
		inline void process(Parameters& parameters, float* buffer, size_t size) {
			for (size_t i = 0; i < size; i++) {
				*buffer++ *= process(parameters);
			}
		}

		// see if envelope is still active
		inline bool isActive() { return phase != Phase::idle; }

	private:
		// support functions
		inline void configurePhase(float start, float end, float seconds) {
			duration = std::max(1, static_cast<int>(std::floor(seconds * OtAudioSettings::sampleRate)));
			currentSample = 0;
			startValue = start;
			valueRange = end - start;
			value = start;
		}

		inline void enterPhase(Parameters& parameters, Phase nextPhase) {
			phase = nextPhase;

			switch (nextPhase) {
				case Phase::idle: value = 0.0; break;
				case Phase::attack: configurePhase(value, 1.0f, parameters.attackTime); break;
				case Phase::decay: configurePhase(1.0f, parameters.sustainLevel, parameters.decayTime); break;
				case Phase::sustain: value = parameters.sustainLevel; break;
				case Phase::release: configurePhase(value, 0.0f, parameters.releaseTime);
			}
		}

		template <typename T>
		inline T easing(T t) {
			return t * (static_cast<T>(2) - t);
		}

		inline void updatePhase(Parameters& parameters, Phase nextPhase) {
			if (currentSample++ >= duration) {
				enterPhase(parameters, nextPhase);

			} else {
				value = startValue + valueRange * easing(static_cast<float>(currentSample) / static_cast<float>(duration));
			}
		}
	};
};
