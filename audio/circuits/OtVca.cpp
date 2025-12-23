//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtCircuitFactory.h"


//
//	OtVca
//

class OtVca : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		audioInput = addInputPin("Input", OtCircuitPinClass::Type::mono);
		cvInput = addInputPin("CV", OtCircuitPinClass::Type::control);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);

		volumeControl = addControl("Volume", nullptr, &volume)->setRange(0.0f, 1.0f)->setLabelFormat("%.2f");
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		return volumeControl->renderKnob();
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::knobWidth();
	}

	inline float getCustomRenderingHeight() override {
		return OtUi::knobHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["volume"] = volume;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		volume = data->value("volume", 1.0f);
	}

	// generate samples
	void execute() override {
		if (audioInput->isSourceConnected()) {
			auto signal = audioInput->getSignalBuffer();

			for (size_t i = 0; i < signal->getSampleCount(); i++) {
				if (cvInput->isSourceConnected()) {
					audioOutput->buffer->set(0, i, signal->get(0, i) * cvInput->getSignalBuffer()->get(0, i) * volume);

				} else {
					audioOutput->buffer->set(0, i, signal->get(0, i) * volume);
				}
			}

		} else {
			audioOutput->buffer->clear();
		}
	};

	static constexpr const char* circuitName = "VCA";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;

private:
	// properties
	float volume = 1.0f;

	// work variables
	OtCircuitPin audioInput;
	OtCircuitPin cvInput;
	OtCircuitPin audioOutput;

	OtCircuitControl volumeControl;
};

static OtCircuitFactoryRegister<OtVca> registration;
