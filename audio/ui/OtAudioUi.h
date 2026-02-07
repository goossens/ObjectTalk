//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

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
	static inline float trimSliderHeight() { return ImGui::GetTextLineHeight(); }
};
