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
#include "OtOscillator.h"


//
//	OtOscillator::Parameters::isReady
//

bool OtOscillator::Parameters::isReady() {
	if (waveForm == WaveForm::wavetable) {
		// handle status changes for wave table
		if (waveTable.isValid() && (waveTableAsset.isNull() || waveTableAsset.isMissing() || waveTableAsset.isInvalid())) {
			waveTable.clear();

		} else if (waveTableAsset.isReady() && waveTableAsset->getWaveTable() != waveTable) {
			waveTable = waveTableAsset->getWaveTable();
		}

		// return current status
		return waveTableAsset.isReady();

	} else if (waveForm == OtOscillator::WaveForm::sample) {
		// handle status changes for sample file
		if (sampleFile.isValid() && (sampleFileAsset.isNull() || sampleFileAsset.isMissing() || sampleFileAsset.isInvalid())) {
			sampleFile.clear();

		} else if (sampleFileAsset.isReady() && sampleFileAsset->getSampleFile() != sampleFile) {
			sampleFile = sampleFileAsset->getSampleFile();
		}

		// return current status
		return sampleFileAsset.isReady();

	} else {
		// the other oscillator modes are always ready (they don't download resources)
		return true;
	}
}


//
//	OtOscillator::Parameters::renderUI
//

bool OtOscillator::Parameters::renderUI() {
	auto changed = false;
	auto spacing = ImGui::GetStyle().ItemInnerSpacing.x;

	ImGui::PushID("Modes");
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::filterBypass, &waveForm, OtOscillator::WaveForm::off, "Off");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modSine, &waveForm, OtOscillator::WaveForm::sine, "Sine");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modSquare, &waveForm, OtOscillator::WaveForm::square, "Square");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modTriangle, &waveForm, OtOscillator::WaveForm::triangle, "Triangle");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::modSawUp, &waveForm, OtOscillator::WaveForm::sawtooth, "Saw Tooth");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::logoAudiobus, &waveForm, OtOscillator::WaveForm::sample, "Sample File");
	ImGui::SameLine(0.0f, spacing);
	changed |= OtAudioUi::audioRadioButton(OtFontAudio::hExpand, &waveForm, OtOscillator::WaveForm::wavetable, "Wave Table");

	ImGui::SameLine();
	OtUi::hSpacer(15.0f);
	auto buttonSize = ImVec2(OtAudioUi::getAudioButtonWidth(), 0.0f);
	changed |= OtUi::latchButton("I", &inverse, buttonSize);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) { ImGui::SetTooltip("Inverse"); }
	ImGui::SameLine(0.0f, spacing);

	if (ImGui::Button("T", buttonSize)) {
		ImGui::OpenPopup("tuningPopup");
	}

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) { ImGui::SetTooltip("Tuning"); }

	if (ImGui::BeginPopup("tuningPopup")) {
		changed |= OtUi::knob("Octaves", &tuningOctaves, -4, +4); ImGui::SameLine();
		changed |= OtUi::knob("Semitones", &tuningSemitones, -12, +12); ImGui::SameLine();
		changed |= OtUi::knob("Cents", &tuningCents, -100, +100);
		ImGui::EndPopup();
	}

	ImGui::PopID();

	if (waveForm == WaveForm::sample) {
		if (sampleFileAsset.renderUI("File")) {
			if (sampleFileAsset.isNull()) {
				sampleFile.clear();

			} else if (sampleFileAsset.isReady()) {
				sampleFile = sampleFileAsset->getSampleFile();

			} else {
				sampleFile.clear();
			}

			changed |= true;
		}
	}

	if (waveForm == WaveForm::wavetable) {
		if (waveTableAsset.renderUI("File")) {
			if (waveTableAsset.isNull()) {
				waveTable.clear();

			} else if (waveTableAsset.isReady()) {
				waveTable = waveTableAsset->getWaveTable();

			} else {
				waveTable.clear();
			}

			changed |= true;
		}
	}

	size_t knobs = 0;

	if (showFrequencyKnob) {
		changed |= OtUi::knob("Freq", &frequency, frequencyKnobLow, frequencyKnobHigh, "%.1fhz", true);
		knobs++;
	}

	if (showPulseWidthKnob) {
		if (knobs) { ImGui::SameLine(); }
		if (waveForm != WaveForm::square) { ImGui::BeginDisabled(); }
		changed |= OtUi::knob("PW", &pulseWidth, 0.0f, 1.0f, "%.2f");
		if (waveForm != WaveForm::square) { ImGui::EndDisabled(); }
		knobs++;
	}

	if (showShapeKnob) {
		if (knobs) { ImGui::SameLine(); }
		if (!(waveForm == WaveForm::wavetable && waveTable.isValid())) { ImGui::BeginDisabled(); }
		changed |= OtUi::knob("Shape", &shape, 0.0f, static_cast<float>(waveTable.getShapeCount()), "%.0f");
		if (!(waveForm == WaveForm::wavetable && waveTable.isValid())) { ImGui::EndDisabled(); }
		knobs++;
	}

	if (showVolumeKnob) {
		if (knobs) { ImGui::SameLine(); }
		changed |= OtUi::knob("Volume", &volume, 0.0f, 10.0f, "%.0f");
	}

	return changed;
}


