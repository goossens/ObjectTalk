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

#include "OtFrustum.h"
#include "OtGpu.h"


//
//	OtCamera Class
//

class OtCamera {
public:
	// constructor
	OtCamera(int w, int h, float n, float f, float fv, const glm::vec3& cp, const glm::mat4 vm) :
		width(w),
		height(h),
		nearPlane(n),
		farPlane(f),
		fov(fv),
		cameraPosition(cp),
		viewMatrix(vm) {

		// calculate projection matrix
		float aspectRatio = (float) width / (float) height;

		projectionMatrix = OtGpuHasHomogeneousDepth()
			? glm::perspectiveRH_NO(glm::radians(fov), aspectRatio, nearPlane, farPlane)
			: glm::perspectiveRH_ZO(glm::radians(fov), aspectRatio, nearPlane, farPlane);

		// determine view/projection matrix
		viewProjectionMatrix = projectionMatrix * viewMatrix;

		// determine the camera's frustum in worldspace
		frustum = OtFrustum(viewProjectionMatrix);
	}

	// properties
	int width;
	int height;
	float nearPlane;
	float farPlane;
	float fov;
	glm::vec3 cameraPosition;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 viewProjectionMatrix;
	OtFrustum frustum;
};
