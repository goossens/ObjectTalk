//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <array>
#include <cmath>

#include "implot.h"

#include "OtFontAudio.h"
#include "OtUi.h"


//
//	OtAudioUi
//

class OtAudioUi : public OtUi {
public:
	inline static float getAudioButtonWidth() {
		return audioFont->CalcTextSizeA(
			15.0f,
			100.0f,
			100.0f,
			OtFontAudio::adr).x + ImGui::GetStyle().ItemInnerSpacing.x * 2.0f;

	}

	template <typename T>
	inline static bool audioRadioButton(const char* label, T* value, T buttonValue, const char* tooltip=nullptr) {
		ImGui::PushFont(getAudioFont(), 0.0f);
		auto changed = radioButton(label, value, buttonValue);
		ImGui::PopFont();

		if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
			ImGui::SetTooltip("%s", tooltip);
		}

		return changed;
	}

	inline static bool toggleButton(const char* label, float* value) {
		bool flag = *value != 0.0f;
		bool result = OtUi::toggleButton(label, &flag);

		if (result) {
			*value = static_cast<float>(flag);
		}

		return result;
	}

	// trim sliders
	static inline bool trimSlider(float* value) {
		ImGui::PushID(value);
		ImGui::SetNextItemWidth(ImGui::GetTextLineHeight() * 4.0f);
		auto result = ImGui::SliderFloat("", value, 0.0f, 2.0f, "%.2f");
		ImGui::PopID();
		return result;
	}

	static inline float trimSliderWidth() { return ImGui::GetTextLineHeight() * 4.0f; }
	static inline float trimSliderHeight() { return ImGui::GetFrameHeightWithSpacing(); }

	// waveform selector
	static inline bool waveFormSelector(float* waveform) {
		auto changed = false;
		auto spacing = ImGui::GetStyle().ItemInnerSpacing.x;
		ImGui::PushID("WaveForms");

		changed |= audioRadioButton(OtFontAudio::filterBypass, waveform, 0.0f, "Off");
		ImGui::SameLine(0.0f, spacing);
		changed |= audioRadioButton(OtFontAudio::modSine, waveform, 1.0f, "Sine");
		ImGui::SameLine(0.0f, spacing);
		changed |= audioRadioButton(OtFontAudio::modTriangle, waveform, 2.0f, "Triangle");
		ImGui::SameLine(0.0f, spacing);
		changed |= audioRadioButton(OtFontAudio::modSawDown, waveform, 3.0f, "Shark Tooth");
		ImGui::SameLine(0.0f, spacing);
		changed |= audioRadioButton(OtFontAudio::modSawUp, waveform, 4.0f, "Saw Tooth");
		ImGui::SameLine(0.0f, spacing);
		changed |= audioRadioButton(OtFontAudio::modSquare, waveform, 5.0f, "Square");
		ImGui::SameLine(0.0f, spacing);
		changed |= audioRadioButton(OtFontAudio::modSh, waveform, 6.0f, "Pulse");

		ImGui::PopID();
		return changed;
	}

	static inline float waveFormSelectorWidth() { return OtAudioUi::getAudioButtonWidth() * 7.0f + ImGui::GetStyle().ItemInnerSpacing.x * 6.0f; }
	static inline float waveFormSelectorHeight() { return ImGui::GetFrameHeightWithSpacing(); }

	// support functions
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

	// render an ADSR envelope graph
	static constexpr size_t adsrDataSize = 1024;

	struct AdsrState {
		float attack = 0.01f;
		float decay = 0.05f;
		float sustain = 0.8f;
		float release = 0.03f;
		std::array<float, adsrDataSize> graph;
		bool update = true;
	};

	static inline void adsrEnvelope(const char* label, AdsrState& state, ImVec2 size) {
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
};
