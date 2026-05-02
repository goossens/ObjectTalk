//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>
#include <mutex>
#include <vector>

#include "OtFontAudio.h"
#include "OtUi.h"

#include "OtAudioSettings.h"

//
//	OtAudioUi
//

class OtAudioUi : public OtUi {
public:
	// render header with decorations
	static bool decoratedHeader(const char* label, float* attenuation, float* tuning, float* power);

	// support buttons with audio-related icons
	static float audioButtonWidth();

	static inline bool audioButton(OtFontAudio::Type label, const char* tooltip=nullptr) {
		ImGui::PushFont(getAudioFont(), 0.0f);
		auto result = ImGui::Button(reinterpret_cast<const char*>(label), ImVec2(audioButtonWidth(), 0.0f));
		ImGui::PopFont();

		if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
			ImGui::SetTooltip("%s", tooltip);
		}

		return result;
	}

	template <typename T>
	static inline bool audioRadioButton(OtFontAudio::Type label, T* value, T buttonValue, const char* tooltip=nullptr) {
		ImGui::PushFont(getAudioFont(), 0.0f);
		auto changed = radioButton(reinterpret_cast<const char*>(label), value, buttonValue);
		ImGui::PopFont();

		if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
			ImGui::SetTooltip("%s", tooltip);
		}

		return changed;
	}

	template <typename T>
	inline static bool audioLatchButton(OtFontAudio::Type label, T* value, const char* tooltip=nullptr) {
		ImGui::PushFont(getAudioFont(), 0.0f);
		auto changed = latchButton(reinterpret_cast<const char*>(label), value);
		ImGui::PopFont();

		if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
			ImGui::SetTooltip("%s", tooltip);
		}

		return changed;
	}

	// toggle buttons based on float value
	static bool toggleButton(const char* label, float* value);
	static bool smallToggleButton(const char* label, float* value);
	static float smallToggleButtonWidth();

	// knobs
	static bool knob(const char* label, float* value, float minValue, float maxValue, const char* format="%.0f", bool logarithmic=false);
	static bool knob(const char* label, int* value, int minValue, int maxValue);

	static float knobWidth(size_t columns=1);
	static float knobHeight(size_t rows=1);

	// vertical slider
	static bool verticalSlider(const char* label, ImVec2 size, float* value, float minValue, float maxValue, const char* format="%f");

	// trim sliders
	static bool trimSlider(float* value);

	static inline float trimSliderWidth() { return ImGui::GetTextLineHeight() * 4.0f; }
	static inline float trimSliderHeight() { return ImGui::GetFrameHeightWithSpacing(); }

	// tuning button with popup
	static bool tuningPopup(float* tuning);

	// dBFS LED display
	struct dbfsState {
		float decibels = -60.0f;

		static constexpr float maxHistoryInSeconds = 1.5f;
		static constexpr size_t maxHistory = static_cast<size_t>(maxHistoryInSeconds * OtAudioSettings::sampleRate / OtAudioSettings::bufferSize);

		std::vector<float> history;
		size_t historyIndex = 0;
		std::mutex mutex;
	};

	static void dbfsReset(dbfsState& state);
	static void dbfsUpdate(dbfsState& state, float* samples, size_t size);
	static void dbfsRenderH(dbfsState& state);
	static void dbfsRenderV(dbfsState& state);
	static float dbfsSizeH();
	static float dbfsSizeV();

	// waveform selector
	static bool waveFormSelector(float* waveform);
	static inline float waveFormSelectorWidth() { return OtAudioUi::audioButtonWidth() * 7.0f + ImGui::GetStyle().ItemInnerSpacing.x * 6.0f; }
	static inline float waveFormSelectorHeight() { return ImGui::GetFrameHeightWithSpacing(); }

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

	static void adsrEnvelope(const char* label, AdsrState& state, ImVec2 size);
};
