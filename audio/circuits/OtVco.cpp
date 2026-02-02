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
#include "OtVco.h"
#include "OtVcoUi.h"


//
//	OtVcoCircuit
//

class OtVcoCircuit : public OtFaustCircuitUI<OtVco, OtVcoUi> {
public:
	// configure pins
	inline void configurePins() override {
		frequencyInput = addInputPin("Freq", OtCircuitPinClass::Type::control)->hasTuning(true);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono)->hasAttenuation(true);
	}

	// configure UI
	inline void configureUI() override {
		ui.showFrequency(!frequencyInput->isSourceConnected());
		needsSizing = true;
	}

	// generate samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
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
			audioOutput->setSamples(output);
		}
	};

	static constexpr const char* circuitName = "VCO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// work variables
	OtCircuitPin frequencyInput;
	OtCircuitPin audioOutput;
};

static OtCircuitFactoryRegister<OtVcoCircuit> registration;
