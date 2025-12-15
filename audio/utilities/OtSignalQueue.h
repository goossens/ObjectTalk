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
#include <array>

#include "OtAssert.h"


//
//	OtSignalQueue
//

template<typename T, size_t N>
class OtSignalQueue {
public:
	// clear the queue with specified value
	inline void clear(T sample) {
		std::fill(queue.begin(), queue.end(), sample);
	}

	// insert samples
	inline void insert(T* samples, size_t count) {
		OtAssert(count < N);
		std::copy(queue.begin() + count, queue.end(), queue.begin());
		std::copy(samples, samples + count, queue.end() - count);
	}

	// access members
	inline T operator[](size_t i) const { return queue[i]; }
	inline T& operator[](size_t i) { return queue[i]; }

	// queue information
	inline T* data() { return queue.data(); }
	inline size_t size() { return N; }

	// interator support
	inline typename std::array<T, N>::iterator begin() { return queue.begin(); }
	inline typename std::array<T, N>::iterator end() { return queue.end(); }

private:
	// signal queue
	std::array<T, N> queue = {};
};
