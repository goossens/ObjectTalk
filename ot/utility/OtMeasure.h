//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <chrono>
#include <utility>


//
//	Measure time to execute function
//

// auto OtMeasureFunction = [](auto&& func, auto&&... params) {

template<typename F, typename... Args>
double OtMeasureFunction(F func, Args&&... args) {
	const auto& start = std::chrono::high_resolution_clock::now();
	std::forward<decltype(func)>(func)(std::forward<decltype(args)>(args)...);
	const auto& stop = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
}
