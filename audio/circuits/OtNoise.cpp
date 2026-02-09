//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtNoise.h"


//
//	OtNoiseCircuit
//

class OtNoiseCircuit : public OtFaustCircuit<OtNoise> {
public:
	// configure pins
	inline void configure() override {
		OtAssert(this->dsp.getNumInputs() == 0);
		OtAssert(this->dsp.getNumOutputs() == 1);

		signalOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// generate samples
	void execute() override {
		if (signalOutput->isDestinationConnected()) {
			float output[OtAudioSettings::bufferSize];
			auto out = output;
			dsp.compute(OtAudioSettings::bufferSize, nullptr, &out);
			signalOutput->setSamples(output);
		}
	};

	static constexpr const char* circuitName = "Noise";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// work variables
	OtCircuitPin signalOutput;
};

static OtCircuitFactoryRegister<OtNoiseCircuit> registration;
