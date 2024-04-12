//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "OtFrustum.h"
#include "OtGpu.h"


//
//	OtCamera Class
//

class OtCamera {
public:
	// constructors
	OtCamera() = default;

	OtCamera(int w, int h, const glm::vec3& cp, const glm::mat4 pm, const glm::mat4 vm) :
		width(w),
		height(h),
		cameraPosition(cp),
		projectionMatrix(pm),
		viewMatrix(vm) {

		// determine view/projection matrix
		viewProjectionMatrix = projectionMatrix * viewMatrix;

		// determine the camera's frustum in worldspace
		frustum = OtFrustum(viewProjectionMatrix);
	}

	OtCamera(int w, int h, float nearPlane, float farPlane, float fov, const glm::vec3& cp, const glm::mat4 vm) :
		width(w),
		height(h),
		cameraPosition(cp),
		viewMatrix(vm) {

		// calculate projection matrix
		projectionMatrix = OtGpuHasHomogeneousDepth()
			? glm::perspectiveFovRH_NO(glm::radians(fov), (float) width, (float) height, nearPlane, farPlane)
			: glm::perspectiveFovRH_ZO(glm::radians(fov), (float) width, (float) height, nearPlane, farPlane);

		// determine view/projection matrix
		viewProjectionMatrix = projectionMatrix * viewMatrix;

		// determine the camera's frustum in worldspace
		frustum = OtFrustum(viewProjectionMatrix);
	}

	// get the near and far value fron the projection matrix
	void getNearFar(float& nearPlane, float& farPlane) {
		if (OtGpuHasHomogeneousDepth()) {
			nearPlane = (2.0f * projectionMatrix[3][2]) / (2.0f * projectionMatrix[2][2] - 2.0f);
			farPlane = ((projectionMatrix[2][2] - 1.0f) * nearPlane) / (projectionMatrix[2][2] + 1.0f);

		} else {
			nearPlane = projectionMatrix[3][2] / projectionMatrix[2][2];
			farPlane = (projectionMatrix[2][2] * nearPlane) / (projectionMatrix[2][2] + 1.0f);
		}
	}

	// properties
	int width;
	int height;
	glm::vec3 cameraPosition;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 viewProjectionMatrix;
	OtFrustum frustum;
};
