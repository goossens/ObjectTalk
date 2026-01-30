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

#include "OtAdsrUi.h"


//
//	Support Functions
//

template <typename T>
inline T easing(T t) {
	return t * (static_cast<T>(2) - t);
}

static inline float* fillVisualizationBuffer(float* p, int samples, float start, float end) {
	auto range = end - start;

	for (auto i = 0; i < samples; i++) {
		*p++ = start + range * easing(static_cast<float>(i) / static_cast<float>(samples));
	}

	return p;
}

static inline int durationInSamples(float seconds, float sampleRate) {
	return std::max(1, static_cast<int>(std::floor(seconds * sampleRate)));
}


//
//	OtAdsrUi::renderUI
//

bool OtAdsrUi::renderUI() {
	if (update) {
		// get total envelope length and length of each phase in samples
		auto totalLengthInTime = (parameters.attack + parameters.decay + parameters.release) * 1.25f;
		auto sampleRate = static_cast<float>(envelopeDataSize) / totalLengthInTime;
		auto attackSamples = durationInSamples(parameters.attack, sampleRate);
		auto decaySamples = durationInSamples(parameters.decay, sampleRate);
		auto sustainSamples = durationInSamples(totalLengthInTime * 0.2f, sampleRate);
		auto releaseSamples = durationInSamples(parameters.release, sampleRate);
		auto roundingSamples = static_cast<int>(envelopeDataSize) - attackSamples - decaySamples - sustainSamples - releaseSamples;

		// populate output buffer
		auto p = graph.data();
		p = fillVisualizationBuffer(p, attackSamples, 0.0f, 1.0f);
		p = fillVisualizationBuffer(p, decaySamples, 1.0f, parameters.sustain);
		p = fillVisualizationBuffer(p, sustainSamples, parameters.sustain, parameters.sustain);
		p = fillVisualizationBuffer(p, releaseSamples, parameters.sustain, 0.0f);
		p = fillVisualizationBuffer(p, roundingSamples, 0.0f, 0.0f);
	}

	// render envelope
	if (ImPlot::BeginPlot("##AHDSR", ImVec2(getRenderWidth(), envelopeHeight), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs)) {
		ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels);
		ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);
		ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, envelopeDataSize);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -0.02, 1.02);
		ImPlot::PlotLine("Signal", graph.data(), envelopeDataSize);
		ImPlot::EndPlot();
	}

	// render envelope control knobs
	auto changed = false;
	changed |= OtUi::knob("Attack", &parameters.attack, 0.0f, 10.0f, "%.3fs", true); ImGui::SameLine();
	changed |= OtUi::knob("Decay", &parameters.decay, 0.0f, 10.0f, "%.3fs", true); ImGui::SameLine();
	changed |= OtUi::knob("Sustain", &parameters.sustain, 0.0f, 1.0f, "%.2f", false); ImGui::SameLine();
	changed |= OtUi::knob("Release", &parameters.release, 0.0f, 10.0f, "%.3fs", true);
	return changed;
}


//
//	OtAdsrUi::getRenderWidth
//

float OtAdsrUi::getRenderWidth() {
	return OtUi::knobWidth(4);
}


//
//	OtAdsrUi::getRenderHeight
//

float OtAdsrUi::getRenderHeight() {
	return envelopeHeight + OtUi::knobHeight();
}


//
//	OtAdsrUi::serialize
//

void OtAdsrUi::serialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	(*data)["attack"] = parameters.attack;
	(*data)["decay"] = parameters.decay;
	(*data)["sustain"] = parameters.sustain;
	(*data)["release"] = parameters.release;
}


//
//	OtAdsrUi::deserialize
//

void OtAdsrUi::deserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	parameters.attack = data->value("attack", 1.0f);
	parameters.decay = data->value("decay", 1.0f);
	parameters.sustain = data->value("sustain", 0.8f);
	parameters.release = data->value("release", 0.2f);
	update = true;
}
