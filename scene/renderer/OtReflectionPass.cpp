//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtc/type_ptr.hpp"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderReflectionPass
//

void OtSceneRenderer::renderReflectionPass(OtScene* scene) {
	/*
	// get the water component
	auto water = scene->getComponent<OtWaterComponent>(waterEntity);

	// save rendering settings
	size_t savedWidth = width;
	size_t savedHeight = height;

	glm::vec3 savedCameraPosition = cameraPosition;
	glm::mat4 savedViewMatrix = viewMatrix;
	glm::mat4 savedProjectionMatrix = projectionMatrix;
	glm::mat4 savedViewProjectionMatrix = viewProjectionMatrix;

	// setup the renderer for the reflection
	width /= 2;
	height /= 2;

	// determine position of reflection camera
	cameraPosition.y = water.height - (cameraPosition.y - water.height);

	// determine new view matrix
	// see http://khayyam.kaplinski.com/2011/09/reflective-water-with-glsl-part-i.html
	// and http://bcnine.com/articles/water/water.md.html
	static const float flip[16] = { // these must be in column-wise order for glm library
		1.0f,  0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.0f, 0.0f, 1.0f
	};

	static float reflection[16] = { // these must be in column-wise order for glm library
		1.0f,  0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.0f, 0.0f, 1.0f
	};

	reflection[13] = 2.0f * water.height;

	glm::mat4 mSceneCamera = glm::inverse(viewMatrix);
	glm::mat4 mReflectionCamera = glm::make_mat4(reflection) * mSceneCamera * glm::make_mat4(flip);
	viewMatrix = glm::inverse(mReflectionCamera);

	// restore rendering settings
	width = savedWidth;
	height = savedHeight;
	cameraPosition = savedCameraPosition;
	viewMatrix = savedViewMatrix;
	projectionMatrix = savedProjectionMatrix;
	viewProjectionMatrix = savedViewProjectionMatrix;
	*/

}

