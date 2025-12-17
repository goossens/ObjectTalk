//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtSignalBuffer
//

class OtSignalBuffer {
public:
	// constructor
	inline OtSignalBuffer(size_t c) : channels(c) {
		buffer.resize(channels * OtAudioSettings::bufferSize);
	}

	// clear the buffer with specified value
	inline void clear(float value=0.0f) {
		std::fill(buffer.begin(), buffer.end(), value);
	}

	// access buffer members
	inline void set(size_t channel, size_t sample, float value) {
		buffer[channels * sample + channel] = value;
	}

	inline float& get(size_t channel, size_t sample) {
		return buffer[channels * sample + channel];
	}

	// mix signal with buffer
	inline void mix(size_t channel, size_t sample, float value) {
		buffer[channels * sample + channel] += value;
	}

	inline void mix(const OtSignalBuffer* input) {
		// sanity check
		OtAssert(buffer.size() == input->buffer.size());

		for (size_t i = 0; i < buffer.size(); i++) {
			buffer[i] += input->buffer[i];
		}
	}

	// get buffer information
	inline size_t getChannelCount() { return channels; }
	inline size_t getSampleCount() { return OtAudioSettings::bufferSize; }
	inline float* data() { return buffer.data(); }

	// interator support
	inline std::vector<float>::iterator begin() { return buffer.begin(); }
	inline std::vector<float>::iterator end() { return buffer.end(); }

private:
	// signal buffer (channels are interleaved)
	size_t channels;
	std::vector<float> buffer;
};
