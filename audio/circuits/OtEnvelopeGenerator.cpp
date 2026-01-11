//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "imgui.h"
#include "implot.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtAudioUi.h"
#include "OtEnvelope.h"
#include "OtCircuitFactory.h"


//
//	OtEnvelopeGenerator
//

class OtEnvelopeGenerator : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		triggerInput = addInputPin("Trigger", OtCircuitPinClass::Type::control);
		envelopeOutput = addOutputPin("Output", OtCircuitPinClass::Type::control);
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		bool changed = OtAudioUi::envelope(&attack, &hold, &decay, &sustain, &release, &plotData, updateVisualization);
		updateVisualization = changed;
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return OtAudioUi::getEnvelopeWidth();
	}

	inline float getCustomRenderingHeight() override {
		return OtAudioUi::getEnvelopeHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["attack"] = attack;
		(*data)["hold"] = hold;
		(*data)["decay"] = decay;
		(*data)["sustain"] = sustain;
		(*data)["release"] = release;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		attack = data->value("attack", 1.0f);
		hold = data->value("hold", 0.0f);
		decay = data->value("decay", 1.0f);
		sustain = data->value("sustain", 0.5f);
		release = data->value("release", 0.2f);
		updateVisualization = true;
	}

	// generate samples
	void execute() override {
		if (envelopeOutput->isDestinationConnected()) {
			if (triggerInput->isSourceConnected()) {
				envelope.setAttackTime(attack);
				envelope.setHoldTime(hold);
				envelope.setDecayTime(decay);
				envelope.setSustainLevel(sustain);
				envelope.setReleaseTime(release);

				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					auto newTriggerState = triggerInput->getSample(i) > 0.5f;

					if (newTriggerState != triggerState) {
						if (newTriggerState) {
							envelope.noteOn();

						} else {
							envelope.noteOff();
						}

						triggerState = newTriggerState;
					}

					envelopeOutput->setSample(i, envelope.process());
				}

			} else {
				envelopeOutput->buffer->clear(0.0f);
			}
		}
	}

	static constexpr const char* circuitName = "Envelope Generator";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	float attack = 1.0f;
	float hold = 0.0f;
	float decay = 1.0f;
	float sustain = 0.8f;
	float release = 0.2f;

	// work variables
	OtCircuitPin triggerInput;
	OtCircuitPin envelopeOutput;

	bool triggerState = false;
	OtEnvelope envelope;
	std::vector<float> plotData;
	bool updateVisualization = true;
};

static OtCircuitFactoryRegister<OtEnvelopeGenerator> registration;
