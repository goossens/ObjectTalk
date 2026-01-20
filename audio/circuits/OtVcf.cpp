//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtAudioFilter.h"
#include "OtAudioSettings.h"
#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"


//
//	OtVcf
//

class OtVcf : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioInput = addInputPin("Input", OtCircuitPinClass::Type::mono);
		fmInput = addInputPin("F Mod", OtCircuitPinClass::Type::control)->hasAttenuation();
		qmInput = addInputPin("Q Mod", OtCircuitPinClass::Type::control)->hasAttenuation();
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::PushItemWidth(itemWidth - parameters.getLabelWidth());
		bool changed = parameters.renderUI();
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

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		parameters.deserialize(data, basedir);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					audioOutput->setSample(i, state.process(parameters, audioInput->getSample(i)));
				}

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

	static constexpr const char* circuitName = "VCF";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::processor;

private:
	// properties
	OtAudioFilter::Parameters parameters;

	// work variables
	OtCircuitPin audioInput;
	OtCircuitPin fmInput;
	OtCircuitPin qmInput;
	OtCircuitPin audioOutput;

	OtAudioFilter::State state;
};

static OtCircuitFactoryRegister<OtVcf> registration;
