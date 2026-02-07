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
#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"
#include "OtLfo.h"
#include "OtLfoUi.h"


//
//	OtLfoCircuit
//

class OtLfoCircuit : public OtFaustCircuitUI<OtLfo, OtLfoUi> {
public:
	// configure pins
	inline void configurePins() override {
		OtAssert(this->dsp.getNumInputs() == 1);
		OtAssert(this->dsp.getNumOutputs() == 1);

		frequencyInput = addInputPin("Freq", OtCircuitPinClass::Type::control)->hasTuning(true);
		signalOutput = addOutputPin("Output", OtCircuitPinClass::Type::control);
	}

	// configure UI
	inline void configureUI() override {
		ui.showFrequency(!frequencyInput->isSourceConnected());
		needsSizing = true;
	}

	// generate samples
	void execute() override {
		if (signalOutput->isDestinationConnected()) {
			float input[OtAudioSettings::bufferSize];
			float output[OtAudioSettings::bufferSize];

			if (frequencyInput->isSourceConnected()) {
				frequencyInput->getSamples(input);

			} else {
				auto freq = OtAudioUtilities::freqToCv(ui.getFrequency());

				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					input[i] = freq;
				}
			}

			auto in = input;
			auto out = output;

			dsp.compute(OtAudioSettings::bufferSize, &in, &out);
			signalOutput->setSamples(output);
		}
	};

	static constexpr const char* circuitName = "LFO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// work variables
	OtCircuitPin frequencyInput;
	OtCircuitPin signalOutput;
};

static OtCircuitFactoryRegister<OtLfoCircuit> registration;
