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
#include "OtOscillator.h"


//
//	OtLfo
//

class OtLfo : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		output = addOutputPin("Output", OtCircuitPinClass::Type::control);

		parameters.showFrequencyKnob = true;
		parameters.frequencyKnobLow = 0.1f;
		parameters.frequencyKnobHigh = 200.0f;
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::PushItemWidth(itemWidth - parameters.getLabelWidth());
		auto changed = parameters.renderUI();
		ImGui::PopItemWidth();
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return parameters.getRenderWidth();
	}

	inline float getCustomRenderingHeight() override {
		return parameters.getRenderHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		parameters.serialize(data, basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		parameters.deserialize(data, basedir);
	}

	// generate samples
	void execute() override {
		if (output->isDestinationConnected()) {
			// process all the samples
			for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
				output->setSample(i, state.get(parameters));
			}
		}
	};

	static constexpr const char* circuitName = "LFO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtOscillator::Parameters parameters;

	// work variables
	OtOscillator::State state;
	OtCircuitPin output;
};

static OtCircuitFactoryRegister<OtLfo> registration;
