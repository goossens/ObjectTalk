//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <chrono>
#include <string>
#include <string_view>
#include <utility>

#include "OtLog.h"


//
//	Measure time to execute function in milliseconds
//

template<typename F, typename... Args>
inline float OtMeasureFunction(F func, Args&&... args) {
	auto start = std::chrono::high_resolution_clock::now();
	std::forward<decltype(func)>(func)(std::forward<decltype(args)>(args)...);
	auto stop = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
}


//
//	OtMeasureStopWatch
//

class OtMeasureStopWatch {
public:
	// constructor
	inline OtMeasureStopWatch() {
		reset();
	}

	// reset the stopwatch
	inline void reset() {
		start = std::chrono::high_resolution_clock::now();
	}

	// return elapsed time in milliseconds
	inline float elapsed() {
		auto now = std::chrono::high_resolution_clock::now();
		auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
		return (float) microseconds / 1000.0f;
	}

private:
	// start of measurement
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
};


//
//	OtMeasureScope
//

class OtMeasureScope {
public:
	// constructor
	OtMeasureScope(const std::string_view& n) : name(n) {}

	// destructor
	~OtMeasureScope() {
		OtLogDebug("{}: {} ms", name, timer.elapsed());
	}

private:
	// properties
	std::string name;
	OtMeasureStopWatch timer;
};
