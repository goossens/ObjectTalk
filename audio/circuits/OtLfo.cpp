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
//	OtLfo
//

class OtLfo : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		output = addOutputPin("Output", OtCircuitPinClass::Type::control);

		frequencyControl = addControl("Freq", nullptr, &pitch)
			->setRange(0.1f, 200.0f)
			->setLabelFormat("%.1f")
			->setIsFrequency()
			->setIsLogarithmic();

		pulseWidthControl = addControl("PW", nullptr, &pulseWidth)
			->setRange(0.0f, 1.0f)
			->setLabelFormat("%.2f");
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		bool changed = false;
		ImGui::SetNextItemWidth(itemWidth);
		changed |= OtUi::selectorEnum("##waveForm", &waveForm, OtOscillator::waveForms, OtOscillator::waveFormCount);
		changed |= frequencyControl->renderKnob(); ImGui::SameLine();
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
		oscillator.setWaveForm(waveForm);

		for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
			oscillator.setPitch(frequencyControl->getValue(i));
			oscillator.setPulseWidth(pulseWidthControl->getValue(i));
			output->buffer->set(0, i, oscillator.get());
		}
	};

	static constexpr const char* circuitName = "LFO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtOscillator::WaveForm waveForm = OtOscillator::WaveForm::sine;
	float pitch = 1.0f;
	float pulseWidth = 0.5f;

	// work variables
	OtCircuitPin output;

	OtCircuitControl frequencyControl;
	OtCircuitControl pulseWidthControl;

	OtOscillator oscillator;
};

static OtCircuitFactoryRegister<OtLfo> registration;
