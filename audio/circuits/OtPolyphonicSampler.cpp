//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtSamplerSynth.h"


//
//	OtPolyphonicSampler
//

class OtPolyphonicSampler : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		midiInput = addInputPin("MIDI", OtCircuitPinClass::Type::midi);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		return synth.renderUI(itemWidth);
	}

	inline float getCustomRenderingWidth() override {
		return synth.getRenderWidth();
	}

	inline float getCustomRenderingHeight() override {
		return synth.getRenderHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		synth.serialize(data, basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		synth.deserialize(data, basedir);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (midiInput->isSourceConnected()) {
				for (auto& message : midiInput->getMidiInputBuffer()) {
					synth.processMidiMessage(message);
				}

				float buffer[OtAudioSettings::bufferSize];
				synth.get(buffer, OtAudioSettings::bufferSize);
				audioOutput->setSamples(buffer);
			}
		}
	};

	static constexpr const char* circuitName = "Polyphonic Sampler";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtSamplerSynth synth;

	// work variables
	OtCircuitPin midiInput;
	OtCircuitPin audioOutput;
};

static OtCircuitFactoryRegister<OtPolyphonicSampler> registration;
