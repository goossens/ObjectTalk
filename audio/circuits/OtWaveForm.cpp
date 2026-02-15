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
#include "OtWaveTableAsset.h"


//
//	OtWaveForm
//

class OtWaveForm : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		frequencyInput = addInputPin("Freq", OtCircuitPinClass::Type::frequency)->hasTuning(true);
		shapeInput = addInputPin("Shape", OtCircuitPinClass::Type::control)->hasAttenuation(true);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono)->hasAttenuation(true);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		bool changed = false;
		ImGui::SetNextItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 5.0f + ImGui::GetStyle().ItemInnerSpacing.x));
		changed |= waveTableAsset.renderUI("Table");

		if (!frequencyInput->isSourceConnected()) {
			changed |= OtUi::knob("Freq", &frequency, 60.0f, 6000.0f, "%.0fhz", true);
		}

		if (!shapeInput->isSourceConnected()) {
			if (!frequencyInput->isSourceConnected()) {
				ImGui::SameLine();
			}

			changed |= OtUi::knob("Shape", &shape, 0.0f, 1.0f, "%.001f");
		}

		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return 250.0f;
	}

	inline float getCustomRenderingHeight() override {
		bool needKnobs = !frequencyInput->isSourceConnected() || !shapeInput->isSourceConnected();
		return ImGui::GetFrameHeightWithSpacing() + (needKnobs ? OtUi::knobHeight() : 0.0f);
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["waveTable"] = OtAssetSerialize(waveTableAsset.getPath(), basedir);
		(*data)["frequency"] = frequency;
		(*data)["shape"] = shape;
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		waveTableAsset = OtAssetDeserialize(data, "waveTable", basedir);
		frequency = data->value("frequency", 440.0f);
		shape = data->value("shape", 440.0f);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (waveTableAsset.isReady()) {
				auto& waveTable = waveTableAsset->getWaveTable();
				float freq[OtAudioSettings::bufferSize];
				float shp[OtAudioSettings::bufferSize];
				float output[OtAudioSettings::bufferSize];

				if (frequencyInput->isSourceConnected()) {
					frequencyInput->getSamples(freq);

				} else {
					for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
						freq[i] = frequency;
					}
				}

				if (shapeInput->isSourceConnected()) {
					shapeInput->getSamples(shp);

				} else {
					for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
						shp[i] = shape;
					}
				}

				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					output[i] = waveTable.get(phase, shp[i]);
					phase += freq[i] * OtAudioSettings::dt;

					if (phase > 1.0f) {
						phase -= 1.0f;
					}
				}

				audioOutput->setSamples(output);

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

	static constexpr const char* circuitName = "WaveForm";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtCircuitPin frequencyInput;
	OtCircuitPin shapeInput;
	OtCircuitPin audioOutput;
	OtAsset<OtWaveTableAsset> waveTableAsset;
	float frequency = 440.0f;
	float shape = 0.0f;

	// work variables
	float phase = 0.0f;
};

static OtCircuitFactoryRegister<OtWaveForm> registration;
