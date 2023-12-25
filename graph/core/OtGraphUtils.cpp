//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <random>

#include "OtGraphUtils.h"

//
//	OtGraphGenerateID
//

uint32_t OtGraphGenerateID() {
	// get a new random ID
	static std::random_device device;
	static std::mt19937_64 engine(device());
	static std::uniform_int_distribution<uint32_t> distribution;
	return distribution(engine);
}
