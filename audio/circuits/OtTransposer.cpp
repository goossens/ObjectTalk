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
//	OtTransposer
//

class OtTransposer : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::control);
		output1 = addOutputPin("Output 1", OtCircuitPinClass::Type::control)->hasTuning();
		output2 = addOutputPin("Output 2", OtCircuitPinClass::Type::control)->hasTuning();
		output3 = addOutputPin("Output 3", OtCircuitPinClass::Type::control)->hasTuning();
		output4 = addOutputPin("Output 4", OtCircuitPinClass::Type::control)->hasTuning();
	}

	// process samples
	void execute() override {
		if (input->isSourceConnected()) {
			for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
				float sample = input->getSample(i);
				if (output1->isDestinationConnected()) { output1->setSample(i, sample); }
				if (output2->isDestinationConnected()) { output2->setSample(i, sample); }
				if (output3->isDestinationConnected()) { output3->setSample(i, sample); }
				if (output4->isDestinationConnected()) { output4->setSample(i, sample); }
			}

		} else {
			if (output1->isDestinationConnected()) { output1->setSamples(0.0f); }
			if (output2->isDestinationConnected()) { output2->setSamples(0.0f); }
			if (output3->isDestinationConnected()) { output3->setSamples(0.0f); }
			if (output4->isDestinationConnected()) { output4->setSamples(0.0f); }
		}
	};

	static constexpr const char* circuitName = "Transposer";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::converter;

private:
	// properties
	OtCircuitPin input;
	OtCircuitPin output1;
	OtCircuitPin output2;
	OtCircuitPin output3;
	OtCircuitPin output4;
};

static OtCircuitFactoryRegister<OtTransposer> registration;