//
//	OtOscillator::Parameters::getLabelWidth
//

float OtOscillator::Parameters::getLabelWidth() {
	return ImGui::CalcTextSize("X").x * 4.0f + ImGui::GetStyle().ItemInnerSpacing.x;
}


//
//	OtOscillator::Parameters::getRenderWidth
//

float OtOscillator::Parameters::getRenderWidth() {
	return OtAudioUi::getAudioButtonWidth() * 9.0f + ImGui::GetStyle().ItemInnerSpacing.x * 7.0f + 15.0f;
}


//
//	OtOscillator::Parameters::getRenderHeight
//

float OtOscillator::Parameters::getRenderHeight() {
	float height = ImGui::GetFrameHeightWithSpacing();
	size_t knobs = 0;

	if (showFrequencyKnob) { knobs++; }
	if (showPulseWidthKnob) { knobs++; }
	if (showShapeKnob) { knobs++; }
	if (showVolumeKnob) { knobs++; }

	height += OtUi::knobHeight(knobs > 0 ? 1 : 0);

	if (waveForm == WaveForm::sample || waveForm == WaveForm::wavetable) {
		height += ImGui::GetFrameHeightWithSpacing();
	}

	return height;
}


//
//	OtOscillator::Parameters::serialize
//

void OtOscillator::Parameters::serialize(nlohmann::json* data, std::string* basedir) {
	(*data)["waveForm"] = waveForm;
	(*data)["frequency"] = frequency;
	(*data)["pulseWidth"] = pulseWidth;
	(*data)["sampleFileFrequency"] = sampleFileFrequency;
	(*data)["sampleFile"] = OtAssetSerialize(sampleFileAsset.getPath(), basedir);
	(*data)["waveTable"] = OtAssetSerialize(waveTableAsset.getPath(), basedir);
	(*data)["inverse"] = inverse;
	(*data)["tuningOctaves"] = tuningOctaves;
	(*data)["tuningSemitones"] = tuningSemitones;
	(*data)["tuningCents"] = tuningCents;
	(*data)["volume"] = volume;
}


//
//	OtOscillator::Parameters::deserialize
//

void OtOscillator::Parameters::deserialize(nlohmann::json* data, std::string* basedir) {
	waveForm = data->value("waveForm", OtOscillator::WaveForm::sine);
	frequency = data->value("frequency", 440.0f);
	pulseWidth = data->value("pulseWidth", 1.0f);
	sampleFileFrequency = data->value("sampleFileFrequency", 0.0f);
	sampleFileAsset = OtAssetDeserialize(data, "sampleFile", basedir);
	waveTableAsset = OtAssetDeserialize(data, "waveTable", basedir);
	inverse = data->value("inverse", false);
	tuningOctaves = data->value("tuningOctaves", 0.0f);
	tuningSemitones = data->value("tuningSemitones", 0.0f);
	tuningCents = data->value("tuningCents", 0.0f);
	volume = data->value("volume", 7.0f);
}
