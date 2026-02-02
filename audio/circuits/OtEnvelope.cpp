//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtAdsr.h"
#include "OtAdsrUi.h"
#include "OtCircuitFactory.h"


//
//	OtEnvelopeCircuit
//

class OtEnvelopeCircuit : public OtFaustCircuitUI<OtAdsr, OtAdsrUi> {
public:
	// configure pins
	inline void configurePins() override {
		triggerInput = addInputPin("Trigger", OtCircuitPinClass::Type::control);
		envelopeOutput = addOutputPin("Output", OtCircuitPinClass::Type::control);
	}

	// generate samples
	void execute() override {
		if (envelopeOutput->isDestinationConnected()) {
			if (triggerInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize];
				float output[OtAudioSettings::bufferSize];

				auto in = input;
				auto out = output;

				triggerInput->getSamples(input);

				dsp.compute(OtAudioSettings::bufferSize, &in, &out);
				envelopeOutput->setSamples(output);

			} else {
				envelopeOutput->audioBuffer->clear(0.0f);
			}
		}
	}

	static constexpr const char* circuitName = "Envelope";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	OtCircuitPin triggerInput;
	OtCircuitPin envelopeOutput;
};

static OtCircuitFactoryRegister<OtEnvelopeCircuit> registration;
