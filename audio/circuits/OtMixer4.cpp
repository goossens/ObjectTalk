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
//	OtMixer4
//

class OtMixer4 : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		input1 = addInputPin("Input 1", OtCircuitPinClass::Type::mono)->hasAttenuation();
		input2 = addInputPin("Input 2", OtCircuitPinClass::Type::mono)->hasAttenuation();
		input3 = addInputPin("Input 3", OtCircuitPinClass::Type::mono)->hasAttenuation();
		input4 = addInputPin("Input 4", OtCircuitPinClass::Type::mono)->hasAttenuation();
		output = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// process samples
	void execute() override {
		if (output->isDestinationConnected()) {
			for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
				float result = 0.0f;

				if (input1->isSourceConnected()) { result += input1->getSample(i); }
				if (input2->isSourceConnected()) { result += input2->getSample(i); }
				if (input3->isSourceConnected()) { result += input3->getSample(i); }
				if (input4->isSourceConnected()) { result += input4->getSample(i); }

				output->setSample(i, result);
			}
		}
	};

	static constexpr const char* circuitName = "4 Channel Mixer";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::processor;

private:
	// properties
	OtCircuitPin input1;
	OtCircuitPin input2;
	OtCircuitPin input3;
	OtCircuitPin input4;
	OtCircuitPin output;
};

static OtCircuitFactoryRegister<OtMixer4> registration;
