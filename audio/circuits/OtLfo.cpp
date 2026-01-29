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
#include "OtLfoDsp.h"
#include "OtLfoUi.h"


//
//	OtLfo
//

class OtLfo : public OtFaustCircuit<OtLfoDsp, OtLfoUi> {
public:
	// configure pins
	inline void configurePins() override {
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
			if (frequencyInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize];
				float output[OtAudioSettings::bufferSize];

				auto in = input;
				auto out = output;

				frequencyInput->getSamples(input);
				dsp.compute(OtAudioSettings::bufferSize, &in, &out);
				signalOutput->setSamples(output);

			} else {
				signalOutput->setSamples(OtAudioUtilities::pitchToCv(ui.getFrequency()));
			}
		}
	};

	static constexpr const char* circuitName = "LFO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// work variables
	OtCircuitPin frequencyInput;
	OtCircuitPin signalOutput;
};

static OtCircuitFactoryRegister<OtLfo> registration;
