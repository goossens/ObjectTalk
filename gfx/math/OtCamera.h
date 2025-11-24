//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "OtAABB.h"
#include "OtFrustum.h"


//
//	OtCamera Class
//

class OtCamera {
public:
	// constructors
	OtCamera() = default;

	inline OtCamera(int w, int h, const glm::mat4 pm, const glm::mat4& vm) :
		width(w),
		height(h),
		viewMatrix(vm),
		projectionMatrix(pm) {

		// update camera
		update();
	}

	inline OtCamera(int w, int h, float nearPlane, float farPlane, float fov, const glm::mat4& vm) :
		width(w),
		height(h),
		viewMatrix(vm) {

		// calculate projection matrix
		projectionMatrix = glm::perspectiveFovRH_ZO(glm::radians(fov), static_cast<float>(width), static_cast<float>(height), nearPlane, farPlane);

		// update camera
		update();
	}

	inline OtCamera(int w, int h, float nearPlane, float farPlane, float fov, const glm::vec3& eye, const glm::vec3& at) :
		width(w),
		height(h) {

		// determine view and projection matrices
		viewMatrix = glm::lookAt(eye, at, glm::vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix = glm::perspectiveFovRH_ZO(glm::radians(fov), static_cast<float>(width), static_cast<float>(height), nearPlane, farPlane);

		// initialize camera
		update();
	}

	// get the near and far value from the projection matrix
	inline void getNearFar(float& nearPlane, float& farPlane) {
		nearPlane = projectionMatrix[3][2] / projectionMatrix[2][2];
		farPlane = (projectionMatrix[2][2] * nearPlane) / (projectionMatrix[2][2] + 1.0f);
	}

	// update camera
	inline void update() {
		// determine camera position
		position = -glm::vec3(viewMatrix[3]);

		// determine view/projection matrix
		viewProjectionMatrix = projectionMatrix * viewMatrix;

		// determine the camera's frustum in world space
		frustum = OtFrustum(viewProjectionMatrix);
	}

	// see if an AABB box is visible
	bool isVisibleAABB(const OtAABB& aabb) { return frustum.isVisibleAABB(aabb); }

	// properties
	int width = 0;
	int height = 0;
	glm::vec3 position{0.0f};
	glm::mat4 viewMatrix{0.0f};
	glm::mat4 projectionMatrix{0.0f};
	glm::mat4 viewProjectionMatrix{0.0f};
	OtFrustum frustum;
};
