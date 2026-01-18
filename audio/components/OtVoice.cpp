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

#include "OtAudioUtilities.h"
#include "OtVoice.h"


//
//	OtVoice::Parameters::renderUI
//

bool OtVoice::Parameters::renderUI(float itemWidth) {
	// setup UI
	ImGui::PushItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 12.0f + ImGui::GetStyle().ItemInnerSpacing.x));
	bool changed = false;

	// edit VCOs
	for (size_t i = 0; i < numberOfOscillators; i++) {
		changed |= renderOscillator(oscillators[i], i + 1);
	}

	// edit filter
	changed |= renderFilter();

	// render amplifier
	changed |= renderAmp();

	ImGui::PopItemWidth();
	return changed;
}

//
//	OtVoice::renderOscillator
//

bool OtVoice::Parameters::renderOscillator(Oscillator& oscillator, int id) {
	bool changed = false;
	ImGui::PushID(id);
	auto left = ImGui::GetCursorScreenPos().x;
	changed |= OtUi::toggleButton("##power", &oscillator.power);

	ImGui::SameLine();
	if (!oscillator.power) { ImGui::BeginDisabled(); }
	auto buttonWidth = ImGui::CalcItemWidth() - (ImGui::GetCursorScreenPos().x - left);
	auto popup = fmt::format("OscillatorPopup{}", id);
	if (ImGui::Button("Edit", ImVec2(buttonWidth, 0.0f))) { ImGui::OpenPopup(popup.c_str()); }
	if (!oscillator.power) { ImGui::EndDisabled(); }
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Oscillator %d", id);

	if (ImGui::BeginPopup(popup.c_str())) {
		OtUi::header("Oscillator Editor");
		changed |= oscillator.parameters.renderUI();
		ImGui::EndPopup();
	}

	ImGui::PopID();
	return changed;
}


//
//	OtVoice::renderFilter
//

bool OtVoice::Parameters::renderFilter() {
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


//
//	OtVoice::Parameters::renderAmp
//

bool OtVoice::Parameters::renderAmp() {
	bool changed = false;
	ImGui::PushID("amp");
	auto left = ImGui::GetCursorScreenPos().x;
	changed |= OtUi::toggleButton("##power", &amp.power);

	ImGui::SameLine();
	if (!amp.power) { ImGui::BeginDisabled(); }
	auto buttonWidth = ImGui::CalcItemWidth() - (ImGui::GetCursorScreenPos().x - left);
	if (ImGui::Button("Edit", ImVec2(buttonWidth, 0.0f))) { ImGui::OpenPopup("ampPopup"); }
	if (!amp.power) { ImGui::EndDisabled(); }
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::TextUnformatted("Amp");

	if (ImGui::BeginPopup("ampPopup")) {
		ImGui::PushItemWidth(amp.parameters.getRenderWidth());
		OtUi::header("Amp Editor");
		changed |= amp.parameters.renderUI();
		ImGui::PopItemWidth();
		ImGui::EndPopup();
	}

	ImGui::PopID();
	return changed;
}


//
//	OtVoice::Parameters::serialize
//

void OtVoice::Parameters::serialize(nlohmann::json_abi_v3_12_0::json *data, std::__1::string *basedir) {
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

	auto ampData = nlohmann::json::object();
	ampData["power"] = filter.power;
	filter.parameters.serialize(&ampData, basedir);
	(*data)["amp"] = ampData;
}


//
//	OtVoice::Parameters::deserialize
//

void OtVoice::Parameters::deserialize(nlohmann::json_abi_v3_12_0::json *data, std::__1::string *basedir) {
	oscillators.fill(Parameters::Oscillator{});
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

	if (data->contains("amp")) {
		amp.power = (*data)["amp"].value("power", false);
		amp.parameters.deserialize(&(*data)["amp"], basedir);
	}
}


//
//	OtVoice::State::noteOn
//

void OtVoice::State::noteOn(Parameters* p, int n, int v) {
	// store information
	parameters = p;
	note = n;
	velocity = static_cast<float>(v) / 127.0f;

	// configure oscillators
	for (auto& oscillator : oscillators) {
		oscillator.frequency = OtAudioUtilities::midiNoteToPitch(note);
	}

	// start envelopes

}


//
//	OtVoice::State::noteOff
//

void OtVoice::State::noteOff([[maybe_unused]] int note, [[maybe_unused]] int velocity) {
}


//
//	OtVoice::State::cancel
//

void OtVoice::State::cancel() {
}


//
//	OtVoice::State::isActive
//

bool OtVoice::State::isActive() {
	return false;
}
