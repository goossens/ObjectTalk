//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAudioSettings.h"


//
//	OtSmoothValue
//

template<typename T>
class OtSmoothValue {
public:
	// constructor
	inline OtSmoothValue(size_t t) : smoothingSamples(t) {}

	// reset state
	inline void reset(T value) {
		current = value;
		target = value;
		samplesRemaining = 0;
	}

	// get the next value
	inline T next(T value) {
		if (value != target) {
			samplesRemaining = smoothingSamples;
			target = value;
			step = (target - current) / static_cast<T>(samplesRemaining);
		}

		if (samplesRemaining == 0) {
			return target;

		} else {
			if (--samplesRemaining == 0) {
				current = target;

			} else {
				current += step;
			}

			return current;
		}
	}

private:
	// properties
	size_t smoothingSamples;

	// work variables
	size_t samplesRemaining{};
	bool first = true;
	T step{};
	T current{};
	T target{};
};
