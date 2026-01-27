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
//	OtAudioBuffer
//

class OtAudioBuffer {
public:
	// constructor
	inline OtAudioBuffer(size_t c) : channels(c) {
		buffer.resize(channels * OtAudioSettings::bufferSize);
	}

	// clear the buffer with specified value
	inline void clear(float value=0.0f) {
		std::fill(begin(), end(), value);
	}

	// access buffer members
	inline void set(size_t channel, size_t sample, float value) {
		buffer[channel * OtAudioSettings::bufferSize + sample] = value;
	}

	inline float& get(size_t channel, size_t sample) {
		return buffer[channel * OtAudioSettings::bufferSize + sample];
	}

	// mix signal with buffer
	inline void mix(size_t channel, size_t sample, float value) {
		buffer[channel * OtAudioSettings::bufferSize + sample] += value;
	}

	inline void mix(const OtAudioBuffer& input) {
		// sanity check
		OtAssert(channels == input.channels);

		for (size_t i = 0; i < size(); i++) {
			buffer[i] += input.buffer[i];
		}
	}

	// get buffer information
	inline size_t getChannelCount() { return channels; }
	inline float* getChannelData(size_t channel) { return buffer.data() + channel * OtAudioSettings::bufferSize; }

	inline float* data() { return buffer.data(); }
	inline size_t size() { return buffer.size(); }

	// iterator support
	inline float* begin() { return buffer.data(); }
	inline float* end() { return buffer.data() + buffer.size(); }

private:
	// buffer
	size_t channels = 0;
	std::vector<float> buffer;
};
