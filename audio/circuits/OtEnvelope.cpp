//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtAdsr.h"
#include "OtAudioUi.h"
#include "OtCircuitFactory.h"


//
//	OtEnvelopeCircuit
//

class OtEnvelopeCircuit : public OtFaustCircuit<OtAdsr> {
public:
	// configure pins
	inline void configure() override {
		OtAssert(this->dsp.getNumInputs() == 1);
		OtAssert(this->dsp.getNumOutputs() == 1);

		triggerInput = addInputPin("Trigger", OtCircuitPinClass::Type::control);
		envelopeOutput = addOutputPin("Output", OtCircuitPinClass::Type::control);
	}

	// render custom fields
	bool customRendering(float itemWidth) override {
		// render envelope
		state.attack = dsp.getAttack();
		state.decay = dsp.getDecay();
		state.sustain = dsp.getSustain();
		state.release = dsp.getRelease();
		OtAudioUi::adsrEnvelope("##ADSR", state, ImVec2(itemWidth, envelopeHeight));

		// render envelope control knobs
		auto changed = dsp.renderUI();
		state.update |= changed;
		return changed;
	}

	float getCustomRenderingHeight() override {
		return dsp.getRenderHeight() + envelopeHeight;
	}

	// deserialize circuit
	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		dsp.deserialize(data, basedir);
		state.update = true;
	}

	// generate samples
	void execute() override {
		if (envelopeOutput->isDestinationConnected()) {
			if (triggerInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize];
				float output[OtAudioSettings::bufferSize];

				auto in = input;
				auto out = output;

				triggerInput->getSamples(input);

				dsp.compute(OtAudioSettings::bufferSize, &in, &out);
				envelopeOutput->setSamples(output);

			} else {
				envelopeOutput->audioBuffer->clear(0.0f);
			}
		}
	}

	static constexpr const char* circuitName = "Envelope";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// work variables
	OtCircuitPin triggerInput;
	OtCircuitPin envelopeOutput;

	OtAudioUi::AdsrState state;
	static constexpr float envelopeHeight = 100.0f;
};

static OtCircuitFactoryRegister<OtEnvelopeCircuit> registration;
