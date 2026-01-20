//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

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
		bool changed = parameters.renderUI();
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return parameters.getRenderWidth();
	}

	inline float getCustomRenderingHeight() override {
		return parameters.getRenderHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		parameters.serialize(data, basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		parameters.deserialize(data, basedir);
	}

	// generate samples
	void execute() override {
		if (envelopeOutput->isDestinationConnected()) {
			if (triggerInput->isSourceConnected()) {
				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					auto newTriggerState = triggerInput->getSample(i) > 0.5f;

					if (newTriggerState != triggerState) {
						if (newTriggerState) {
							state.noteOn(parameters);

						} else {
							state.noteOff(parameters);
						}

						triggerState = newTriggerState;
					}

					envelopeOutput->setSample(i, state.process(parameters));
				}

			} else {
				envelopeOutput->audioBuffer->clear(0.0f);
			}
		}
	}

	static constexpr const char* circuitName = "Envelope Generator";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtEnvelope::Parameters parameters;

	// work variables
	OtEnvelope::State state;

	OtCircuitPin triggerInput;
	OtCircuitPin envelopeOutput;

	bool triggerState = false;
};

static OtCircuitFactoryRegister<OtEnvelopeGenerator> registration;
