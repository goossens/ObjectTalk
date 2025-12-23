//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtCircuitFactory.h"
#include "OtVirtualAnalogFilter.h"


//
//	OtVcf
//

class OtVcf : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		audioInput = addInputPin("Input", OtCircuitPinClass::Type::mono);
		fmInput = addInputPin("F Mod", OtCircuitPinClass::Type::control, true);
		qmInput = addInputPin("Q Mod", OtCircuitPinClass::Type::control, true);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);

		frequencyControl = addControl("Freq", nullptr, &pitch)
			->setRange(80.0f, 8000.0f)
			->setLabelFormat("%.0f")
			->setIsFrequency()
			->setIsLogarithmic();

		resonanceControl = addControl("Q", nullptr, &resonance)
			->setRange(0.0f, 1.0f)
			->setLabelFormat("%.2f");
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		bool changed = OtUi::selectorEnum("##modes", &mode, OtVirtualAnalogFilter::modes, OtVirtualAnalogFilter::modesCount);
		changed |= frequencyControl->renderKnob(); ImGui::SameLine();
		changed |= resonanceControl->renderKnob();
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::knobWidth(2);
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + OtUi::knobHeight();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["mode"] = mode;
		(*data)["pitch"] = pitch;
		(*data)["resonance"] = resonance;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		mode = data->value("mode", OtVirtualAnalogFilter::Mode::off);
		pitch = data->value("pitch", 1000.0f);
		resonance = data->value("resonance", 0.5f);
	}

	// generate samples
	void execute() override {
		if (audioInput->isSourceConnected()) {
			auto signal = audioInput->getSignalBuffer();

			for (size_t i = 0; i < signal->getSampleCount(); i++) {
				filter.set(mode, pitch, resonance);
				audioOutput->buffer->set(0, i, filter.process(signal->get(0, i)));
			}

		} else {
			audioOutput->buffer->clear();
		}
	};

	static constexpr const char* circuitName = "VCF";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;

private:
	// properties
	OtVirtualAnalogFilter::Mode mode = OtVirtualAnalogFilter::Mode::off;
	float pitch = 1000.0f;
	float resonance = 0.5f;

	// work variables
	OtCircuitPin audioInput;
	OtCircuitPin fmInput;
	OtCircuitPin qmInput;
	OtCircuitPin audioOutput;

	OtCircuitControl frequencyControl;
	OtCircuitControl resonanceControl;

	OtVirtualAnalogFilter filter;
};

static OtCircuitFactoryRegister<OtVcf> registration;
