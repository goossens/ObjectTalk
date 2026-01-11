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

#include "OtAudioSettings.h"
#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"
#include "OtVirtualAnalogFilter.h"


//
//	OtVcf
//

class OtVcf : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioInput = addInputPin("Input", OtCircuitPinClass::Type::mono);
		fmInput = addInputPin("F Mod", OtCircuitPinClass::Type::control)->hasAttenuation();
		qmInput = addInputPin("Q Mod", OtCircuitPinClass::Type::control)->hasAttenuation();
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);

		frequencyControl = addControl("Freq", &frequency)->setRange(80.0f, 8000.0f)->setLabelFormat("%.0fhz")->setIsFrequency()->setIsLogarithmic();
		resonanceControl = addControl("Q", &resonance)->setRange(0.0f, 1.0f)->setLabelFormat("%.2f");
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
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

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["mode"] = mode;
		(*data)["frequency"] = frequency;
		(*data)["resonance"] = resonance;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		mode = data->value("mode", OtVirtualAnalogFilter::Mode::off);
		frequency = data->value("frequency", 1000.0f);
		resonance = data->value("resonance", 0.5f);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					filter.set(
						mode,
						(fmInput->isSourceConnected()) ? OtAudioUtilities::tune(frequency, fmInput->getSample(i)) : frequency,
						(qmInput->isSourceConnected()) ? resonance * qmInput->getSample(i) : resonance);

					audioOutput->setSample(i, filter.process(audioInput->getSample(i)));
				}

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

	static constexpr const char* circuitName = "VCF";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;

private:
	// properties
	OtVirtualAnalogFilter::Mode mode = OtVirtualAnalogFilter::Mode::off;
	float frequency = 1000.0f;
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
