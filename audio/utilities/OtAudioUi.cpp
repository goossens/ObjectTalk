//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "imgui.h"
#include "implot.h"

#include "OtUi.h"

#include "OtAudioSettings.h"
#include "OtAudioUi.h"


//
//	Constants

static constexpr size_t envelopeDataSize = 1024;
static constexpr float envelopeHeight = 100.0f;

//
//	Support Functions
//

static inline int durationInSamples(float seconds, float sampleRate=OtAudioSettings::sampleRate) {
	return std::max(1, static_cast<int>(std::floor(seconds * sampleRate)));
}

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


//
//	OtAudioUi::envelope
//

bool OtAudioUi::envelope(float* attack, float* hold, float* decay, float* sustain, float* release, std::vector<float>* values, bool update) {
	if (update) {
		// get total envelope length and length of each phase in samples
		auto totalLengthInTime = (*attack + *hold + *decay + *release) * 1.25f;
		auto sampleRate = static_cast<float>(envelopeDataSize) / totalLengthInTime;
		auto attackSamples = durationInSamples(*attack, sampleRate);
		auto holdSamples = durationInSamples(*hold, sampleRate);
		auto decaySamples = durationInSamples(*decay, sampleRate);
		auto sustainSamples = durationInSamples(totalLengthInTime * 0.2f, sampleRate);
		auto releaseSamples = durationInSamples(*release, sampleRate);
		auto roundingSamples = envelopeDataSize - attackSamples - holdSamples - decaySamples - sustainSamples - releaseSamples;

		// populate output buffer
		values->resize(envelopeDataSize);
		auto p = values->data();
		p = fillVisualizationBuffer(p, attackSamples, 0.0f, 1.0f);
		p = fillVisualizationBuffer(p, holdSamples, 1.0f, 1.0f);
		p = fillVisualizationBuffer(p, decaySamples, 1.0f, *sustain);
		p = fillVisualizationBuffer(p, sustainSamples, *sustain, *sustain);
		p = fillVisualizationBuffer(p, releaseSamples, *sustain, 0.0f);
		p = fillVisualizationBuffer(p, roundingSamples, 0.0f, 0.0f);
	}

	// render envelope
	if (ImPlot::BeginPlot("##AHDSR", ImVec2(getEnvelopeWidth(), envelopeHeight), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs)) {
		ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels);
		ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);
		ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, envelopeDataSize);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -0.02, 1.02);
		ImPlot::PlotLine("Signal", values->data(), envelopeDataSize);
		ImPlot::EndPlot();
	}

	// render envelope control knobs
	bool changed = false;
	changed |= OtUi::knob("Attack", attack, 0.0f, 10.0f, "%.2fs", true); ImGui::SameLine();
	changed |= OtUi::knob("Hold", hold, 0.0f, 10.0f, "%.2fs", true); ImGui::SameLine();
	changed |= OtUi::knob("Decay", decay, 0.0f, 10.0f, "%.2fs", true); ImGui::SameLine();
	changed |= OtUi::knob("Sustain", sustain, 0.0f, 1.0f, "%.2f", false); ImGui::SameLine();
	changed |= OtUi::knob("Release", release, 0.0f, 10.0f, "%.2fs", true);
	return changed;
}


//
//	OtAudioUi::getEnvelopeWidth
//

float OtAudioUi::getEnvelopeWidth() {
	return OtUi::knobWidth(5);
}


//
//	OtAudioUi::getEnvelopeHeight
//

float OtAudioUi::getEnvelopeHeight() {
	return envelopeHeight + OtUi::knobHeight();
}
