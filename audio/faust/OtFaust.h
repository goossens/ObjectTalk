//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtAssert.h"

#include "OtUi.h"

#include "OtAudioSettings.h"
#include "OtAudioUi.h"
#include "OtCircuit.h"


//
//	OtFaust
//

class OtFaust {
protected:
	// Faust metadata collector
	class Meta {
	public:
		// add metadata
		void declare(const char* name, const char* value) {
			metadata[name] = value;
		}

	private:
		// properties
		std::unordered_map<std::string, std::string> metadata;
	} meta;

public:
	// destructor
	virtual ~OtFaust() {}

	// (de)serialize variables
	void serialize(nlohmann::json* data, std::string* basedir);
	void deserialize(nlohmann::json* data, std::string* basedir);

	virtual void iterateParameters(std::function<void(const char*, float*, float)>) = 0;
};


//
//	OtFaustCircuit
//

template<typename T>
class OtFaustCircuit : public OtCircuitClass {
public:
	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		return dsp.renderUI();
	}

	inline float getCustomRenderingWidth() override {
		return dsp.getRenderWidth();
	}

	inline float getCustomRenderingHeight() override {
		return dsp.getRenderHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		dsp.serialize(data, basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		dsp.deserialize(data, basedir);
	}

protected:
	// target Faust processor
	T dsp;
};


//
//	OtFaustEffectMono
//

template<typename T>
class OtFaustEffectMono : public OtFaustCircuit<T> {
public:
	// configure circuit pins
	inline void configure() override {
		OtAssert(this->dsp.getNumInputs() == 1);
		OtAssert(this->dsp.getNumOutputs() == 1);

		audioInput = this->addInputPin("Input", OtCircuitPinClass::Type::mono);
		audioOutput = this->addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize];
				float output[OtAudioSettings::bufferSize];

				auto in = input;
				auto out = output;

				audioInput->getSamples(input);
				this->dsp.compute(OtAudioSettings::bufferSize, &in, &out);
				audioOutput->setSamples(output);

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	}

private:
	// circuit work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;
};


//
//	OtFaustEffectStereo
//

template<typename T>
class OtFaustEffectStereo : public OtFaustCircuit<T> {
public:
	// configure circuit pins
	inline void configure() override {
		OtAssert(this->dsp.getNumInputs() == 2);
		OtAssert(this->dsp.getNumOutputs() == 2);

		audioInput = this->addInputPin("Input", OtCircuitPinClass::Type::stereo);
		audioOutput = this->addOutputPin("Output", OtCircuitPinClass::Type::stereo);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize * 2];
				float output[OtAudioSettings::bufferSize * 2];

				float* in[] = { input, input + OtAudioSettings::bufferSize };
				float* out[] = { output, output + OtAudioSettings::bufferSize };

				audioInput->getSamples(input);
				this->dsp.compute(OtAudioSettings::bufferSize, in, out);
				audioOutput->setSamples(output);

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	}

private:
	// circuit work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;
};
