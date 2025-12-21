//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "OtAudioSettings.h"


//
//	OtEnvelope
//

class OtEnvelope {
public:
	// envelope state
	enum class State {
		idle,
		attack,
		hold,
		decay,
		sustain,
		release
	};

	// set properties
	inline void setAttackTime(float seconds) { attackTime = seconds; }
	inline void setHoldTime(float seconds) { holdTime = seconds; }
	inline void setDecayTime(float seconds) { decayTime = seconds; }
	inline void setSustainLevel(float level) { sustainLevel = std::clamp(level, 0.01f, 1.0f); }
	inline void setReleaseTime(float seconds) { releaseTime = seconds; }

	// set state
	inline void mute() { enterState(State::idle); }
	inline void noteOn() { enterState(State::attack); }
	inline void noteOff() { enterState(State::release); }

	// process the next sample
	inline float process() {
		switch (state) {
			case State::idle: break;
			case State::attack: updateState(State::hold); break;
			case State::hold: updateState(State::decay); break;
			case State::decay: updateState(State::sustain); break;
			case State::sustain: break;
			case State::release: updateState(State::idle); break;
		}

		return static_cast<float>(value);
	}

	// get state information
	inline State getState() { return state; }
	inline bool isActive() { return state != State::idle; }

	// get envelope as set of samples for visualization purposes
	inline void getVisualization(float* output, size_t size) {
		// get total envelope length and length of each phase in samples
		auto totalLengthInTime = (attackTime + holdTime + decayTime + releaseTime) * 1.25f;
		auto sampleRate = static_cast<float>(size) / totalLengthInTime;
		auto attackSamples = durationInSamples(attackTime, sampleRate);
		auto holdSamples = durationInSamples(holdTime, sampleRate);
		auto decaySamples = durationInSamples(decayTime, sampleRate);
		auto sustainSamples = durationInSamples(totalLengthInTime * 0.2f, sampleRate);
		auto releaseSamples = durationInSamples(releaseTime, sampleRate);
		auto roundingSamples = size - attackSamples - holdSamples - decaySamples - sustainSamples - releaseSamples;

		// populate output buffer
		auto p = output;
		p = fillVisualizationBuffer(p, attackSamples, 0.0f, 1.0f);
		p = fillVisualizationBuffer(p, holdSamples, 1.0f, 1.0f);
		p = fillVisualizationBuffer(p, decaySamples, 1.0f, sustainLevel);
		p = fillVisualizationBuffer(p, sustainSamples, sustainLevel, sustainLevel);
		p = fillVisualizationBuffer(p, releaseSamples, sustainLevel, 0.0f);
		p = fillVisualizationBuffer(p, roundingSamples, 0.0f, 0.0f);
	}

private:
	// properties
	float attackTime;
	float holdTime;
	float decayTime;
	float sustainLevel;
	float releaseTime;

	// work variables
	State state = State::idle;
	int duration;
	int currentSample;
	double startValue;
	double valueRange;
	double value = 0.0f;

	// support functions
	inline int durationInSamples(double seconds, double sampleRate=OtAudioSettings::sampleRate) {
		return std::max(1, static_cast<int>(std::floor(seconds * sampleRate)));
	}

	inline void configureState(float start, float end, float seconds) {
		duration = durationInSamples(seconds);
		currentSample = 0;
		startValue = start;
		valueRange = end - start;
		value = start;
	}

	inline void enterState(State nextState) {
		state = nextState;

		switch (state) {
			case State::idle: value = 0.0; break;
			case State::attack: configureState(static_cast<float>(value), 1.0f, attackTime); break;
			case State::hold: configureState(1.0f, 1.0f, holdTime); break;
			case State::decay: configureState(1.0f, sustainLevel, decayTime); break;
			case State::sustain: value = sustainLevel; break;
			case State::release: configureState(static_cast<float>(value), 0.0f, releaseTime);
		}
	}

	template <typename T>
	inline T easing(T t) {
		return t * (static_cast<T>(2) - t);
	}

	inline void updateState(State nextState) {
		if (currentSample++ >= duration) {
			enterState(nextState);

		} else {
			value = startValue + valueRange * easing(static_cast<double>(currentSample) / static_cast<double>(duration));
		}
	}

	inline float* fillVisualizationBuffer(float* p, int samples, float start, float end) {
		auto range = end - start;

		for (auto i = 0; i < samples; i++) {
			*p++ = start + range * easing(static_cast<float>(i) / static_cast<float>(samples));
		}

		return p;
	}
};
