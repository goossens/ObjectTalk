//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "implot.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtEnvelope.h"
#include "OtCircuitControl.h"
#include "OtCircuitFactory.h"


//
//	OtEnvelopeGenerator
//

class OtEnvelopeGenerator : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		triggerInput = addInputPin("Trigger", OtCircuitPinClass::Type::control);
		envelopeOutput = addOutputPin("Output", OtCircuitPinClass::Type::control);

		attackControl = addControl("Attack", nullptr, &attack)->setRange(0.0f, 10.0f)->setLabelFormat("%.2f")->setIsLogarithmic();
		holdControl = addControl("Hold", nullptr, &hold)->setRange(0.0f, 10.0f)->setLabelFormat("%.2f")->setIsLogarithmic();
		decayControl = addControl("Decay", nullptr, &decay)->setRange(0.0f, 10.0f)->setLabelFormat("%.2f")->setIsLogarithmic();
		sustainControl = addControl("Sustain", nullptr, &sustain)->setRange(0.0f, 1.0f)->setLabelFormat("%.2f");
		releaseControl = addControl("Release", nullptr, &release)->setRange(0.0f, 10.0f)->setLabelFormat("%.2f")->setIsLogarithmic();
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		if (updateVisualization) {
			envelope.setAttackTime(attack);
			envelope.setHoldTime(hold);
			envelope.setDecayTime(decay);
			envelope.setSustainLevel(sustain);
			envelope.setReleaseTime(release);
			envelope.getVisualization(visualization, visualizationSize);
		}

		if (ImPlot::BeginPlot("##AHDSR", ImVec2(itemWidth, visualizationHeight), ImPlotFlags_CanvasOnly)) {
			ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels);
			ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);
			ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, visualizationSize);
			ImPlot::SetupAxisLimits(ImAxis_Y1, -0.02, 1.02);
			ImPlot::PlotLine("Signal", visualization, visualizationSize);
			ImPlot::EndPlot();
		}

		bool changed = false;
		changed |= attackControl->renderKnob(); ImGui::SameLine();
		changed |= holdControl->renderKnob(); ImGui::SameLine();
		changed |= decayControl->renderKnob(); ImGui::SameLine();
		changed |= sustainControl->renderKnob(); ImGui::SameLine();
		changed |= releaseControl->renderKnob();
		updateVisualization = changed;
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::knobWidth(5);
	}

	inline float getCustomRenderingHeight() override {
		return visualizationHeight + OtUi::knobHeight();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["attack"] = attack;
		(*data)["hold"] = hold;
		(*data)["decay"] = decay;
		(*data)["sustain"] = sustain;
		(*data)["release"] = release;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		attack = data->value("attack", 1.0f);
		hold = data->value("hold", 0.0f);
		decay = data->value("decay", 1.0f);
		sustain = data->value("sustain", 0.5f);
		release = data->value("release", 0.2f);
		updateVisualization = true;
	}

	// generate samples
	void execute() override {
		if (triggerInput->isSourceConnected()) {
			envelope.setAttackTime(attack);
			envelope.setHoldTime(hold);
			envelope.setDecayTime(decay);
			envelope.setSustainLevel(sustain);
			envelope.setReleaseTime(release);

			auto triggerBuffer = triggerInput->getSignalBuffer();

			for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
				auto newTriggerState = triggerBuffer->get(0, i);

				if (newTriggerState != triggerState) {
					if (newTriggerState) {
						envelope.noteOn();

					} else {
						envelope.noteOff();
					}

					triggerState = newTriggerState;
				}

				envelopeOutput->buffer->set(0, i, envelope.process());
			}

		} else {
			envelopeOutput->buffer->clear(0.0f);
		}
	}

	static constexpr const char* circuitName = "Envelope Generator";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;
	static constexpr float width = 400.0f;
	static constexpr float visualizationHeight = 100.0f;
	static constexpr int visualizationSize = 1024;

private:
	// properties
	float attack = 1.0f;
	float hold = 0.0f;
	float decay = 1.0f;
	float sustain = 0.8f;
	float release = 0.2f;

	// work variables
	OtCircuitPin triggerInput;
	OtCircuitPin envelopeOutput;

	OtCircuitControl attackControl;
	OtCircuitControl holdControl;
	OtCircuitControl decayControl;
	OtCircuitControl sustainControl;
	OtCircuitControl releaseControl;

	bool triggerState = false;
	OtEnvelope envelope;
	float visualization[visualizationSize];
	bool updateVisualization = true;
};

static OtCircuitFactoryRegister<OtEnvelopeGenerator> registration;
