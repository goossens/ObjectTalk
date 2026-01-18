//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtAudioFilter.h"


//
//	OtAudioFilter::Parameters::renderUI
//

bool OtAudioFilter::Parameters::renderUI() {
	bool changed = false;
	changed |= OtUi::selectorEnum("Filter Mode", &mode, modes, modesCount);

	if (mode != Mode::off) {
		changed |= OtUi::knob("Freq", &frequency, 80.0f, 8000.0f, "%.0fhz", true); ImGui::SameLine();
		changed |= OtUi::knob("Res", &resonance, 0.0f, 1.0f, "%.2f", false);
	}

	return changed;
}


//
//	OtAudioFilter::Parameters::getLabelWidth
//

float OtAudioFilter::Parameters::getLabelWidth() {
	return ImGui::CalcTextSize("X").x * 10.0f + ImGui::GetStyle().ItemInnerSpacing.x;
}


//
//	OtAudioFilter::Parameters::getRenderWidth
//

float OtAudioFilter::Parameters::getRenderWidth() {
	return OtUi::knobWidth(3);
}


//
//	OtAudioFilter::Parameters::getRenderHeight
//

float OtAudioFilter::Parameters::getRenderHeight() {
	float height = ImGui::GetFrameHeightWithSpacing();

	if (mode != Mode::off) {
		height += OtUi::knobHeight();
	}

	return height;
}


//
//	OtAudioFilter::Parameters::serialize
//

void OtAudioFilter::Parameters::serialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	(*data)["mode"] = mode;
	(*data)["frequency"] = frequency;
	(*data)["resonance"] = resonance;
}


//
//	OtAudioFilter::Parameters::deserialize
//

void OtAudioFilter::Parameters::deserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	mode = data->value("mode", Mode::off);
	frequency = data->value("frequency", 1000.0f);
	resonance = data->value("resonance", 0.5f);
}


//
//	OtAudioFilter::process
//

float OtAudioFilter::process(Parameters& parameters, State& state, float sample) {
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
