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
	inline static float process(Parameters& parameters, State&state, float sample) {
		if (state.mode != parameters.mode || state.frequency != parameters.frequency || state.resonance != parameters.resonance) {
			state.mode = parameters.mode ;
			state.frequency = parameters.frequency;
			state.resonance = parameters.resonance;

			switch (state.mode) {
				case Mode::lowPass:
				case Mode::bandPass:
				case Mode::highPass:
					state.gain = std::tan(OtAudioSettings::pi * state.frequency * OtAudioSettings::dt);
					state.damping = std::min(1.0f - state.resonance, 0.999f);
					state.inverseDenominator = 1.0f / (1.0f + (2.0f * state.damping * state.gain) + state.gain * state.gain);
					break;

				case Mode::moogLadder:
					state.cutoff = 2.0f * state.frequency * OtAudioSettings::dt;

					state.p = state.cutoff * (1.8f - 0.8f * state.cutoff);
					state.k = 2.0f * std::sin(state.cutoff * OtAudioSettings::pi * 0.5f) - 1.0f;
					state.t1 = (1.0f - state.p) * 1.386249f;
					state.t2 = 12.0f + state.t1 * state.t1;

					state.res = state.resonance * (state.t2 + 6.0f * state.t1) / (state.t2 - 6.0f * state.t1);
					break;

				default:
					break;
			}

		}

		if (state.mode == Mode::moogLadder) {
			float x = sample - state.res * state.stage[3];

			// 4 cascaded one-pole filters (bilinear transform)
			state.stage[0] = x * state.p + state.delay[0]  * state.p - state.k * state.stage[0];
			state.stage[1] = state.stage[0] * state.p + state.delay[1] * state.p - state.k * state.stage[1];
			state.stage[2] = state.stage[1] * state.p + state.delay[2] * state.p - state.k * state.stage[2];
			state.stage[3] = state.stage[2] * state.p + state.delay[3] * state.p - state.k * state.stage[3];

			// clipping band-limited sigmoid
			state.stage[3] -= (state.stage[3] * state.stage[3] * state.stage[3]) / 6.0f;

			state.delay[0] = x;
			state.delay[1] = state.stage[0];
			state.delay[2] = state.stage[1];
			state.delay[3] = state.stage[2];

			return state.stage[3];

		} else if (state.mode != Mode::off) {
			auto highPass = (sample - (2.0f * state.damping + state.gain) * state.state1 - state.state2) * state.inverseDenominator;
			auto bandPass = highPass * state.gain + state.state1;
			auto lowPass = bandPass * state.gain + state.state2;

			state.state1 = state.gain * highPass + bandPass;
			state.state2 = state.gain * bandPass + lowPass;

			switch (state.mode) {
				case Mode::lowPass:  return lowPass;
				case Mode::bandPass: return bandPass;
				case Mode::highPass: return highPass;
				default: break;
			}
		}

		return sample;
	}

	//	filter an entire buffer
	inline static void process(Parameters& parameters, State& state, float* buffer, size_t size) {
		for (size_t i = 0; i < size; i++) {
			*buffer = process(parameters, state, *buffer);
			buffer++;
		}
	}
};
