//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://gist.github.com/agrafix/aa49c17cd32c8ba63b6a7cb8dce8b0bd


#pragma once


//
//	Include files
//

#include <cmath>
#include <vector>

#include "OtAudioSettings.h"


//
//	OtHammingWindow
//
//	Implements a tapering function used to minimize spectral leakage when performing Fourier transforms (FFTs)
//

class OtHammingWindow {
public:
	// constructor
	OtHammingWindow(size_t size) {
		hamming.resize(size);

		for (size_t i = 0; i < size; i++) {
			hamming[i] = 0.53836f - 0.46164f * std::cos(OtAudioSettings::pi2 * static_cast<float>(i) / (static_cast<float>(size)));
		}
	}

	// apply window to samples
	inline void apply(float* data) {
		for (size_t i = 0; i < hamming.size(); i++) {
			data[i] *= hamming[i];
		}
	}

private:
	// lookup table for faster processing
	std::vector<float> hamming;
};
