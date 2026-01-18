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
		hold,
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
		float holdTime = 0.0f;
		float decayTime = 1.0f;
		float sustainLevel = 0.8f;
		float releaseTime = 0.3f;

		// work variables
		inline static constexpr size_t envelopeDataSize = 1024;
		inline static constexpr float envelopeHeight = 100.0f;
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
	};

	// set state
	inline static void mute(Parameters& parameters, State& state) { enterPhase(parameters, state, Phase::idle); }
	inline static void noteOn(Parameters& parameters, State& state) { enterPhase(parameters, state, Phase::attack); }
	inline static void noteOff(Parameters& parameters, State& state) { enterPhase(parameters, state, Phase::release); }

		// get the next envelope sample
	inline static float process(Parameters& parameters, State& state) {
		switch (state.phase) {
			case Phase::idle: break;
			case Phase::attack: updatePhase(parameters, state, Phase::hold); break;
			case Phase::hold: updatePhase(parameters, state, Phase::decay); break;
			case Phase::decay: updatePhase(parameters, state, Phase::sustain); break;
			case Phase::sustain: break;
			case Phase::release: updatePhase(parameters, state, Phase::idle); break;
		}

		return state.value;
	}

	//	process an entire buffer
	inline static void process(Parameters& parameters, State& state, float* buffer, size_t size) {
		for (size_t i = 0; i < size; i++) {
			*buffer++ *= process(parameters, state);
		}
	}

private:
	// support functions
	inline static int durationInSamples(float seconds, float sampleRate=OtAudioSettings::sampleRate) {
		return std::max(1, static_cast<int>(std::floor(seconds * sampleRate)));
	}


	inline static void configurePhase(State& state, float start, float end, float seconds) {
		state.duration = durationInSamples(seconds);
		state.currentSample = 0;
		state.startValue = start;
		state.valueRange = end - start;
		state.value = start;
	}

	inline static void enterPhase(Parameters& parameters, State& state, Phase nextPhase) {
		state.phase = nextPhase;

		switch (nextPhase) {
			case Phase::idle: state.value = 0.0; break;
			case Phase::attack: configurePhase(state, state.value, 1.0f, parameters.attackTime); break;
			case Phase::hold: configurePhase(state, 1.0f, 1.0f, parameters.holdTime); break;
			case Phase::decay: configurePhase(state, 1.0f, parameters.sustainLevel, parameters.decayTime); break;
			case Phase::sustain: state.value = parameters.sustainLevel; break;
			case Phase::release: configurePhase(state, state.value, 0.0f, parameters.releaseTime);
		}
	}

	template <typename T>
	inline static T easing(T t) {
		return t * (static_cast<T>(2) - t);
	}

	inline static void updatePhase(Parameters& parameters, State& state, Phase nextPhase) {
		if (state.currentSample++ >= state.duration) {
			enterPhase(parameters, state, nextPhase);

		} else {
			state.value = state.startValue + state.valueRange * easing(static_cast<float>(state.currentSample) / static_cast<float>(state.duration));
		}
	}
};
