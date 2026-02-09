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

#include "OtUi.h"


//
//	OtAudioUi
//

class OtAudioUi : public OtUi {
public:
	// render headers with decorations
	static bool headerWithToggleButton(const char* label, bool* value);

	// support buttons with audio-related icons
	static float getAudioButtonWidth();

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

	// toggle button based on float value
	static bool toggleButton(const char* label, float* value);

	// trim sliders
	static bool trimSlider(float* value);

	static inline float trimSliderWidth() { return ImGui::GetTextLineHeight() * 4.0f; }
	static inline float trimSliderHeight() { return ImGui::GetFrameHeightWithSpacing(); }

	// waveform selector
	static bool waveFormSelector(float* waveform);
	static inline float waveFormSelectorWidth() { return OtAudioUi::getAudioButtonWidth() * 7.0f + ImGui::GetStyle().ItemInnerSpacing.x * 6.0f; }
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
