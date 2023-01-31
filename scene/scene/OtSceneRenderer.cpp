//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <glm/gtx/quaternion.hpp>
#include "glm/gtx/matrix_decompose.hpp"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::render
//

int OtSceneRenderer::render(OtScene2 scene, OtCamera2 c, int w, int h) {
	// save information
	camera = c;
	width = w;
	height = h;

	// run composite pass
	composite.update(width, height);
	renderEnvironmentPass(scene);
	renderCompositePass(scene);
	return composite.getColorTextureIndex();
}


//
//	OtSceneRenderer::renderEnvironmentPass
//

void OtSceneRenderer::renderEnvironmentPass(OtScene2 scene) {
	// get the camera's view matrix and decompose it
	auto viewMatrix = camera->getViewMatrix();
	glm::vec3 scale;
	glm::quat rotate;
	glm::vec3 translate;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(viewMatrix, scale, rotate, translate, skew, perspective);

	// create a new matrix that only honors the rotation
	glm::mat4 newViewMatrix = glm::toMat4(rotate);

	// setup pass
	environmentPass.reserveRenderingSlot();
	environmentPass.setClear(true, false);
	environmentPass.setRectangle(0, 0, width, height);
	environmentPass.setFrameBuffer(composite);
	environmentPass.setTransform(newViewMatrix, camera->getProjectionMatrix());

	// we can only have one environment component
	bool environmentComponent = false;
	bool tooManyEnvironmentComponents = false;

	// see if we have any sky boxes
	scene->view<OtSkyBoxComponent>().each([&](auto& component) {
		if (!environmentComponent) {
			component.render(environmentPass);
			environmentComponent = true;

		} else {
			tooManyEnvironmentComponents = true;
		}
	});

	// see if we have any sky spheres
	scene->view<OtSkySphereComponent>().each([&](auto& component) {
		if (!environmentComponent) {
			component.render(environmentPass);
			environmentComponent = true;

		} else {
			tooManyEnvironmentComponents = true;
		}
	});

}




//
//	OtSceneRenderer::renderCompositePass
//

void OtSceneRenderer::renderCompositePass(OtScene2 scene) {
}
