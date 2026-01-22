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
	auto changed = false;

	// determine dimensions
	auto frameHeight = ImGui::GetFrameHeightWithSpacing();
	auto width = 0.0f;
	auto height = 0.0f;

	for (auto& oscillator : oscillators) {
		width = std::max(width, oscillator.getRenderWidth());
		height += frameHeight + oscillator.getRenderHeight();
	}

	height += frameHeight + noise.getRenderHeight();

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
		oscillator.showPulseWidthKnob = true;
		oscillator.showShapeKnob = true;
		oscillator.showVolumeKnob = true;
		oscillator.showTuningButton = true;

		ImGui::PushID(static_cast<int>(i));
		auto label = fmt::format("Oscillator {}", static_cast<int>(i + 1));
		OtUi::header(label.c_str());
		changed |= oscillator.renderUI();
		ImGui::PopID();
	}

	OtUi::header("Noise");
	changed |= noise.renderUI();

	ImGui::EndChild();

	// edit filter
	ImGui::SameLine();
	ImGui::BeginChild("Filter", ImVec2(filter.envelopeParameters.getRenderWidth() + paddingX, height), ImGuiChildFlags_Borders);
	ImGui::PushID("filter");
	OtUi::header("Filter");
	ImGui::PushItemWidth(filter.filterParameters.getRenderWidth() - filter.filterParameters.getLabelWidth());
	changed |= filter.filterParameters.renderUI();
	ImGui::PopItemWidth();

	changed |= OtUi::headerWithToggleButton("Filter Envelope", &filter.envelopePower);
	if (!filter.envelopePower) { ImGui::BeginDisabled(); }
	changed |= filter.envelopeParameters.renderUI();
	if (!filter.envelopePower) { ImGui::EndDisabled(); }

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

void OtVoice::Parameters::serialize(nlohmann::json_abi_v3_12_0::json *data, std::string *basedir) {
	auto oscillatorArray = nlohmann::json::array();

	for (auto& oscillator : oscillators) {
		auto oscillatorData = nlohmann::json::object();
		oscillator.serialize(&oscillatorData, basedir);
		oscillatorArray.emplace_back(oscillatorData);
	}

	(*data)["oscillators"] = oscillatorArray;

	auto noiseData = nlohmann::json::object();
	noise.serialize(&noiseData, basedir);
	(*data)["noise"] = noiseData;

	auto filterData = nlohmann::json::object();
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

void OtVoice::Parameters::deserialize(nlohmann::json_abi_v3_12_0::json *data, std::string *basedir) {
	oscillators.fill(OtOscillator::Parameters{});
	size_t index = 0;

	for (auto& oscillatorData : (*data)["oscillators"]) {
		if (index < numberOfOscillators) {
			oscillators[index++].deserialize(&oscillatorData, basedir);
		}
	}

	if (data->contains("noise")) {
		noise.deserialize(&(*data)["noise"], basedir);
	}

	if (data->contains("filter")) {
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
	for (size_t i = 0; i < numberOfOscillators; i++) {
		oscillators[i].set(parameters.oscillators[i], OtAudioUtilities::midiNoteToPitch(note));
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
		if (parameters.oscillators[i].isOn()) {
			sample += oscillators[i].get(parameters.oscillators[i]);
			count++;
		}
	}

	if (count) {
		sample /= static_cast<float>(count);
	}

	// apply filter
	filter.setFrequencyModulation(
		(parameters.filter.envelopePower)
			? filterEnvelopeState.process(parameters.filter.envelopeParameters)
			: 0.0f);

	sample = filter.process(parameters.filter.filterParameters, sample);

	// apply amp envelope
	if (parameters.amp.power) {
		sample *= ampEnvelopeState.process(parameters.amp.parameters);
	}

	return sample;
}
