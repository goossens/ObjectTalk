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
#include "OtAudioUi.h"
#include "OtCircuitFactory.h"
#include "OtVco.h"


//
//	OtVcoCircuit
//

class OtVcoCircuit : public OtFaustCircuit<OtVco> {
public:
	// configure pins
	inline void configure() override {
		OtAssert(this->dsp.getNumInputs() == 1);
		OtAssert(this->dsp.getNumOutputs() == 1);

		frequencyInput = addInputPin("Freq", OtCircuitPinClass::Type::frequency)->hasTuning(true);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono)->hasAttenuation(true);
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		auto waveform = dsp.getWaveForm();
		auto changed = OtAudioUi::waveFormSelector(&waveform);

		if (changed) {
			dsp.setWaveForm(waveform);
		}

		if (!frequencyInput->isSourceConnected()) {
			changed |= OtUi::knob("Freq", &frequency, 60.0f, 6000.0f, "%.0fhz", true);
		}

		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return OtAudioUi::waveFormSelectorWidth();
	}

	inline float getCustomRenderingHeight() override {
		return OtAudioUi::waveFormSelectorHeight() + (frequencyInput->isSourceConnected() ? 0.0f : OtUi::knobHeight());
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		dsp.serialize(data, basedir);
		(*data)["frequency"] = frequency;
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		dsp.deserialize(data, basedir);
		frequency = data->value("frequency", 440.0f);
	}

	// generate samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			float input[OtAudioSettings::bufferSize];
			float output[OtAudioSettings::bufferSize];

			if (frequencyInput->isSourceConnected()) {
				frequencyInput->getSamples(input);

			} else {
				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					input[i] = frequency;
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
	float frequency = 440.0f;
};

static OtCircuitFactoryRegister<OtVcoCircuit> registration;
