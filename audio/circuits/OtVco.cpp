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
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		auto old = serialize().dump();

		if (OtUi::selectorEnum("##waveForm", &waveForm, OtOscillator::waveForms, OtOscillator::waveFormCount)) {
			oscillator.setWaveForm(waveForm);
			oldState = old;
			newState = serialize().dump();
			needsSaving = true;
		}

		if (frequency->isSourceConnected()) { ImGui::BeginDisabled(); }
		if (OtUi::knob("Freq", &manualFrequency, 80.0f, 8000.0f, "%.0f", true)) {}
		if (frequency->isSourceConnected()) { ImGui::EndDisabled(); }

		ImGui::SameLine();

		if (pulseWidth->isSourceConnected() || waveForm != OtOscillator::WaveForm::square) { ImGui::BeginDisabled(); }
		if (OtUi::knob("Width", &manualPulseWidth, 0.0f, 1.0f, "%.2f")) {}
		if (pulseWidth->isSourceConnected() || waveForm != OtOscillator::WaveForm::square) { ImGui::EndDisabled(); }
	}

	inline float getCustomRenderingWidth() override {
		return 200.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + ImGui::GetTextLineHeight() * 6.0f;
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["waveForm"] = waveForm;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		waveForm = data->value("waveForm", OtOscillator::WaveForm::sine);
	}

	// generate samples
	void execute(size_t sampleRate, size_t samples) override {
		output->buffer->resize(samples);
		oscillator.setSampleRate(sampleRate);

		// process all the samples
		for (size_t i = 0; i < samples; i++) {
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
	OtOscillator oscillator;

	OtOscillator::WaveForm waveForm = OtOscillator::WaveForm::sine;
	float manualFrequency = 440.0f;
	float manualPulseWidth = 0.5f;
};

static OtCircuitFactoryRegister<OtVco> registration;
