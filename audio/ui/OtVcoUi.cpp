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

#include "OtFontAudio.h"
#include "OtUi.h"

#include "OtAudioUi.h"
#include "OtVcoUi.h"


//
//	OtVcoUi::renderUI
//

bool OtVcoUi::renderUI() {
	auto changed = false;
	auto spacing = ImGui::GetStyle().ItemInnerSpacing.x;

	ImGui::PushID("Modes");
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::filterBypass, &parameters.waveForm, 0.0f, "Off");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modSine, &parameters.waveForm, 1.0f, "Sine");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modTriangle, &parameters.waveForm, 2.0f, "Triangle");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modSawDown, &parameters.waveForm, 3.0f, "Shark Tooth");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modSawUp, &parameters.waveForm, 4.0f, "Saw Tooth");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modSquare, &parameters.waveForm, 5.0f, "Square");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modSh, &parameters.waveForm, 6.0f, "Pulse");
	ImGui::PopID();

	if (showingFrequency) {
		changed |= OtUi::knob("Freq", &frequency, 60.0f, 6000.0f, "%.0fhz", true);
	}

	return changed;
}


//
//	OtVcoUi::getRenderWidth
//

float OtVcoUi::getRenderWidth() {
	return OtAudioUi::getAudioButtonWidth() * 7.0f + ImGui::GetStyle().ItemInnerSpacing.x * 6.0f;
}


//
//	OtVcoUi::getRenderHeight
//

float OtVcoUi::getRenderHeight() {
	return ImGui::GetFrameHeightWithSpacing() + (showingFrequency ? OtUi::knobHeight() : 0.0f);
}


//
//	OtVcoUi::serialize
//

void OtVcoUi::serialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	(*data)["waveForm"] = parameters.waveForm;
	(*data)["frequency"] = frequency;
}


//
//	OtVcoUi::deserialize
//

void OtVcoUi::deserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	parameters.waveForm = data->value("waveForm", 1.0f);
	frequency = data->value("frequency", 440.0f);
}
