//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"


//
//	OtVca
//

class OtVca : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioInput = addInputPin("Input", OtCircuitPinClass::Type::mono);
		cvInput = addInputPin("CV", OtCircuitPinClass::Type::control)->hasAttenuation();
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono)->hasAttenuation();
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					if (cvInput->isSourceConnected()) {
						audioOutput->setSample(i, audioInput->getSample(i) * cvInput->getSample(i));

					} else {
						audioOutput->setSample(i, audioInput->getSample(i));
					}
				}

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

	static constexpr const char* circuitName = "VCA";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;

private:
	// work variables
	OtCircuitPin audioInput;
	OtCircuitPin cvInput;
	OtCircuitPin audioOutput;
};

static OtCircuitFactoryRegister<OtVca> registration;
