//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "NeuralAudio/NeuralModel.h"


//
//	OtNeuralAudioModel
//

class OtNeuralAudioModel {
public:
	// destructor
	~OtNeuralAudioModel() { clear(); }

	// load the specified model file);
	void load(const std::string& path);

	// clear the model
	inline void clear() {
		delete model;
		model = nullptr;
	}

	// get information
	inline bool isValid() { return model != nullptr; }

	// see if model are identical
	inline bool operator==(OtNeuralAudioModel& rhs) { return model == rhs.model; }
	inline bool operator!=(OtNeuralAudioModel& rhs) { return !operator==(rhs); }

	// run set of samples through the model
	void process(size_t size, float* input, float* output);

private:
	// properties
	NeuralAudio::NeuralModel* model;
	float gain = 1.0f;
	size_t sampleRate;
};
