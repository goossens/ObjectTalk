//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <mutex>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtNeuralAudioModelAsset.h"


//
//	OtNeuralModelerCircuit
//

class OtNeuralModelerCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioInput = this->addInputPin("Input", OtCircuitPinClass::Type::mono);
		audioOutput = this->addOutputPin("Output", OtCircuitPinClass::Type::mono)->hasAttenuation();
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::PushItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 5.0f + ImGui::GetStyle().ItemInnerSpacing.x));
		std::lock_guard<std::mutex> guard(mutex);
		return asset.renderUI("Model");
	}


	inline float getCustomRenderingWidth() override {
		return 200.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["model"] = OtAssetSerialize(asset.getPath(), basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = OtAssetDeserialize(data, "model", basedir);
	}

	// generate output by evaluating MIDI messages
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			float output[OtAudioSettings::bufferSize];

			if (audioInput->isSourceConnected()) {
				if (asset.isReady()) {
					// get input and apply neural network
					float input[OtAudioSettings::bufferSize];
					audioInput->getSamples(input);
					std::lock_guard<std::mutex> guard(mutex);
					asset->getModel().process(OtAudioSettings::bufferSize, input, output);

				} else {
					// model not ready (yet), send input in bypass mode
					audioInput->getSamples(output);
				}

			} else {
				std::fill(output, output + OtAudioSettings::bufferSize, 0.0f);
			}

			audioOutput->setSamples(output);
		}
	}

	static constexpr const char* circuitName = "Neural Modeler";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::processor;

private:
	// properties
	OtAsset<OtNeuralAudioModelAsset> asset;

	// work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;
	std::mutex mutex;
};

static OtCircuitFactoryRegister<OtNeuralModelerCircuit> registration;
