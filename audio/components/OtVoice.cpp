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
//	OtVoice::noteOn
//

void OtVoice::noteOn(Parameters& parameters, State& state, int note, int velocity) {
	// store information
	state.note = note;
	state.velocity = static_cast<float>(velocity) / 127.0f;

	// configure oscillators
	for (auto& oscillator : state.oscillators) {
		oscillator.frequency = OtAudioUtilities::midiNoteToPitch(note);
	}

	// start envelopes
	OtEnvelope::noteOn(parameters.filter.envelopeParameters, state.filterEnvelopeState);
	OtEnvelope::noteOn(parameters.amp.parameters, state.ampEnvelopeState);
}


//
//	OtVoice::noteOff
//

void OtVoice::noteOff(Parameters& parameters, State& state) {
	// inform envelopes (if required)
	if (parameters.filter.envelopePower) {
		OtEnvelope::noteOff(parameters.filter.envelopeParameters, state.filterEnvelopeState);
	}

	if (parameters.amp.power) {
		OtEnvelope::noteOff(parameters.amp.parameters, state.ampEnvelopeState);
	}
}


//
//	OtVoice::cancel

void OtVoice::cancel(Parameters& parameters, State& state) {
	// cancel envelopes
	OtEnvelope::cancel(parameters.filter.envelopeParameters, state.filterEnvelopeState);
	OtEnvelope::cancel(parameters.amp.parameters, state.ampEnvelopeState);
}


//
//	OtVoice::isActive
//

bool OtVoice::isActive(State& state) {
	return OtEnvelope::isActive(state.filterEnvelopeState) || OtEnvelope::isActive(state.ampEnvelopeState);
}


//
//	OtVoice::get
//

float OtVoice::get(Parameters& parameters, State& state) {
	auto sample = 0.0f;
	auto count = 0;

	// mix all active oscillators
	for (size_t i = 0; i < numberOfOscillators; i++) {
		if (parameters.oscillators[i].power) {
			sample += OtOscillator::get(parameters.oscillators[i].parameters, state.oscillators[i]);
			count++;
		}
	}

	if (count) {
		sample /= static_cast<float>(count);
	}

	// apply filter
	// sample = OtAudioFilter::apply(sample);
	if (parameters.filter.envelopePower) {
		OtEnvelope::process(parameters.filter.envelopeParameters, state.filterEnvelopeState);
	}

	// apply amp envelope
	if (parameters.amp.power) {
		sample *= OtEnvelope::process(parameters.amp.parameters, state.ampEnvelopeState);
	}

	return sample;
}


//
//	OtVoice::get
//

void OtVoice::get(Parameters& parameters, State& state, float* buffer, size_t size) {
	for (size_t i = 0; i < size; i++) {
		*buffer = get(parameters, state);
		buffer++;
	}
}
