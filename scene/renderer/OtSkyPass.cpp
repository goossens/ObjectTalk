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
//	OtSceneRenderer::renderSkyPass
//

void OtSceneRenderer::renderSkyPass(OtScene* scene) {
	// get the camera's view matrix and decompose it
	glm::vec3 scale;
	glm::quat rotate;
	glm::vec3 translate;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(viewMatrix, scale, rotate, translate, skew, perspective);

	// create a new matrix that only honors the rotation
	glm::mat4 newViewMatrix = glm::toMat4(rotate);

	// setup pass
	OtPass pass;
	pass.setClear(false, false);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(compositeBuffer);
	pass.setTransform(newViewMatrix, projectionMatrix);

	// see if we have any sky boxes
	for (auto [entity, component] : scene->view<OtSkyBoxComponent>().each()) {
		if (component.cubemap.isReady()) {
			renderSkyBox(pass, component);
		}
	};

	// see if we have any sky spheres
	for (auto [entity, component] : scene->view<OtSkySphereComponent>().each()) {
		if (component.texture.isReady()) {
			renderSkySphere(pass, component);
		}
	};
}


//
//	OtSceneRenderer::renderSkyBox
//

void OtSceneRenderer::renderSkyBox(OtPass& pass, OtSkyBoxComponent& component) {
	// setup the mesh
	if (!unityBoxGeometry) {
		unityBoxGeometry = OtBoxGeometry::create();
	}

	unityBoxGeometry->submitTriangles();

	// set the uniform values
	glm::vec4* uniforms = backgroundUniforms.getValues();
	uniforms[0] = glm::vec4(component.brightness, component.gamma, 0.0f, 0.0f);
	backgroundUniforms.submit();

	// submit texture via sampler
	skyBoxSampler.submit(0, component.cubemap->getCubeMap());

	// run the shader
	skyBoxShader.setState(OtStateWriteRgb | OtStateWriteA);
	pass.runShader(skyBoxShader);
}


//
//	OtSceneRenderer::renderSkySphere
//

void OtSceneRenderer::renderSkySphere(OtPass& pass, OtSkySphereComponent& component) {
	// setup the mesh
	if (!unitySphereGeometry) {
		unitySphereGeometry = OtSphereGeometry::create();
	}

	unitySphereGeometry->submitTriangles();

	// set the uniform values
	glm::vec4* uniforms = backgroundUniforms.getValues();
	uniforms[0] = glm::vec4(component.brightness, component.gamma, 0.0f, 0.0f);
	backgroundUniforms.submit();

	// submit texture via sampler
	skySphereSampler.submit(0, component.texture->getTexture());

	// run the shader
	skySphereShader.setState(OtStateWriteRgb | OtStateWriteA);
	pass.runShader(skySphereShader);
}
