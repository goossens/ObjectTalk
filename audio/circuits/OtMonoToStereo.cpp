//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtAudioSettings.h"
#include "OtCircuitControl.h"
#include "OtCircuitFactory.h"
#include "OtDelay.h"


//
//	OtMonoToStereo
//

class OtMonoToStereo : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::mono);
		output = addOutputPin("Output", OtCircuitPinClass::Type::stereo);

		panControl = addControl("Pan", &panLevel)->setRange(-1.0f, 1.0f)->setLabelFormat("%.2f");
		leftDelayControl = addControl("Dly L", &leftDelayTimeInMs)->setRange(0.0f, 100.0f)->setLabelFormat("%.1fms");
		rightDelayControl = addControl("Dly R", &rightDelayTimeInMs)->setRange(0.0f, 100.0f)->setLabelFormat("%.1fms");
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		bool changed = panControl->renderKnob(); ImGui::SameLine();
		changed |= leftDelayControl->renderKnob(); ImGui::SameLine();
		changed |= rightDelayControl->renderKnob();
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::knobWidth(3);
	}

	inline float getCustomRenderingHeight() override {
		return OtUi::knobHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["panLevel"] = panLevel;
		(*data)["leftDelayTime"] = leftDelayTimeInMs;
		(*data)["rightDelayTime"] = rightDelayTimeInMs;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		panLevel = data->value("panLevel", 0.0f);
		leftDelayTimeInMs = data->value("leftDelayTime", 0.0f);
		rightDelayTimeInMs = data->value("RightDelayTime", 0.0f);
	}

	// process samples
	void execute() override {
		if (output->isDestinationConnected()) {
			if (input->isSourceConnected()) {
				auto angle = (panLevel * OtAudioSettings::pi / 4.0f) + OtAudioSettings::pi / 4.0f;
				auto leftGain = std::cos(angle);
				auto rightGain = std::sin(angle);

				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					auto value = input->getSample(i);

					output->setSample(0, i, leftDelay.process(value, leftDelayTimeInMs) * leftGain);
					output->setSample(1, i, rightDelay.process(value, rightDelayTimeInMs) * rightGain);
				}

			} else {
				output->setSamples(0.0f);
			}
		}
	};

	static constexpr const char* circuitName = "Mono to Stereo";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::converter;

private:
	// properties
	OtCircuitPin input;
	OtCircuitPin output;

	float panLevel = 0.0f;
	float leftDelayTimeInMs = 0.0f;
	float rightDelayTimeInMs = 0.0f;

	// work variables
	OtCircuitControl panControl;
	OtCircuitControl leftDelayControl;
	OtCircuitControl rightDelayControl;

	OtDelay<float, OtAudioSettings::sampleRate / 10> leftDelay; // max delay is 100ms
	OtDelay<float, OtAudioSettings::sampleRate / 10> rightDelay;
};

static OtCircuitFactoryRegister<OtMonoToStereo> registration;
