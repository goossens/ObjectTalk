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
//	OtOscillator::Parameters::renderUI
//

bool OtEnvelope::Parameters::renderUI() {
	if (update) {
		// get total envelope length and length of each phase in samples
		auto totalLengthInTime = (attackTime + decayTime + releaseTime) * 1.25f;
		auto sampleRate = static_cast<float>(envelopeDataSize) / totalLengthInTime;
		auto attackSamples = durationInSamples(attackTime, sampleRate);
		auto decaySamples = durationInSamples(decayTime, sampleRate);
		auto sustainSamples = durationInSamples(totalLengthInTime * 0.2f, sampleRate);
		auto releaseSamples = durationInSamples(releaseTime, sampleRate);
		auto roundingSamples = envelopeDataSize - attackSamples - decaySamples - sustainSamples - releaseSamples;

		// populate output buffer
		auto p = graph.data();
		p = fillVisualizationBuffer(p, attackSamples, 0.0f, 1.0f);
		p = fillVisualizationBuffer(p, decaySamples, 1.0f, sustainLevel);
		p = fillVisualizationBuffer(p, sustainSamples, sustainLevel, sustainLevel);
		p = fillVisualizationBuffer(p, releaseSamples, sustainLevel, 0.0f);
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
	changed |= OtUi::knob("Attack", &attackTime, 0.0f, 10.0f, "%.2fs", true); ImGui::SameLine();
	changed |= OtUi::knob("Decay", &decayTime, 0.0f, 10.0f, "%.2fs", true); ImGui::SameLine();
	changed |= OtUi::knob("Sustain", &sustainLevel, 0.0f, 1.0f, "%.2f", false); ImGui::SameLine();
	changed |= OtUi::knob("Release", &releaseTime, 0.0f, 10.0f, "%.2fs", true);
	return changed;
}


//
//	OtEnvelope::Parameters::getRenderWidth
//

float OtEnvelope::Parameters::getRenderWidth() {
	return OtUi::knobWidth(4);
}


//
//	OtAudioUi::Parameters::getRenderHeight
//

float OtEnvelope::Parameters::getRenderHeight() {
	return envelopeHeight + OtUi::knobHeight();
}


//
//	OtEnvelope::Parameters::serialize
//

void OtEnvelope::Parameters::serialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	(*data)["attack"] = attackTime;
	(*data)["decay"] = decayTime;
	(*data)["sustain"] = sustainLevel;
	(*data)["release"] = releaseTime;
}


//
//	OtEnvelope::Parameters::deserialize
//

void OtEnvelope::Parameters::deserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	attackTime = data->value("attack", 1.0f);
	decayTime = data->value("decay", 1.0f);
	sustainLevel = data->value("sustain", 0.8f);
	releaseTime = data->value("release", 0.2f);
	update = true;
}
