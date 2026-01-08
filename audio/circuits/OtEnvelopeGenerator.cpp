//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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

		attackControl = addControl("Attack", nullptr, &attack)->setRange(0.0f, 10.0f)->setLabelFormat("%.2fs")->setIsLogarithmic();
		holdControl = addControl("Hold", nullptr, &hold)->setRange(0.0f, 10.0f)->setLabelFormat("%.2fs")->setIsLogarithmic();
		decayControl = addControl("Decay", nullptr, &decay)->setRange(0.0f, 10.0f)->setLabelFormat("%.2fs")->setIsLogarithmic();
		sustainControl = addControl("Sustain", nullptr, &sustain)->setRange(0.0f, 1.0f)->setLabelFormat("%.2f");
		releaseControl = addControl("Release", nullptr, &release)->setRange(0.0f, 10.0f)->setLabelFormat("%.2fs")->setIsLogarithmic();
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		if (updateVisualization) {
			envelope.setAttackTime(attack);
			envelope.setHoldTime(hold);
			envelope.setDecayTime(decay);
			envelope.setSustainLevel(sustain);
			envelope.setReleaseTime(release);
			envelope.getVisualization(plotData, plotSize);
		}

		if (ImPlot::BeginPlot("##AHDSR", ImVec2(itemWidth, plotHeight), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs)) {
			ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels);
			ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);
			ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, plotSize);
			ImPlot::SetupAxisLimits(ImAxis_Y1, -0.02, 1.02);
			ImPlot::PlotLine("Signal", plotData, plotSize);
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
		return plotHeight + OtUi::knobHeight();
	}

	// (de)serialize circuit
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
		if (envelopeOutput->isDestinationConnected()) {
			if (triggerInput->isSourceConnected()) {
				envelope.setAttackTime(attack);
				envelope.setHoldTime(hold);
				envelope.setDecayTime(decay);
				envelope.setSustainLevel(sustain);
				envelope.setReleaseTime(release);

				for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
					auto newTriggerState = triggerInput->getSample(i) > 0.5f;

					if (newTriggerState != triggerState) {
						if (newTriggerState) {
							envelope.noteOn();

						} else {
							envelope.noteOff();
						}

						triggerState = newTriggerState;
					}

					envelopeOutput->setSample(i, envelope.process());
				}

			} else {
				envelopeOutput->buffer->clear(0.0f);
			}
		}
	}

	static constexpr const char* circuitName = "Envelope Generator";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;
	static constexpr float plotHeight = 100.0f;
	static constexpr int plotSize = 1024;

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
	float plotData[plotSize];
	bool updateVisualization = true;
};

static OtCircuitFactoryRegister<OtEnvelopeGenerator> registration;
