//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://gist.github.com/agrafix/aa49c17cd32c8ba63b6a7cb8dce8b0bd


#pragma once


//
//	Include files
//

#include <cmath>


//
//	Support functions
//

inline float OtLinearToDb(float x) {
	return 20.0f * std::log10(x);
}

inline float OtDbToLinear(float x) {
	return std::pow(10.0f, x / 20.0f);
}
