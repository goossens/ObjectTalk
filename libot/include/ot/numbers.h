//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
// This can all go away when we go to C++20
//

namespace std::numbers {
	inline constexpr double pi = 3.141592653589793238462643383279502884;
	inline constexpr double e = 2.718281828459045235360287471352662497;
}


//	Non-cryptographic random numbers
double OtRandom(double num1, double num2);
inline double OtRandom(double num) { return OtRandom(0.0, num); }

//	Clamp a number between two limits
inline double OtClamp(double value, double min, double max) {
	return value < min ? min : value > max ? max : value;
}
