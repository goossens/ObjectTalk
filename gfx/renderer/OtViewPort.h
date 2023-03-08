//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "OtGpu.h"


//
//	OtViewPortGetMatrix
//

inline glm::mat4& OtViewPortGetMatrix() {
	// calculate viewport projection matrix based on platform
	static bool initialized = false;
	static glm::mat4 matrix;

	if (!initialized) {
		float tz = OtGpuHasHomogeneousDepth() ? 0.5 : 0.0;
		matrix = glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, tz));

		float sy = OtGpuHasOriginBottomLeft() ? 0.5 : -0.5;
		float sz = OtGpuHasHomogeneousDepth() ? 0.5 : 1.0;
		matrix = glm::scale(matrix, glm::vec3(0.5, sy, sz));

		initialized = true;
	}

	return matrix;
}
