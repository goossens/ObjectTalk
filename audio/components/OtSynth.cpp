//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtSynth.h"


//
//	OtSynth::renderUI
//

bool OtSynth::renderUI(float itemWidth) {
	// setup UI
	ImGui::PushItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 12.0f + ImGui::GetStyle().ItemInnerSpacing.x));
	bool changed = false;

	// edit VCOs
	for (size_t i = 0; i < numberOfOscillators; i++) {
		changed |= renderOscillator(i);
	}

	// edit filter
	changed |= renderFilter();

	// render amplifier
	// changed |= renderAmplifier();

	ImGui::PopItemWidth();
	return changed;
}


//
//	OtSynth::serialize
//

void OtSynth::serialize(nlohmann::json* data, std::string* basedir) {
	auto oscillatorArray = nlohmann::json::array();

	for (auto& oscillator : oscillators) {
		auto oscillatorData = nlohmann::json::object();
		oscillatorData["power"] = oscillator.power;
		oscillator.parameters.serialize(&oscillatorData, basedir);
		oscillatorArray.emplace_back(oscillatorData);
	}

	(*data)["oscillators"] = oscillatorArray;

	auto filterData = nlohmann::json::object();
	filterData["power"] = filter.power;
	filter.parameters.serialize(&filterData, basedir);
	(*data)["filter"] = filterData;
}


//
//	OtSynth::deserialize
//

void OtSynth::deserialize(nlohmann::json* data, std::string* basedir) {
	oscillators.fill(Oscillator{});
	size_t index = 0;

	for (auto& oscillatorData : (*data)["oscillators"]) {
		auto& oscillator = oscillators[index++];
		oscillator.power = oscillatorData.value("power", false);
		oscillator.parameters.deserialize(&oscillatorData, basedir);
	}

	if (data->contains("filter")) {
		filter.power = (*data)["filter"].value("power", false);
		filter.parameters.deserialize(&(*data)["filter"], basedir);
	}
}


//
//	OtSynth::renderOscillator
//

bool OtSynth::renderOscillator(size_t id) {
	bool changed = false;
	ImGui::PushID(id);
	auto left = ImGui::GetCursorScreenPos().x;
	changed |= OtUi::toggleButton("##power", &oscillators[id].power);

	ImGui::SameLine();
	if (!oscillators[id].power) { ImGui::BeginDisabled(); }
	auto buttonWidth = ImGui::CalcItemWidth() - (ImGui::GetCursorScreenPos().x - left);
	auto popup = fmt::format("OscillatorPopup{}", id);
	if (ImGui::Button("Edit", ImVec2(buttonWidth, 0.0f))) { ImGui::OpenPopup(popup.c_str()); }
	if (!oscillators[id].power) { ImGui::EndDisabled(); }
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Oscillator %d", static_cast<int>(id + 1));

	if (ImGui::BeginPopup(popup.c_str())) {
		OtUi::header("Oscillator Editor");
		changed |= oscillators[id].parameters.renderUI();
		ImGui::EndPopup();
	}

	ImGui::PopID();
	return changed;
}


//
//	OtSynth::renderFilter
//

bool OtSynth::renderFilter() {
	bool changed = false;
	ImGui::PushID("filter");
	auto left = ImGui::GetCursorScreenPos().x;
	changed |= OtUi::toggleButton("##power", &filter.power);

	ImGui::SameLine();
	if (!filter.power) { ImGui::BeginDisabled(); }
	auto buttonWidth = ImGui::CalcItemWidth() - (ImGui::GetCursorScreenPos().x - left);
	if (ImGui::Button("Edit", ImVec2(buttonWidth, 0.0f))) { ImGui::OpenPopup("filterPopup"); }
	if (!filter.power) { ImGui::EndDisabled(); }
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::TextUnformatted("Filter");

	if (ImGui::BeginPopup("filterPopup")) {
		ImGui::PushItemWidth(filter.parameters.getRenderWidth() - filter.parameters.getLabelWidth());
		OtUi::header("Filter Editor");
		changed |= filter.parameters.renderUI();
		ImGui::PopItemWidth();
		ImGui::EndPopup();
	}

	ImGui::PopID();
	return changed;
}
