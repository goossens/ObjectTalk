//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <array>
#include <cmath>

#include "OtAudioSettings.h"
#include "OtSmoothValue.h"


//
//	OtDelay
//

template<typename T, size_t N>
class OtDelay {
public:
	// process a sample
	inline T process(T sample, size_t delayInSamples) {
		write(sample);
		return read(static_cast<float>(delayInSamples));
	}

	inline T process(T sample, float delayInMs) {
		write(sample);
		return read(delayInMs * OtAudioSettings::sampleRate / 1000.0f);
	}

private:
	// properties
	std::array<T, N> history = {};
	size_t offset = 0;
	T value{};
	OtSmoothValue<T> smoothValue{OtAudioSettings::sampleRate / 10};

	// local support functions
	inline void write(T sample) {
		history[offset] = sample;
		offset = (offset + N - 1) % N;
	}

	inline T read(float delayInSamples) {
		delayInSamples = smoothValue.next(delayInSamples);
		delayInSamples = std::clamp(delayInSamples, 0.0f, static_cast<float>(N - 1));
		auto delayIntegral = static_cast<int>(std::floor(delayInSamples));
		auto delayFractional = delayInSamples - static_cast<float>(delayIntegral);

		if (delayFractional < static_cast<T>(0.618) && delayIntegral >= 1) {
			delayFractional++;
			delayIntegral--;
		}

		if (std::abs(delayFractional) < 0.0001f) {
			value = history[offset + delayIntegral];

		} else {
			auto index1 = offset + delayIntegral;
			auto index2 = index1 + 1;

			if (index2 >= N) {
				index1 %= N;
				index2 %= N;
			}

			auto value1 = history[index1];
			auto value2 = history[index2];

			auto alpha = (1.0f - delayFractional) / (1.0f + delayFractional);
			value = value2 + alpha * (value1 - value);
		}

		return value;
	}
};
