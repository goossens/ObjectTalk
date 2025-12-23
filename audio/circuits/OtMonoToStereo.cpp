//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>
#include <cstdint>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtNumbers.h"

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
	// configure node
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::mono);
		output = addOutputPin("Output", OtCircuitPinClass::Type::stereo);

		panControl = addControl("Pan", nullptr, &panLevel)->setRange(-1.0f, 1.0f)->setLabelFormat("%.2f");
		leftDelayControl = addControl("Dly L", nullptr, &leftDelayTimeInMs)->setRange(0.0f, 100.0f)->setLabelFormat("%.1f");
		rightDelayControl = addControl("Dly R", nullptr, &rightDelayTimeInMs)->setRange(0.0f, 100.0f)->setLabelFormat("%.1f");
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

	// process frame
	void execute() override {
		if (input->isSourceConnected()) {
			auto signal = input->getSignalBuffer();

			auto angle = (panLevel * std::numbers::pi / 4.0f) + std::numbers::pi / 4.0f;
			auto leftGain = std::cos(angle);
			auto rightGain = std::sin(angle);

			auto leftDelayTimeInSamples = leftDelayTimeInMs * OtAudioSettings::sampleRate / 1000.0f;
			auto rightDelayTimeInSamples = rightDelayTimeInMs * OtAudioSettings::sampleRate / 1000.0f;

			for (size_t i = 0; i < signal->getSampleCount(); i++) {
				auto value = signal->get(0, i);

				output->buffer->set(0, i, leftDelay.process(value, leftDelayTimeInSamples) * leftGain);
				output->buffer->set(1, i, rightDelay.process(value, rightDelayTimeInSamples) * rightGain);
			}

		} else {
			output->buffer->clear();
		}
	};

	static constexpr const char* circuitName = "Mono to Stereo";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;

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

	OtDelay leftDelay{OtAudioSettings::sampleRate / 10}; // max delay is 100ms
	OtDelay rightDelay{OtAudioSettings::sampleRate / 10};
};

static OtCircuitFactoryRegister<OtMonoToStereo> registration;
