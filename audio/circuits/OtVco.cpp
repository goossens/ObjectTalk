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
#include "OtVcoDsp.h"
#include "OtVcoUi.h"


//
//	OtVco
//

class OtVco : public OtFaustCircuit<OtVcoDsp, OtVcoUi> {
public:
	// configure pins
	inline void configurePins() override {
		frequencyInput = addInputPin("Pitch", OtCircuitPinClass::Type::control)->hasTuning(true);
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
			if (frequencyInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize];
				float output[OtAudioSettings::bufferSize];

				auto in = input;
				auto out = output;

				frequencyInput->getSamples(input);
				dsp.compute(OtAudioSettings::bufferSize, &in, &out);
				audioOutput->setSamples(output);

			} else {
				audioOutput->setSamples(OtAudioUtilities::pitchToCv(ui.getFrequency()));
			}
		}
	};

	static constexpr const char* circuitName = "VCO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// work variables
	OtCircuitPin frequencyInput;
	OtCircuitPin audioOutput;
};

static OtCircuitFactoryRegister<OtVco> registration;
