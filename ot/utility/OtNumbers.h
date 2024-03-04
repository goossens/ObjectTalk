//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

	inline constexpr double pi2 = pi * 2.0;
}

namespace std {
	template <typename T>
	T lerp(T a, T b, T t) {
		return a + t * (b - a);
	}
}

//	Non-cryptographic random numbers
double OtRandom(double num1, double num2);
inline double OtRandom(double num) { return OtRandom(0.0, num); }
inline double OtRandom() { return OtRandom(0.0, 1.0); }

inline float OtRandom(float num1, float num2) { return float(OtRandom(double(num1), double(num2))); }
inline float OtRandom(float num) { return float(OtRandom(0.0, double(num))); }

inline int OtRandom(int num1, int num2) { return int(OtRandom(double(num1), double(num2))); }
inline int OtRandom(int num) { return int(OtRandom(0.0, double(num))); }
