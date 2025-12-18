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

#include "OtAssert.h"


//
//	OtCircularBuffer
//

template<typename T, size_t N>
class OtCircularBuffer {
public:
	// clear the buffer with specified value
	inline void clear(T sample) {
		std::fill(buffer, buffer + N, sample);
	}

	// insert samples
	inline void insert(T* samples, size_t count) {
		OtAssert(count < N);
		std::copy(buffer + count, buffer + N, buffer);
		std::copy(samples, samples + count, buffer + N - count);
	}

	// access members
	inline T operator[](size_t i) const { return buffer[i]; }
	inline T& operator[](size_t i) { return buffer[i]; }

	// queue information
	inline T* data() { return buffer; }
	inline size_t size() { return N; }

	// interator support
	inline T* begin() { return buffer; }
	inline T* end() { return buffer + N; }

private:
	// signal queue
	alignas(32) T buffer[N] = {};
};
