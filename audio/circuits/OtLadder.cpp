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
#include "OtLadder.h"


//
//	OtLadderCircuit
//

class OtLadderCircuit : public OtFaustCircuit<OtLadder> {
public:
	// configure pins
	inline void configure() override {
		OtAssert(this->dsp.getNumInputs() == 4);
		OtAssert(this->dsp.getNumOutputs() == 1);

		signalInput = addInputPin("Input", OtCircuitPinClass::Type::mono);
		freqInput = addInputPin("Freq", OtCircuitPinClass::Type::control)->hasAttenuation();
		mod1Input = addInputPin("Mod 1", OtCircuitPinClass::Type::control)->hasAttenuation();
		mod2Input = addInputPin("Mod 2", OtCircuitPinClass::Type::control)->hasAttenuation();
		ladderOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// generate samples
	void execute() override {
		float output[OtAudioSettings::bufferSize] = {0.0f};

		if (ladderOutput->isDestinationConnected() && signalInput->isSourceConnected()) {
			float signal[OtAudioSettings::bufferSize];
			float freq[OtAudioSettings::bufferSize] = {0.0f};
			float mod1[OtAudioSettings::bufferSize] = {0.0f};
			float mod2[OtAudioSettings::bufferSize] = {0.0f};

			float* in[] = {signal, freq, mod1, mod2};
			auto out = output;

			signalInput->getSamples(signal);

			if (freqInput->isSourceConnected()) {
				freqInput->getSamples(freq);
			}

			if (mod1Input->isSourceConnected()) {
				mod1Input->getSamples(mod1);
			}

			if (mod2Input->isSourceConnected()) {
				mod2Input->getSamples(mod2);
			}

			dsp.compute(OtAudioSettings::bufferSize, in, &out);
		}

		ladderOutput->setSamples(output);
	}

	static constexpr const char* circuitName = "Ladder";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::processor;

private:
	OtCircuitPin signalInput;
	OtCircuitPin freqInput;
	OtCircuitPin mod1Input;
	OtCircuitPin mod2Input;
	OtCircuitPin ladderOutput;
};

static OtCircuitFactoryRegister<OtLadderCircuit> registration;
