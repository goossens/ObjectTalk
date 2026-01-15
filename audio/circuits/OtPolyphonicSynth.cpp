//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCircuitFactory.h"
#include "OtSynth.h"


//
//	OtPolyphonicSynth
//

class OtPolyphonicSynth : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono)->hasAttenuation();
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
		}
	};

	static constexpr const char* circuitName = "Polyphonic Synth";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtSynth synth;

	// work variables
	OtCircuitPin audioOutput;
};

static OtCircuitFactoryRegister<OtPolyphonicSynth> registration;
