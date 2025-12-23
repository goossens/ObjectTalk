//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>


//
//	OtDelay
//

class OtDelay {
public:
	// constructor
	inline OtDelay(size_t maxDelayInSamples) {
		history.resize(maxDelayInSamples);
	}

	// process a sample
	inline float process(float sample, size_t delayInSamples) {
		if (delayInSamples) {
			offset = (offset + 1) % delayInSamples;
			auto result = history[offset];
			history[offset] = sample;
			return result;

		} else {
			return sample;
		}
	}

private:
	// properties
	std::vector<float> history;
	size_t offset = 0;
};
