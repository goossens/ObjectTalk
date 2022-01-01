//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstdlib>
#include <ctime>

#include "ot/numbers.h"


//
//	OfRandom
//

double OtRandom(double num1, double num2) {
	static bool initialized = false;

	if (!initialized) {
		srand(time(0));
		initialized = true;
	}

	double high = std::max(num1, num2);
	double low = std::min(num1, num2);
	return std::max(low, (low + ((high - low) * rand() / float(RAND_MAX))) * (1.0f - std::numeric_limits<float>::epsilon()));
}
