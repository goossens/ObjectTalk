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

#include "fmt/format.h"
#include "implot.h"

#include "OtFontAudio.h"

#include "OtAudioUi.h"


//
//	OtAudioUi::decoratedHeader
//

bool OtAudioUi::decoratedHeader(const char* label, float* attenuation, float* tuning, float* power) {
	// track changes
	bool changed = false;

	// determine right side of header
	auto pos = ImGui::GetCursorScreenPos();
	pos.x += ImGui::GetContentRegionAvail().x;

	// determine start of decorations
	auto spacing = ImGui::GetStyle().ItemSpacing.x;

	if (attenuation) { pos.x -= trimSliderWidth(); }
	if (tuning && attenuation) { pos.x -= spacing; }
	if (tuning) { pos.x -= audioButtonWidth(); }
	if (power && (attenuation || tuning)) { pos.x -= spacing; }
	if (power) { pos.x -= OtUi::toggleButtonWidth(); }

	// render basic header
	ImGui::PushID(label);
	header(label);

	// render decorations
	ImGui::SetCursorScreenPos(pos);

	if (attenuation) {
		changed |= trimSlider(attenuation);
	}

	if (tuning) {
		if (attenuation) {
			ImGui::SameLine();
		}

		changed |= tuningPopup(tuning);
	}

	if (power) {
		if (attenuation || tuning) {
			ImGui::SameLine();
		}

		auto id = fmt::format("##{}Power", label);
		changed |= smallToggleButton(id.c_str(), power);
	}

	ImGui::PopID();
	return changed;
}


//
//	OtAudioUi::audioButtonWidth
//

float OtAudioUi::audioButtonWidth() {
	return audioFont->CalcTextSizeA(
		ImGui::GetFontSize(),
		100.0f,
		100.0f,
		OtFontAudio::adr).x + ImGui::GetStyle().ItemInnerSpacing.x * 2.0f;
}


//
//	OtAudioUi::toggleButton
//

bool OtAudioUi::toggleButton(const char* label, float* value) {
	bool flag = *value != 0.0f;
	bool result = OtUi::toggleButton(label, &flag);

	if (result) {
		*value = static_cast<float>(flag);
	}

	return result;
}


//
//	OtAudioUi::smallToggleButton
//

bool OtAudioUi::smallToggleButton(const char* label, float* value) {
	auto changed = false;

	auto pos = ImGui::GetCursorScreenPos();
	float height = ImGui::GetFrameHeight();
	float width = height * 1.4f;

	auto id = fmt::format("##{}Button", label);
	ImGui::InvisibleButton(id.c_str(), ImVec2(width, height));

	if (ImGui::IsItemClicked()) {
		*value = *value == 0.0f ? 1.0f : 0.0f;
		changed = true;
	}

	float spacing = 3.0f;
	float diameter = height - spacing * 2.0f;
	float radius = diameter * 0.5f;

	pos += ImVec2(spacing, spacing);
	height = diameter;
	width = diameter * 1.55f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	ImDrawList* drawlist = ImGui::GetWindowDrawList();

	if (ImGui::IsItemHovered()) {
		drawlist->AddRectFilled(
			pos,
			ImVec2(pos.x + width, pos.y + height),
			ImGui::GetColorU32(*value ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_ScrollbarGrabActive]),
			radius);

	} else {
		drawlist->AddRectFilled(
			pos,
			ImVec2(pos.x + width, pos.y + height),
			ImGui::GetColorU32(*value ? colors[ImGuiCol_Button] : colors[ImGuiCol_ScrollbarGrab]),
			radius);
	}

	drawlist->AddCircleFilled(
		ImVec2(pos.x + radius + (*value ? 1.0f : 0.0f) * (width - radius * 2.0f), pos.y + radius),
		radius - 1.5f,
		IM_COL32(255, 255, 255, 255));

	return changed;
}


//
//	OtAudioUi::smallToggleButtonWidth
//

float OtAudioUi::smallToggleButtonWidth() {
	return ImGui::GetFrameHeight() * 1.4f;;
}


//
//	OtAudioUi::trimSlider
//

bool OtAudioUi::trimSlider(float* value) {
	ImGui::PushID(value);
	ImGui::SetNextItemWidth(ImGui::GetTextLineHeight() * 4.0f);
	auto result = ImGui::SliderFloat("", value, 0.0f, 2.0f, "%.2f");
	ImGui::PopID();
	return result;
}


