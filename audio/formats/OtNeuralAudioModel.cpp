//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "OtLog.h"

#include "OtAudioSettings.h"
#include "OtNeuralAudioModel.h"


//
//	OtNeuralAudioModel::load
//

void OtNeuralAudioModel::load(const std::string& path) {
	model = NeuralAudio::NeuralModel::CreateFromFile(path);

	if (model == nullptr) {
		OtLogError("Can't open Neural Audio Model [{}]");
	}

	// determine automatic gain correction
	gain = std::pow(10.0f, (model->GetRecommendedOutputDBAdjustment()) * 0.05f);

	auto modelSampleRate = model->GetSampleRate();
	auto adjustedSampleRate = static_cast<size_t>(modelSampleRate);

	if (modelSampleRate <= 0.0f) {
		sampleRate = 48000;

	} else if (adjustedSampleRate > OtAudioSettings::sampleRate) {
		sampleRate = adjustedSampleRate;

	} else if (adjustedSampleRate < OtAudioSettings::sampleRate) {
		sampleRate = adjustedSampleRate;

	} else {
		sampleRate = OtAudioSettings::sampleRate;
	}
}


//
//	OtNeuralAudioModel::process
//

void OtNeuralAudioModel::process(size_t size, float* input, float* output) {
	// run the samples through the neural network
	model->Process(input, output, size);

	// adjust output levels
	for (size_t i = 0; i < size; i++) {
		output[i] *= gain;
	}
}
