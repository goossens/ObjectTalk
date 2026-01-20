//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtAudioUtilities.h"
#include "OtVoice.h"


//
//	OtVoice::Parameters::renderUI
//

bool OtVoice::Parameters::renderUI([[maybe_unused]]float itemWidth) {
	bool changed = false;

	// determine dimensions
	auto frameHeight = ImGui::GetFrameHeightWithSpacing();
	auto width = 0.0f;
	auto height = 0.0f;

	for (auto& oscillator : oscillators) {
		width = std::max(width, oscillator.parameters.getRenderWidth());
		height += frameHeight + oscillator.parameters.getRenderHeight();
	}

	height = std::max(height, frameHeight + filter.filterParameters.getRenderHeight()+ filter.envelopeParameters.getRenderHeight());
	height = std::max(height, frameHeight + amp.parameters.getRenderHeight());

	auto& style = ImGui::GetStyle();
	auto paddingX = style.WindowPadding.x * 2.0f;
	auto paddingY = style.WindowPadding.y * 2.0f;
	width += paddingX;
	height += paddingY;

	// edit oscillators
	ImGui::BeginChild("Oscillators", ImVec2(width, height), ImGuiChildFlags_Borders);

	for (size_t i = 0; i < numberOfOscillators; i++) {
		auto& oscillator = oscillators[i];
		oscillator.parameters.showPulseWidthKnob = true;
		oscillator.parameters.showShapeKnob = true;
		oscillator.parameters.showVolumeKnob = true;
		oscillator.parameters.showTuningButton = true;

		ImGui::PushID(i);
		auto label = fmt::format("Oscillator {}", i + 1);
		changed |= OtUi::headerWithToggleButton(label.c_str(), &oscillator.power);
		if (!oscillator.power) { ImGui::BeginDisabled(); }
		ImGui::PushItemWidth(oscillator.parameters.getRenderWidth() - oscillator.parameters.getLabelWidth());
		changed |= oscillator.parameters.renderUI();
		ImGui::PopItemWidth();
		if (!oscillator.power) { ImGui::EndDisabled(); }
		ImGui::PopID();
	}

	ImGui::EndChild();

	// edit filter
	ImGui::SameLine();
	ImGui::BeginChild("Filter", ImVec2(filter.envelopeParameters.getRenderWidth() + paddingX, height), ImGuiChildFlags_Borders);
	ImGui::PushID("filter");
	changed |= OtUi::headerWithToggleButton("Filter", &filter.filterPower);
	if (!filter.filterPower) { ImGui::BeginDisabled(); }
	ImGui::PushItemWidth(filter.filterParameters.getRenderWidth() - filter.filterParameters.getLabelWidth());
	changed |= filter.filterParameters.renderUI();
	ImGui::PopItemWidth();

	changed |= OtUi::headerWithToggleButton("Filter Envelope", &filter.envelopePower);
	if (!filter.envelopePower) { ImGui::BeginDisabled(); }
	changed |= filter.envelopeParameters.renderUI();
	if (!filter.envelopePower) { ImGui::EndDisabled(); }

	if (!filter.filterPower) { ImGui::EndDisabled(); }
	ImGui::PopID();
	ImGui::EndChild();

	// render amplifier
	ImGui::SameLine();
	ImGui::BeginChild("Amplifier", ImVec2(amp.parameters.getRenderWidth() + paddingX, height), ImGuiChildFlags_Borders);
	ImGui::PushID("amplifier");
	changed |= OtUi::headerWithToggleButton("Amplifier", &amp.power);
	if (!amp.power) { ImGui::BeginDisabled(); }
	changed |= amp.parameters.renderUI();
	if (!amp.power) { ImGui::EndDisabled(); }
	ImGui::PopID();
	ImGui::EndChild();

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
	filterData["power"] = filter.filterPower;
	filter.filterParameters.serialize(&filterData, basedir);
	filterData["envelopePower"] = filter.envelopePower;
	filter.envelopeParameters.serialize(&filterData, basedir);
	(*data)["filter"] = filterData;

	auto ampData = nlohmann::json::object();
	ampData["power"] = amp.power;
	amp.parameters.serialize(&ampData, basedir);
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
		filter.filterPower = (*data)["filter"].value("power", false);
		filter.filterParameters.deserialize(&(*data)["filter"], basedir);
		filter.envelopePower = (*data)["filter"].value("envelopePower", false);
		filter.envelopeParameters.deserialize(&(*data)["filter"], basedir);
	}

	if (data->contains("amp")) {
		amp.power = (*data)["amp"].value("power", true);
		amp.parameters.deserialize(&(*data)["amp"], basedir);
	}
}


//
//	OtVoice::State::noteOn
//

void OtVoice::State::noteOn(Parameters& parameters, int n, int v) {
	// store values
	note = n;
	velocity = v;

	// configure oscillators
	for (auto& oscillator : oscillators) {
		oscillator.frequency = OtAudioUtilities::midiNoteToPitch(note);
	}

	// start envelopes
	filterEnvelopeState.noteOn(parameters.filter.envelopeParameters);
	ampEnvelopeState.noteOn(parameters.amp.parameters);
}


//
//	OtVoice::State::noteOff
//

void OtVoice::State::noteOff(Parameters& parameters) {
	// inform envelopes (if required)
	if (parameters.filter.envelopePower) {
		filterEnvelopeState.noteOff(parameters.filter.envelopeParameters);
	}

	if (parameters.amp.power) {
		ampEnvelopeState.noteOff(parameters.amp.parameters);
	}
}


//
//	OtVoice::State::cancel

void OtVoice::State::cancel(Parameters& parameters) {
	// cancel envelopes
	filterEnvelopeState.cancel(parameters.filter.envelopeParameters);
	ampEnvelopeState.cancel(parameters.amp.parameters);
}


//
//	OtVoice::State::get
//

float OtVoice::State::get(Parameters& parameters) {
	auto sample = 0.0f;
	auto count = 0;

	// mix all active oscillators
	for (size_t i = 0; i < numberOfOscillators; i++) {
		if (parameters.oscillators[i].power) {
			sample += oscillators[i].get(parameters.oscillators[i].parameters);
			count++;
		}
	}

	if (count) {
		sample /= static_cast<float>(count);
	}

	// apply filter
	// sample = OtAudioFilter::apply(sample);
	if (parameters.filter.envelopePower) {
		filterEnvelopeState.process(parameters.filter.envelopeParameters);
	}

	// apply amp envelope
	if (parameters.amp.power) {
		sample *= ampEnvelopeState.process(parameters.amp.parameters);
	}

	return sample;
}
