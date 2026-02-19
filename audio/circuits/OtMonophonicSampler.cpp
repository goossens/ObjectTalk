//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"
#include "OtUi.h"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtSampleFileAsset.h"


//
//	OtMonophonicSampler
//

class OtMonophonicSampler : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		frequencyInput = addInputPin("Freq", OtCircuitPinClass::Type::frequency)->hasTuning(true);
		gateInput = addInputPin("Gate", OtCircuitPinClass::Type::control);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono)->hasAttenuation(true);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		bool changed = false;
		ImGui::SetNextItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 4.0f + ImGui::GetStyle().ItemInnerSpacing.x));
		changed |= sampleFileAsset.renderUI("File");

		if (!frequencyInput->isSourceConnected()) {
			changed |= OtUi::knob("File", &frequency, 60.0f, 6000.0f, "%.0fhz", true);
		}

		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return 250.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + (frequencyInput->isSourceConnected() ? 0.0f : OtUi::knobHeight());
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["sampler"] = OtAssetSerialize(sampleFileAsset.getPath(), basedir);
		(*data)["frequency"] = frequency;
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		sampleFileAsset = OtAssetDeserialize(data, "sampler", basedir);
		frequency = data->value("frequency", 440.0f);
	}

	// process sampler
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (sampleFileAsset.isReady()) {
				auto& sampler = sampleFileAsset->getSampleFile();

				if (frequencyInput->isSourceConnected()) {
					for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
						frequency = frequencyInput->getSample(i);
					}
				}

				if (gateInput->isSourceConnected()) {
					for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
						auto state = gateInput->getSample(i);

						if (gate == 0.0f && state == 1.0f) {
							dt = frequency == 0.0f ? 1.0f : frequency / sampler.getRootFrequency();
							offset = 0.0f;
						}

						gate = state;
					}

				} else {
					gate = 0.0f;
				}

				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					audioOutput->setSample(i, sampler.get(offset));
					offset += dt;
				}

			} else {
				gate = 0.0f;
				audioOutput->setSamples(0.0f);
			}
		}
	};

	static constexpr const char* circuitName = "Monophonic Sampler";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtCircuitPin frequencyInput;
	OtCircuitPin gateInput;
	OtCircuitPin audioOutput;
	OtAsset<OtSampleFileAsset> sampleFileAsset;
	float frequency = 440.0f;

	// work variables
	float gate = 0.0f;
	float dt;
	float offset;
};

static OtCircuitFactoryRegister<OtMonophonicSampler> registration;
