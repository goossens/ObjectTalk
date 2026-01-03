//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	inline T lerp(T a, T b, T t) {
		return a + t * (b - a);
	}
}
