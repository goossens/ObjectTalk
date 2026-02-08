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
#include "OtAudioUi.h"
#include "OtCircuitFactory.h"
#include "OtLfo.h"


//
//	OtLfoCircuit
//

class OtLfoCircuit : public OtFaustCircuit<OtLfo> {
public:
	// configure pins
	inline void configurePins() override {
		OtAssert(this->dsp.getNumInputs() == 1);
		OtAssert(this->dsp.getNumOutputs() == 1);

		signalOutput = addOutputPin("Output", OtCircuitPinClass::Type::control);
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		auto waveform = dsp.getWaveForm();
		auto changed = OtAudioUi::waveFormSelector(&waveform);

		if (changed) {
			dsp.setWaveForm(waveform);
		}

		changed |= OtUi::knob("Freq", &frequency, 60.0f, 6000.0f, "%.0fhz", true);
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return OtAudioUi::waveFormSelectorWidth();
	}

	inline float getCustomRenderingHeight() override {
		return OtAudioUi::waveFormSelectorHeight() + OtUi::knobHeight();
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
		if (signalOutput->isDestinationConnected()) {
			float input[OtAudioSettings::bufferSize];
			float output[OtAudioSettings::bufferSize];

			auto freq = OtAudioUtilities::freqToCv(frequency);

			for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
				input[i] = freq;
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
	OtCircuitPin signalOutput;
	float frequency = 440.0f;
};

static OtCircuitFactoryRegister<OtLfoCircuit> registration;
