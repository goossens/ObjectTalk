//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtCircuitFactory.h"
#include "OtWaveTable.h"
#include "OtWaveTableAsset.h"

//
//	OtWto
//

class OtWto : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		pitchInput = addInputPin("Pitch", OtCircuitPinClass::Type::control);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);

		pitchControl = addControl("Pitch", pitchInput, &pitch)
			->setRange(80.0f, 8000.0f)
			->setLabelFormat("%.0fhz")
			->setIsFrequency()
			->setIsLogarithmic();
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth - ImGui::CalcTextSize("Wave Table  ").x);
		bool changed = asset.renderUI("Wave Table");

		if (changed) {
			if (asset.isNull()) {
				wavetable.clear();

			} else if (asset.isReady()) {
				wavetable = asset->getWaveTable();

			} else {
				wavetable.clear();
			}
		}

		if (!pitchInput->isSourceConnected()) {
			changed |= pitchControl->renderKnob();
		}

		if (changed) {
			needsSizing = true;
		}

		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return 250.0f;
	}

	inline float getCustomRenderingHeight() override {
		float height = ImGui::GetFrameHeightWithSpacing();

		if (!pitchInput->isSourceConnected()) {
			height += OtUi::knobHeight();
		}

		return height;
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["path"] = OtAssetSerialize(asset.getPath(), basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = OtAssetDeserialize(data, "path", basedir);
	}

	// generate samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if ((asset.isNull() || asset.isMissing() || asset.isInvalid()) && wavetable.isValid()) {
				wavetable.clear();

			} else if (asset.isReady() && asset->getWaveTable() != wavetable) {
				wavetable = asset->getWaveTable();
			}

			if (wavetable.isValid()) {
				// process all the samples
				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					audioOutput->setSample(i, wavetable.get(pitchControl->getValue(i)));
				}

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

	static constexpr const char* circuitName = "Wave Table Oscillator";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtAsset<OtWaveTableAsset> asset;
	OtWaveTable wavetable;
	float pitch = 440.0f;

	// work variables
	OtCircuitPin pitchInput;
	OtCircuitPin audioOutput;

	OtCircuitControl pitchControl;
};

static OtCircuitFactoryRegister<OtWto> registration;
