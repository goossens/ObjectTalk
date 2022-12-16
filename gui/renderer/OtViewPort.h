//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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

inline glm::mat4 OtViewPortGetMatrix() {
	// calculate viewport projection matrix based on platform
	float sy = OtGpuHasOriginBottomLeft() ? 0.5f : -0.5f;
	float sz = OtGpuHasHomogeneousDepth() ? 0.5f :  1.0f;
	float tz = OtGpuHasHomogeneousDepth() ? 0.5f :  0.0f;

	glm::mat4 matrix = glm::mat4(1.0);
	matrix = glm::translate(matrix, glm::vec3(0.5, 0.5, tz));
	matrix = glm::scale(matrix, glm::vec3(0.5, sy, sz));
	return matrix;
}
