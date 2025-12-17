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

#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"
#include "OtOscillator.h"


//
//	OtVco
//

class OtVco : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		frequency = addInputPin("Frequency", OtCircuitPinClass::Type::control);
		pulseWidth = addInputPin("Pulse Width", OtCircuitPinClass::Type::control);
		output = addOutputPin("Output", OtCircuitPinClass::Type::mono);

		frequencyControl = addControl("Freq", frequency, &manualFrequency)
			->setRange(80.0f, 8000.0f)
			->setLabelFormat("%.0f")
			->setIsFrequency()
			->setIsLogarithmic();

		pulseWidthControl = addControl("PW", pulseWidth, &manualPulseWidth)
			->setRange(0.0f, 1.0f)
			->setLabelFormat("%.2f");
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		auto old = serialize().dump();
		auto changed = false;

		if (OtUi::selectorEnum("##waveForm", &waveForm, OtOscillator::waveForms, OtOscillator::waveFormCount)) {
			oscillator.setWaveForm(waveForm);
			changed = true;
		}

		if (frequencyControl->renderKnob()) { changed |= true; }
		ImGui::SameLine();
		if (pulseWidthControl->renderKnob()) { changed |= true; }

		if (changed) {
			oldState = old;
			newState = serialize().dump();
			needsSaving = true;
		}
	}

	inline float getCustomRenderingWidth() override {
		return 200.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + OtUi::knobHeight();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["waveForm"] = waveForm;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		waveForm = data->value("waveForm", OtOscillator::WaveForm::sine);
	}

	// generate samples
	void execute() override {
		// process all the samples
		for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
			oscillator.setFrequency(frequencyControl->getValue(i));
			oscillator.setPulseWidth(pulseWidthControl->getValue(i));
			output->buffer->set(0, i, oscillator.get());
		}
	};

	static constexpr const char* circuitName = "VCO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtCircuitPin frequency;
	OtCircuitPin pulseWidth;
	OtCircuitPin output;

	OtCircuitControl frequencyControl;
	OtCircuitControl pulseWidthControl;
	float manualFrequency = 440.0f;
	float manualPulseWidth = 0.5f;

	OtOscillator oscillator;
	OtOscillator::WaveForm waveForm = OtOscillator::WaveForm::sine;
};

static OtCircuitFactoryRegister<OtVco> registration;
