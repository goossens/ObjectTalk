//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFbm.h"


//
//	OtFbm::preparePass
//

OtComputeProgram& OtFbm::preparePass() {
	uniform.setValue(0, static_cast<float>(frequency), lacunarity, amplitude, persistence);
	uniform.setValue(1, static_cast<float>(octaves), static_cast<float>(noiseType), 0.0f, 0.0f);
	uniform.submit();

	return program;
}
