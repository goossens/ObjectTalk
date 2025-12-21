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

#include "OtUi.h"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtOscillator.h"


//
//	OtVco
//

class OtVco : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		pitchOutput = addInputPin("Pitch", OtCircuitPinClass::Type::control);
		pulseWidthOutput = addInputPin("Pulse Width", OtCircuitPinClass::Type::control);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);

		pitchControl = addControl("Pitch", pitchOutput, &pitch)
			->setRange(80.0f, 8000.0f)
			->setLabelFormat("%.0f")
			->setIsPitch()
			->setIsLogarithmic();

		pulseWidthControl = addControl("PW", pulseWidthOutput, &pulseWidth)
			->setRange(0.0f, 1.0f)
			->setLabelFormat("%.2f");
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		bool changed = false;

		if (OtUi::selectorEnum("##waveForm", &waveForm, OtOscillator::waveForms, OtOscillator::waveFormCount)) {
			oscillator.setWaveForm(waveForm);
			changed = true;
		}

		changed |= pitchControl->renderKnob(); ImGui::SameLine();
		changed |= pulseWidthControl->renderKnob();
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::knobWidth(2);
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + OtUi::knobHeight();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["waveForm"] = waveForm;
		(*data)["pitch"] = pitch;
		(*data)["pulseWidth"] = pulseWidth;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		waveForm = data->value("waveForm", OtOscillator::WaveForm::sine);
		pitch = data->value("pitch", 440.0f);
		pulseWidth = data->value("pulseWidth", 0.5f);
	}

	// generate samples
	void execute() override {
		// process all the samples
		for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
			oscillator.setPitch(pitchControl->getValue(i));
			oscillator.setPulseWidth(pulseWidthControl->getValue(i));
			audioOutput->buffer->set(0, i, oscillator.get());
		}
	};

	static constexpr const char* circuitName = "VCO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtCircuitPin pitchOutput;
	OtCircuitPin pulseWidthOutput;
	OtCircuitPin audioOutput;

	OtCircuitControl pitchControl;
	OtCircuitControl pulseWidthControl;
	float pitch = 440.0f;
	float pulseWidth = 0.5f;

	OtOscillator oscillator;
	OtOscillator::WaveForm waveForm = OtOscillator::WaveForm::sine;
};

static OtCircuitFactoryRegister<OtVco> registration;
