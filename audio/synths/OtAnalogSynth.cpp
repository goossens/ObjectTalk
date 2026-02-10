//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <numeric>

#include "OtUi.h"

#include "OtAnalogSynth.h"


//
//	OtAnalogSynth::OtAnalogSynth
//

OtAnalogSynth::OtAnalogSynth() {
	nextVoice.resize(numberOfVoices);
	std::iota(nextVoice.begin(), nextVoice.end(), 0);
}


//
//	OtAnalogSynth::renderUI
//

bool OtAnalogSynth::renderUI([[maybe_unused]] float itemWidth) {
	auto width = OtUi::knobWidth(4) + ImGui::GetStyle().WindowPadding.x * 2.0f;
	auto height = OtUi::knobHeight(3) + ImGui::GetFrameHeightWithSpacing() * 4.0f + envelopeHeight + ImGui::GetStyle().WindowPadding.y * 2.0f;
	bool changed = false;

	// render synth components
	changed |= renderVco(width, height); ImGui::SameLine();
	changed |= renderVcf(width, height); ImGui::SameLine();
	changed |= renderVca(width, height);

	return changed;
}


//
//	OtAnalogSynth::processMidiMessage
//

void OtAnalogSynth::processMidiMessage(OtMidiMessage message) {
	if (message->isNoteOn()) {
		noteOn(message->getKeyNumber(), message->getVelocity());

	} else if (message->isNoteOff()) {
		noteOff(message->getKeyNumber(), message->getVelocity());

	} else if (message->isAllNotesOff()) {
		AllNotesOff();
	}
}


//
//	OtAnalogSynth::get
//

void OtAnalogSynth::get(float* buffer, size_t size) {
	auto parameters = getParameters();
	std::fill(buffer, buffer + size, 0.0f);
	float voiceData[OtAudioSettings::bufferSize];

	for (auto& voice : voices) {
		if (voice.isActive()) {
			voice.get(parameters, voiceData, OtAudioSettings::bufferSize);

			for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
				buffer[i] += voiceData[i];
			}
		}
	}
}


//
//	OtAnalogSynth::noteOn
//

void OtAnalogSynth::noteOn(int note, int velocity) {
	// get next available voice
	auto openVoice = nextVoice.end();

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
	voices[*openVoice].noteOn(note, velocity);
}


//
//	OtAnalogSynth::noteOff
//

void OtAnalogSynth::noteOff([[maybe_unused]] int note, [[maybe_unused]] int velocity) {
	voices[index[note]].noteOff();
}


//
//	OtAnalogSynth::AllNotesOff
//

void OtAnalogSynth::AllNotesOff() {
	for (auto& voice : voices) {
		if (voice.isActive()) {
			noteOff(voice.getNote(), 0);
		}
	}
}


//
//	OtAnalogSynth::renderVco
//

bool OtAnalogSynth::renderVco(float width, float height) {
	auto changed = false;

	ImGui::BeginChild("VCO", ImVec2(width, height), ImGuiChildFlags_Borders);
	OtUi::header("Oscillator 1");
	ImGui::PushID("Oscillator 1");
	auto waveform1 = getVcoWaveForm1();

	if (OtAudioUi::waveFormSelector(&waveform1)) {
		setVcoWaveForm1(waveform1);
		changed |= true;
	}

	ImGui::PopID();
	OtUi::header("Oscillator 2");
	ImGui::PushID("Oscillator 2");
	auto waveform2 = getVcoWaveForm2();

	if (OtAudioUi::waveFormSelector(&waveform2)) {
		setVcoWaveForm2(waveform2);
		changed |= true;
	}

	ImGui::PopID();
	OtUi::header("Mixer");
	changed |= editMixerOsc1(); ImGui::SameLine();
	changed |= editMixerOsc2(); ImGui::SameLine();
	changed |= editMixerWhite(); ImGui::SameLine();
	changed |= editMixerPink();

	ImGui::EndChild();
	return changed;
}


//
//	OtAnalogSynth::renderVcf
//

bool OtAnalogSynth::renderVcf(float width, float height) {
	auto changed = false;

	ImGui::BeginChild("VCF", ImVec2(width, height), ImGuiChildFlags_Borders);
	OtUi::header("Filter");
	changed |= editVcfCutoff(); ImGui::SameLine();
	changed |= editVcfRes(); ImGui::SameLine();
	changed |= editVcfEnv(); ImGui::SameLine();
	changed |= editVcfLfo();

	OtUi::header("Envelope");
	vcfAdsrState.attack = getVcfAttack();
	vcfAdsrState.decay = getVcfDecay();
	vcfAdsrState.sustain = getVcfSustain();
	vcfAdsrState.release = getVcfRelease();

	OtAudioUi::adsrEnvelope("##ADSR", vcfAdsrState, ImVec2(OtUi::knobWidth(4), envelopeHeight));

	changed |= editVcfAttack(); ImGui::SameLine();
	changed |= editVcfDecay(); ImGui::SameLine();
	changed |= editVcfSustain(); ImGui::SameLine();
	changed |= editVcfRelease();
	vcfAdsrState.update |= changed;

	OtUi::header("LFO");
	auto waveform = getVcfWaveForm();

	if (OtAudioUi::waveFormSelector(&waveform)) {
		setVcfWaveForm(waveform);
		changed |= true;
	}

	changed |= editVcfFreq();
	ImGui::EndChild();
	return changed;
}


//
//	OtAnalogSynth::renderVca
//

bool OtAnalogSynth::renderVca(float width, float height) {
	auto changed = false;

	ImGui::BeginChild("VCA", ImVec2(width, height), ImGuiChildFlags_Borders);
	OtUi::header("Amplifier");

	OtUi::header("Envelope");
	vcaAdsrState.attack = getVcaAttack();
	vcaAdsrState.decay = getVcaDecay();
	vcaAdsrState.sustain = getVcaSustain();
	vcaAdsrState.release = getVcaRelease();

	OtAudioUi::adsrEnvelope("##ADSR", vcaAdsrState, ImVec2(OtUi::knobWidth(4), envelopeHeight));

	changed |= editVcaAttack(); ImGui::SameLine();
	changed |= editVcaDecay(); ImGui::SameLine();
	changed |= editVcaSustain(); ImGui::SameLine();
	changed |= editVcaRelease();
	vcaAdsrState.update |= changed;

	ImGui::EndChild();
	return changed;
}
