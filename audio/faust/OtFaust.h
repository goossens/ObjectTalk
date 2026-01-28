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
#include "OtCircuitFactory.h"
#include "OtAudioBuffer.h"


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

	// unused for now
	struct Soundfile{};

public:
	// destructor
	virtual ~OtFaust() {}

	// initialize processor
	void initialize() {
		init(OtAudioSettings::sampleRate);
		metadata(&meta);
	}

	// collect metadata
	virtual void metadata(Meta* meta) = 0;

	// return number of audio inputs/outputs
	virtual int getNumInputs() = 0;
	virtual int getNumOutputs() = 0;

	// return the sample rate
	virtual int getSampleRate() = 0;

	// initialize the DSP unit
	virtual void init(int sampleRate) = 0;

	// run the Faust DSP unit
	virtual void compute(int count, float** inputs, float** outputs) = 0;

	// (de)serialize variables
	void serialize(nlohmann::json* data);
	void deserialize(nlohmann::json* data);

	virtual void iterateParameters(std::function<void(const char*, float*, float)>) = 0;
};


//
//	OtFaustCircuit
//

template<typename T>
class OtFaustCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		dsp.initialize();
		configurePins();
	}

	virtual void configurePins() {}

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
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		dsp.serialize(data);
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		dsp.deserialize(data);
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
	inline void configurePins() override {
		OtAssert(this->dsp.getNumInputs() == 1);
		OtAssert(this->dsp.getNumOutputs() == 1);

		audioInput = this->addInputPin("Input", OtCircuitPinClass::Type::mono);
		audioOutput = this->addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				auto in = audioInput->getAudioInputBuffer().data();
				auto out = audioOutput->getAudioOutputBuffer().data();
				this->dsp.compute(OtAudioSettings::bufferSize, &in, &out);

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

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
	inline void configurePins() override {
		OtAssert(this->dsp.getNumInputs() == 2);
		OtAssert(this->dsp.getNumOutputs() == 2);

		audioInput = this->addInputPin("Input", OtCircuitPinClass::Type::stereo);
		audioOutput = this->addOutputPin("Output", OtCircuitPinClass::Type::stereo);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				auto& input = audioInput->getAudioInputBuffer();
				auto& output = audioOutput->getAudioOutputBuffer();

				float* inputs[] = { input.getChannelData(0), input.getChannelData(1) };
				float* outputs[] = { output.getChannelData(0), output.getChannelData(1) };
				this->dsp.compute(OtAudioSettings::bufferSize, inputs, outputs);

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

private:
	// circuit work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;
};
