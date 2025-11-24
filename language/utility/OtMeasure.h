//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
		startTime = std::chrono::high_resolution_clock::now();
		lapTime = startTime;
	}

	// return elapsed time in milliseconds since start of stopwatch
	inline float elapsed() {
		auto now = std::chrono::high_resolution_clock::now();
		auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime).count();
		return static_cast<float>(microseconds) / 1000.0f;
	}

	// return elapsed time in milliseconds since last "lap" call (or the start if this is the first call)
	inline float lap() {
		auto now = std::chrono::high_resolution_clock::now();
		auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - lapTime).count();
		lapTime = now;
		return static_cast<float>(microseconds) / 1000.0f;
	}

private:
	// start of measurement
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lapTime;
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
