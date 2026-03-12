//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCircuitFactory.h"
#include "OtFaust.h"
#include "OtGraphicEQ.h"


//
//	OtGraphicEqCircuit
//

class OtGraphicEqCircuit : public OtFaustEffectMono<OtGraphicEQ> {
public:
	// render custom fields
	bool customRendering([[maybe_unused]] float itemWidth) override {
		auto parameters = dsp.getParameters();
		ImVec2 size{sliderWidth, sliderHeight};
		bool changed = false;

		changed |= OtAudioUi::verticalSlider("31", size, &parameters.band1, -70.0f, 10.0f, "%.1fdB"); ImGui::SameLine(0.0f, sliderSpacing);
		changed |= OtAudioUi::verticalSlider("62", size, &parameters.band2, -70.0f, 10.0f, "%.1fdB"); ImGui::SameLine(0.0f, sliderSpacing);
		changed |= OtAudioUi::verticalSlider("125", size, &parameters.band3, -70.0f, 10.0f, "%.1fdB"); ImGui::SameLine(0.0f, sliderSpacing);
		changed |= OtAudioUi::verticalSlider("250", size, &parameters.band4, -70.0f, 10.0f, "%.1fdB"); ImGui::SameLine(0.0f, sliderSpacing);
		changed |= OtAudioUi::verticalSlider("500", size, &parameters.band5, -70.0f, 10.0f, "%.1fdB"); ImGui::SameLine(0.0f, sliderSpacing);
		changed |= OtAudioUi::verticalSlider("1k", size, &parameters.band6, -70.0f, 10.0f, "%.1fdB"); ImGui::SameLine(0.0f, sliderSpacing);
		changed |= OtAudioUi::verticalSlider("2k", size, &parameters.band7, -70.0f, 10.0f, "%.1fdB"); ImGui::SameLine(0.0f, sliderSpacing);
		changed |= OtAudioUi::verticalSlider("4k", size, &parameters.band8, -70.0f, 10.0f, "%.1fdB"); ImGui::SameLine(0.0f, sliderSpacing);
		changed |= OtAudioUi::verticalSlider("8k", size, &parameters.band9, -70.0f, 10.0f, "%.1fdB"); ImGui::SameLine(0.0f, sliderSpacing);
		changed |= OtAudioUi::verticalSlider("16k", size, &parameters.band10, -70.0f, 10.0f, "%.1fdB");

		if (changed) {
			dsp.setParameters(parameters);
		}
		return changed;
	}

	float getCustomRenderingWidth() override {
		return sliderWidth * 10.0f + sliderSpacing * 9.0f;
	}
	float getCustomRenderingHeight() override {
		return sliderHeight + ImGui::GetFrameHeightWithSpacing();
	}

	static constexpr const char* circuitName = "Graphic EQ";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::equalizer;
	static inline constexpr float sliderWidth = 20.0f;
	static inline constexpr float sliderHeight = 100.0f;
	static inline constexpr float sliderSpacing = 5.0f;
};

static OtCircuitFactoryRegister<OtGraphicEqCircuit> registration;
