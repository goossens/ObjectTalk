//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtOscillator.h"


//
//	OtVco
//

class OtVco : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		frequencyInput = addInputPin("Pitch", OtCircuitPinClass::Type::control);
		pulseWidthInput = addInputPin("Pulse Width", OtCircuitPinClass::Type::control);
		shapeInput = addInputPin("Shape", OtCircuitPinClass::Type::control);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);

		parameters.showFrequencyKnob = true;
		parameters.frequencyKnobLow = 80.0f;
		parameters.frequencyKnobHigh = 8000.0f;
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
		parameters.showFrequencyKnob = !frequencyInput->isSourceConnected();
		parameters.showPulseWidthKnob = parameters.waveForm == OtOscillator::WaveForm::square && !pulseWidthInput->isSourceConnected();
		parameters.showShapeKnob = parameters.waveForm == OtOscillator::WaveForm::wavetable && !shapeInput->isSourceConnected();
		return parameters.getRenderHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		parameters.serialize(data, basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		parameters.deserialize(data, basedir);
	}

	// generate samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (parameters.isReady()) {
				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					state.frequency = frequencyInput->isSourceConnected() ? OtAudioUtilities::cvToPitch(frequencyInput->getSample(i)) : parameters.frequency;
					state.pulseWidth = pulseWidthInput->isSourceConnected() ? pulseWidthInput->getSample(i) : parameters.pulseWidth;
					state.shape = shapeInput->isSourceConnected() ? shapeInput->getSample(i) : parameters.shape;
					audioOutput->setSample(i, state.get(parameters));
				}

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

	static constexpr const char* circuitName = "VCO";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtOscillator::Parameters parameters;

	// work variables
	OtOscillator::State state;

	OtCircuitPin frequencyInput;
	OtCircuitPin pulseWidthInput;
	OtCircuitPin shapeInput;
	OtCircuitPin audioOutput;
};

static OtCircuitFactoryRegister<OtVco> registration;
