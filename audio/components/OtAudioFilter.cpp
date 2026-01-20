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
	changed |= OtUi::selectorEnum("Mode", &mode, modes, modesCount);

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
	return ImGui::CalcTextSize("X").x * 4.0f + ImGui::GetStyle().ItemInnerSpacing.x;
}


//
//	OtAudioFilter::Parameters::getRenderWidth
//

float OtAudioFilter::Parameters::getRenderWidth() {
	return OtUi::knobWidth(2) + getLabelWidth();
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
//	OtAudioFilter::State::process
//

float OtAudioFilter::State::process(Parameters& parameters, float sample) {
	if (mode != parameters.mode || frequency != parameters.frequency || resonance != parameters.resonance) {
		mode = parameters.mode ;
		frequency = parameters.frequency;
		resonance = parameters.resonance;

		switch (mode) {
			case Mode::lowPass:
			case Mode::bandPass:
			case Mode::highPass:
				gain = std::tan(OtAudioSettings::pi * frequency * OtAudioSettings::dt);
				damping = std::min(1.0f - resonance, 0.999f);
				inverseDenominator = 1.0f / (1.0f + (2.0f * damping * gain) + gain * gain);
				break;

			case Mode::moogLadder:
				cutoff = 2.0f * frequency * OtAudioSettings::dt;

				p = cutoff * (1.8f - 0.8f * cutoff);
				k = 2.0f * std::sin(cutoff * OtAudioSettings::pi * 0.5f) - 1.0f;
				t1 = (1.0f - p) * 1.386249f;
				t2 = 12.0f + t1 * t1;

				res = resonance * (t2 + 6.0f * t1) / (t2 - 6.0f * t1);
				break;

			default:
				break;
		}

	}

	if (mode == Mode::moogLadder) {
		float x = sample - res * stage[3];

		// 4 cascaded one-pole filters (bilinear transform)
		stage[0] = x * p + delay[0]  * p - k * stage[0];
		stage[1] = stage[0] * p + delay[1] * p - k * stage[1];
		stage[2] = stage[1] * p + delay[2] * p - k * stage[2];
		stage[3] = stage[2] * p + delay[3] * p - k * stage[3];

		// clipping band-limited sigmoid
		stage[3] -= (stage[3] * stage[3] * stage[3]) / 6.0f;

		delay[0] = x;
		delay[1] = stage[0];
		delay[2] = stage[1];
		delay[3] = stage[2];

		return stage[3];

	} else if (mode != Mode::off) {
		auto highPass = (sample - (2.0f * damping + gain) * state1 - state2) * inverseDenominator;
		auto bandPass = highPass * gain + state1;
		auto lowPass = bandPass * gain + state2;

		state1 = gain * highPass + bandPass;
		state2 = gain * bandPass + lowPass;

		switch (mode) {
			case Mode::lowPass:  return lowPass;
			case Mode::bandPass: return bandPass;
			case Mode::highPass: return highPass;
			default: break;
		}
	}

	return sample;
}
