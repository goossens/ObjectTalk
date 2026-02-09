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
#include "OtFaust.h"
#include "OtVcf.h"


//
//	OtVcfCircuit
//

class OtVcfCircuit : public OtFaustCircuit<OtVcf> {
public:
	// configure pins
	inline void configure() override {
		OtAssert(this->dsp.getNumInputs() == 3);
		OtAssert(this->dsp.getNumOutputs() == 1);

		signalInput = addInputPin("Input", OtCircuitPinClass::Type::mono);
		mod1Input = addInputPin("Mod 1", OtCircuitPinClass::Type::control)->hasAttenuation();
		mod2Input = addInputPin("Mod 2", OtCircuitPinClass::Type::control)->hasAttenuation();
		signalOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// generate samples
	void execute() override {
		float output[OtAudioSettings::bufferSize] = {0.0f};

		if (signalOutput->isDestinationConnected() && signalInput->isSourceConnected()) {
			float signal[OtAudioSettings::bufferSize];
			float mod1[OtAudioSettings::bufferSize] = {0.0f};
			float mod2[OtAudioSettings::bufferSize] = {0.0f};

			float* in[] = {signal, mod1, mod2};
			auto out = output;

			signalInput->getSamples(signal);

			if (mod1Input->isSourceConnected()) {
				mod1Input->getSamples(mod1);
			}

			if (mod2Input->isSourceConnected()) {
				mod2Input->getSamples(mod2);
			}

			dsp.compute(OtAudioSettings::bufferSize, in, &out);
		}

		signalOutput->setSamples(output);
	}

	static constexpr const char* circuitName = "VCF";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::processor;

private:
	OtCircuitPin signalInput;
	OtCircuitPin mod1Input;
	OtCircuitPin mod2Input;
	OtCircuitPin signalOutput;
};

static OtCircuitFactoryRegister<OtVcfCircuit> registration;