//
//	OtAudioUi::tuningPopup
//

bool OtAudioUi::tuningPopup(float* tuning) {
	ImGui::PushID(tuning);
	bool changed = false;

	if (OtAudioUi::audioButton(OtFontAudio::tuning)) {
		ImGui::OpenPopup("tuningPopup");
	}

	if (ImGui::BeginPopup("tuningPopup")) {
		auto cents = static_cast<int>(*tuning);

		if (OtUi::knob("Cents", &cents, -4800, 4800)) {
			*tuning = static_cast<float>(cents);
			changed |= true;
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();
	return changed;
}


//
//	OtAudioUi::waveFormSelector
//

bool OtAudioUi::waveFormSelector(float* waveform) {
	auto changed = false;
	auto spacing = ImGui::GetStyle().ItemInnerSpacing.x;
	ImGui::PushID("WaveForms");

	changed |= audioRadioButton(OtFontAudio::filterBypass, waveform, 0.0f, "Off");
	ImGui::SameLine(0.0f, spacing);
	changed |= audioRadioButton(OtFontAudio::modSine, waveform, 1.0f, "Sine");
	ImGui::SameLine(0.0f, spacing);
	changed |= audioRadioButton(OtFontAudio::modTriangle, waveform, 2.0f, "Triangle");
	ImGui::SameLine(0.0f, spacing);
	changed |= audioRadioButton(OtFontAudio::modSharkTooth, waveform, 3.0f, "Shark Tooth");
	ImGui::SameLine(0.0f, spacing);
	changed |= audioRadioButton(OtFontAudio::modSawUp, waveform, 4.0f, "Saw Tooth");
	ImGui::SameLine(0.0f, spacing);
	changed |= audioRadioButton(OtFontAudio::modSquare, waveform, 5.0f, "Square");
	ImGui::SameLine(0.0f, spacing);
	changed |= audioRadioButton(OtFontAudio::modSh, waveform, 6.0f, "Pulse");

	ImGui::PopID();
	return changed;
}


//
//	Support functions
//

template <typename T>
static inline T easing(T t) {
	return t * (static_cast<T>(2) - t);
}

static inline float* fillVisualizationBuffer(float* p, int samples, float start, float end) {
	auto range = end - start;

	for (auto i = 0; i < samples; i++) {
		*p++ = start + range * easing(static_cast<float>(i) / static_cast<float>(samples));
	}

	return p;
}

static inline int durationInSamples(float seconds, float sampleRate) {
	return std::max(1, static_cast<int>(std::floor(seconds * sampleRate)));
}


//
//	OtAudioUi::adsrEnvelope
//

void OtAudioUi::adsrEnvelope(const char* label, AdsrState& state, ImVec2 size) {
	if (state.update) {
		// get total envelope length and length of each phase in samples
		auto totalLengthInTime = (state.attack + state.decay + state.release) * 1.25f;
		auto sampleRate = static_cast<float>(adsrDataSize) / totalLengthInTime;
		auto attackSamples = durationInSamples(state.attack, sampleRate);
		auto decaySamples = durationInSamples(state.decay, sampleRate);
		auto sustainSamples = durationInSamples(totalLengthInTime * 0.2f, sampleRate);
		auto releaseSamples = durationInSamples(state.release, sampleRate);
		auto roundingSamples = static_cast<int>(adsrDataSize) - attackSamples - decaySamples - sustainSamples - releaseSamples;

		// populate output buffer
		auto p = state.graph.data();
		p = fillVisualizationBuffer(p, attackSamples, 0.0f, 1.0f);
		p = fillVisualizationBuffer(p, decaySamples, 1.0f, state.sustain);
		p = fillVisualizationBuffer(p, sustainSamples, state.sustain, state.sustain);
		p = fillVisualizationBuffer(p, releaseSamples, state.sustain, 0.0f);
		p = fillVisualizationBuffer(p, roundingSamples, 0.0f, 0.0f);
		state.update = false;
	}

	// render envelope
	if (ImPlot::BeginPlot(label, size, ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs)) {
		ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels);
		ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);
		ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, adsrDataSize);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -0.02, 1.02);
		ImPlot::PlotLine("Signal", state.graph.data(), adsrDataSize);
		ImPlot::EndPlot();
	}
}
