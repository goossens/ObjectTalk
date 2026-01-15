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
