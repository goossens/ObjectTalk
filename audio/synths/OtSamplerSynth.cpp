//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <numeric>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtSamplerSynth.h"


//
//	OtSamplerSynth::OtSamplerSynth
//

OtSamplerSynth::OtSamplerSynth() {
	nextVoice.resize(numberOfVoices);
	std::iota(nextVoice.begin(), nextVoice.end(), 0);
}


//
//	OtSamplerSynth::renderUI
//

bool OtSamplerSynth::renderUI(float itemWidth) {
	ImGui::SetNextItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 4.0f + ImGui::GetStyle().ItemInnerSpacing.x));
	bool changed = sampleFileAsset.renderUI("File");
	renderVoiceUsage(itemWidth);
	return changed;
}


//
//	OtSamplerSynth::getRenderWidth
//

float OtSamplerSynth::getRenderWidth() {
	return 194.0f;
}


//
//	OtSamplerSynth::getRenderHeight
//

float OtSamplerSynth::getRenderHeight() {
	return ImGui::GetFrameHeightWithSpacing() * 3.0f + 20.0f;
}


//
//	OtSamplerSynth::serialize
//

void OtSamplerSynth::serialize(nlohmann::json* data, std::string* basedir) {
	(*data)["sampler"] = OtAssetSerialize(sampleFileAsset.getPath(), basedir);
}


//
//	OtSamplerSynth::deserialize
//

void OtSamplerSynth::deserialize(nlohmann::json* data, std::string* basedir) {
	sampleFileAsset = OtAssetDeserialize(data, "sampler", basedir);
}


//
//	OtSamplerSynth::processMidiMessage
//

void OtSamplerSynth::processMidiMessage(OtMidiMessage message) {
	if (sampleFileAsset.isReady()) {
		if (message->isNoteOn()) {
			noteOn(message->getKeyNumber(), message->getVelocity());

		} else if (message->isNoteOff()) {
			noteOff(message->getKeyNumber(), message->getVelocity());

		} else if (message->isAllNotesOff()) {
			AllNotesOff();
		}
	}
}


//
//	OtSamplerSynth::get
//

void OtSamplerSynth::get(float* buffer, size_t size) {
	// clear output
	std::fill(buffer, buffer + size, 0.0f);

	// add all active voices to output
	if (sampleFileAsset.isReady()) {
		for (auto& voice : voices) {
			if (voice.isActive()) {
				float voiceData[OtAudioSettings::bufferSize];
				voice.get(sampleFileAsset->getSampleFile(), voiceData, OtAudioSettings::bufferSize);

				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					buffer[i] += voiceData[i];
				}
			}
		}
	}
}


//
//	OtSamplerSynth::noteOn
//

void OtSamplerSynth::noteOn(int note, int velocity) {
	// if this note is still active, we reuse it
	auto openVoice = nextVoice.end();

	for (auto i = nextVoice.begin(); i != nextVoice.end() && openVoice == nextVoice.end(); i++) {
		if (voices[*i].isActive() && voices[*i].getNote() == note) {
			openVoice = i;
			voices[*openVoice].cancel();
		}
	}

	// get next available voice (if required)
	for (auto i = nextVoice.begin(); i != nextVoice.end() && openVoice == nextVoice.end(); i++) {
		if (!voices[*i].isActive()) {
			openVoice = i;
		}
	}

	// if we ran out of voices, we steal the oldest voice
	if (openVoice == nextVoice.end()) {
		openVoice = nextVoice.begin();
		voices[*openVoice].cancel();
	}

	// put voice at the end of the list and add index entry
	nextVoice.splice(nextVoice.end(), nextVoice, openVoice);
	index[note] = *openVoice;

	// turn voice on
	voices[*openVoice].noteOn(sampleFileAsset->getSampleFile(), note, velocity);
}


//
//	OtSamplerSynth::noteOff
//

void OtSamplerSynth::noteOff([[maybe_unused]] int note, [[maybe_unused]] int velocity) {
	voices[index[note]].noteOff();
}


//
//	OtSamplerSynth::AllNotesOff
//

void OtSamplerSynth::AllNotesOff() {
	for (auto& voice : voices) {
		if (voice.isActive()) {
			noteOff(voice.getNote(), 0);
		}
	}
}


//
//	OtSamplerSynth::renderVoiceUsage
//

void OtSamplerSynth::renderVoiceUsage(float itemWidth) {
	ImGui::BeginGroup();

	// configuration
	static constexpr const char* label = "Voices in Use";
	static constexpr float voiceHeight = 20.0f;
	static constexpr float voiceGap = 4.0f;
	static constexpr ImU32 voiceColor = IM_COL32(0, 128, 0, 255);

	// render label
	auto labelWidth = ImGui::CalcTextSize(label).x;
	ImGui::TextUnformatted("");
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (itemWidth - labelWidth) * 0.5f);
	ImGui::TextUnformatted(label);

	// render voice status
	auto drawList = ImGui::GetWindowDrawList();
	auto availableVoicesWidth = itemWidth - voiceGap * (numberOfVoices - 1);
	auto voiceWidth = std::floor(availableVoicesWidth / numberOfVoices);
	auto padding = (availableVoicesWidth - (voiceWidth * numberOfVoices)) / 2.0f;

	auto topLeft = ImGui::GetCursorScreenPos() + ImVec2(padding, 0.0f);
	auto bottomRight = topLeft + ImVec2(voiceWidth, voiceHeight);
	auto offset = ImVec2(voiceWidth + voiceGap, 0.0f);

	for (auto& voice : voices) {
		if (voice.isActive()) {
			drawList->AddRectFilled(topLeft, bottomRight, voiceColor);

		} else {
			drawList->AddRect(topLeft, bottomRight, voiceColor);
		}

		topLeft += offset;
		bottomRight += offset;
	}

	ImGui::InvisibleButton("voicesInUse", ImVec2(itemWidth, voiceHeight + ImGui::GetStyle().ItemSpacing.y));
	ImGui::EndGroup();
}
