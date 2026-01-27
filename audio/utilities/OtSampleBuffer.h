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
#include <vector>

#include "OtAssert.h"

#include "OtAudioSettings.h"


//
//	OtSampleBuffer
//

class OtSampleBuffer {
public:
	// constructor
	inline OtSampleBuffer(size_t c, size_t s) : channels(c), samples(s) {
		buffer.resize(channels * samples);
	}

	// resize the buffer
	inline void resize(size_t c, size_t s) {
		if (channels != c || samples != s) {
			channels = c;
			samples = s;
			buffer.resize(channels * samples);
		}
	}

	// clear the buffer with specified value
	inline void clear(float value=0.0f) {
		std::fill(begin(), end(), value);
	}

	// resize and load the buffer
	inline void load(float* data, size_t newChannels, size_t newSamples) {
		buffer.resize(newChannels * newSamples);
		channels = newChannels;
		samples = newSamples;

		for (size_t i = 0; i < size(); i++) {
			buffer[i] += data[i];
		}
	}

	// access buffer members
	inline void set(size_t channel, size_t sample, float value) {
		buffer[channel * samples + sample] = value;
	}

	inline float& get(size_t channel, size_t sample) {
		return buffer[channel * samples + sample];
	}

	// get buffer information
	inline size_t getChannelCount() { return channels; }
	inline size_t getSampleCount() { return samples; }
	inline float getDurationInSeconds() { return static_cast<float>(samples) * OtAudioSettings::dt; }

	inline float* data() { return buffer.data(); }
	inline size_t size() { return buffer.size(); }

	// iterator support
	inline float* begin() { return buffer.data(); }
	inline float* end() { return buffer.data() + buffer.size(); }

private:
	// buffer (channels are interleaved)
	size_t channels = 0;
	size_t samples = 0;
	std::vector<float> buffer;
};
