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

#include "OtAsset.h"
#include "OtUi.h"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtOscillator.h"
#include "OtWaveTable.h"
#include "OtWaveTableAsset.h"


//
//	OtVco
//

class OtVco : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		pitchInput = addInputPin("Pitch", OtCircuitPinClass::Type::control);
		pulseWidthInput = addInputPin("Pulse Width", OtCircuitPinClass::Type::control);
		shapeInput = addInputPin("Shape", OtCircuitPinClass::Type::control);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);

		pitchControl = addControl("Pitch", pitchInput, &pitch)
			->setRange(80.0f, 8000.0f)
			->setLabelFormat("%.0fhz")
			->setIsFrequency()
			->setIsLogarithmic();

		pulseWidthControl = addControl("PW", pulseWidthInput, &pulseWidth)
			->setRange(0.0f, 1.0f)
			->setLabelFormat("%.2f");

		shapeControl = addControl("Shape", shapeInput, &shape)
			->setRange(0.0f, 1.0f)
			->setLabelFormat("%.2f");
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		bool changed = OtUi::selectorEnum("##waveForm", &waveForm, OtOscillator::waveForms, OtOscillator::waveFormCount);
		bool knobs = false;

		if (changed) {
			needsSizing = true;
		}

		if (waveForm == OtOscillator::WaveForm::wavetable) {
			ImGui::SetNextItemWidth(itemWidth - ImGui::CalcTextSize(" Table").x);

			if (asset.renderUI(" Table")) {
				if (asset.isNull()) {
					wavetable.clear();

				} else if (asset.isReady()) {
					wavetable = asset->getWaveTable();

				} else {
					wavetable.clear();
				}

				changed |= true;
			}
		}

		if (!pitchInput->isSourceConnected()) {
			changed |= pitchControl->renderKnob();
			knobs = true;
		}

		if (!pulseWidthInput->isSourceConnected() && waveForm == OtOscillator::WaveForm::square) {
			if (knobs) { ImGui::SameLine(); }
			changed |= pulseWidthControl->renderKnob();
			knobs = true;
		}

		if (!shapeInput->isSourceConnected() && waveForm == OtOscillator::WaveForm::wavetable) {
			if (knobs) { ImGui::SameLine(); }
			changed |= shapeControl->renderKnob();
			knobs = true;
		}

		return changed;
	}

	inline float getCustomRenderingWidth() override {
		if (waveForm == OtOscillator::WaveForm::wavetable) {
			return 200.0f;

		} else {
			return OtUi::knobWidth(2);
		}
	}

	inline float getCustomRenderingHeight() override {
		float height = ImGui::GetFrameHeightWithSpacing();

		if (waveForm == OtOscillator::WaveForm::wavetable) {
			height += ImGui::GetFrameHeightWithSpacing();
		}

		size_t knobs = 0;
		if (!pitchInput->isSourceConnected()) { knobs++; }
		if (!pulseWidthInput->isSourceConnected() && waveForm == OtOscillator::WaveForm::square) { knobs++; }
		if (!shapeInput->isSourceConnected() && waveForm == OtOscillator::WaveForm::wavetable) { knobs++; }

		if (knobs) {
			height += OtUi::knobHeight();
		}

		return height;
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["waveForm"] = waveForm;
		(*data)["waveTable"] = OtAssetSerialize(asset.getPath(), basedir);
		(*data)["pitch"] = pitch;
		(*data)["pulseWidth"] = pulseWidth;
		(*data)["shape"] = shape;

	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		waveForm = data->value("waveForm", OtOscillator::WaveForm::sine);
		asset = OtAssetDeserialize(data, "waveTable", basedir);
		pitch = data->value("pitch", 440.0f);
		pulseWidth = data->value("pulseWidth", 0.5f);
		shape = data->value("shape", 0.0f);
	}

	// generate samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			// process all the samples
			if (waveForm == OtOscillator::WaveForm::wavetable) {
				if ((asset.isNull() || asset.isMissing() || asset.isInvalid()) && wavetable.isValid()) {
					wavetable.clear();

				} else if (asset.isReady() && asset->getWaveTable() != wavetable) {
					wavetable = asset->getWaveTable();
					oscillator.setWaveTable(&wavetable);
				}
			}

			for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
				audioOutput->setSample(i, oscillator.get(
					waveForm,
					pitchControl->getValue(i),
					pulseWidthControl->getValue(i),
					shapeControl->getValue(i)));
			}
		}
	};

	static constexpr const char* circuitName = "VCO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtOscillator::WaveForm waveForm = OtOscillator::WaveForm::sine;
	float pitch = 440.0f;
	float pulseWidth = 0.5f;
	float shape = 0.0f;

	OtAsset<OtWaveTableAsset> asset;
	OtWaveTable wavetable;

	// work variables
	OtCircuitPin pitchInput;
	OtCircuitPin pulseWidthInput;
	OtCircuitPin shapeInput;
	OtCircuitPin audioOutput;

	OtCircuitControl pitchControl;
	OtCircuitControl pulseWidthControl;
	OtCircuitControl shapeControl;

	OtOscillator oscillator;
};

static OtCircuitFactoryRegister<OtVco> registration;
