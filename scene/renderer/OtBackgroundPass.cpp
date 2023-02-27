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
//	OtSceneRenderer::renderBackgroundPass
//

void OtSceneRenderer::renderBackgroundPass(std::shared_ptr<OtScene2> scene) {
	// update composite buffer
	compositeBuffer.update(width, height);

	// we can only have one background component
	bool backgroundComponent = false;

	// determine background color
	glm::vec3 backgroundColor{0.0f};

	for (auto [entity, component] : scene->view<OtBackgroundComponent>().each()) {
		if (!backgroundComponent) {
			backgroundColor = component.color;
			backgroundComponent = true;
		}
	}

	glm::u8vec3 result = glm::u8vec3(glm::round(backgroundColor * 255.0f));
	uint32_t clearColor = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | 255;

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
	pass.reserveRenderingSlot();
	pass.setClear(true, false, clearColor);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(compositeBuffer);
	pass.setTransform(newViewMatrix, projectionMatrix);

	// copy depth buffer from geometry gbuffer to the composite framebuffer
	pass.blit(compositeBuffer.getDepthTexture(), 0, 0, gbuffer.getDepthTexture());

	// see if we have any sky boxes
	for (auto [entity, component] : scene->view<OtSkyBoxComponent>().each()) {
		if (!backgroundComponent) {
			if (component.isValid()) {
				renderSkyBox(pass, component);
				backgroundComponent = true;
			}
		}
	};

	// see if we have any sky spheres
	for (auto [entity, component] : scene->view<OtSkySphereComponent>().each()) {
		if (!backgroundComponent) {
			if (component.isValid()) {
				renderSkySphere(pass, component);
				backgroundComponent = true;
			}
		}
	};
}


//
//	OtSceneRenderer::renderSkyBox
//

void OtSceneRenderer::renderSkyBox(OtPass& pass, OtSkyBoxComponent& component) {
	// setup the mesh
	if (!unityBoxGeometry) {
		unityBoxGeometry = OtBoxGeometryClass::create();
	}

	unityBoxGeometry->submitTriangles();

	// set the uniform values
	glm::vec4* uniforms = backgroundUniforms.getValues();
	uniforms[0] = glm::vec4(component.brightness, component.gamma, 0.0f, 0.0f);
	backgroundUniforms.submit();

	// submit texture via sampler
	skyBoxSampler.submit(0, component.cubemap);

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
		unitySphereGeometry = OtSphereGeometryClass::create();
	}

	unitySphereGeometry->submitTriangles();

	// set the uniform values
	glm::vec4* uniforms = backgroundUniforms.getValues();
	uniforms[0] = glm::vec4(component.brightness, component.gamma, 0.0f, 0.0f);
	backgroundUniforms.submit();

	// submit texture via sampler
	skySphereSampler.submit(0, component.texture);

	// run the shader
	skySphereShader.setState(OtStateWriteRgb | OtStateWriteA);
	pass.runShader(skySphereShader);
}