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

#include "OtNumbers.h"

#include "OtCircuitFactory.h"


//
//	OtMonoToStereo
//

class OtMonoToStereo : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::mono);
		output = addOutputPin("Output", OtCircuitPinClass::Type::stereo);
	}

	// process frame
	void execute() override {
		if (input->isSourceConnected()) {
			auto signal = input->getSignalBuffer();

			for (size_t i = 0; i < signal->getSampleCount(); i++) {
				auto value = signal->get(0, i);
				output->buffer->set(0, i, value);
				output->buffer->set(1, i, value);
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
};

static OtCircuitFactoryRegister<OtMonoToStereo> registration;
