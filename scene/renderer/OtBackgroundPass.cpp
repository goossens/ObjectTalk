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

void OtSceneRenderer::renderBackgroundPass(OtScene2 scene) {
	// we can only have one background component
	bool backgroundComponent = false;

	// determine background color
	glm::vec3 backgroundColor{0.0f};

	scene->view<OtBackgroundComponent>().each([&](auto& component) {
		if (!backgroundComponent) {
			backgroundColor = component.color;
			backgroundComponent = true;
		}
	});

	glm::u8vec3 result = glm::u8vec3(glm::round(backgroundColor * 255.0f));
	uint32_t clearColor = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | 255;

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
	backgroundPass.reserveRenderingSlot();
	backgroundPass.setClear(true, false, clearColor);
	backgroundPass.setRectangle(0, 0, width, height);
	backgroundPass.setFrameBuffer(composite);
	backgroundPass.setTransform(newViewMatrix, camera->getProjectionMatrix());

	// copy depth buffer from geometry gbuffer to the composite framebuffer
	backgroundPass.blit(composite.getDepthTexture(), 0, 0, gbuffer.getDepthTexture());

	// see if we have any sky boxes
	scene->view<OtSkyBoxComponent>().each([&](auto& component) {
		if (!backgroundComponent) {
			if (component.isValid()) {
				renderSkyBox(component);
				backgroundComponent = true;
			}
		}
	});

	// see if we have any sky spheres
	scene->view<OtSkySphereComponent>().each([&](auto& component) {
		if (!backgroundComponent) {
			if (component.isValid()) {
				renderSkySphere(component);
				backgroundComponent = true;
			}
		}
	});
}


//
//	OtSceneRenderer::renderSkyBox
//

void OtSceneRenderer::renderSkyBox(OtSkyBoxComponent& component) {
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
	skyMapSampler.submit(0, component.cubemap, "s_cubemap");

	// load the shader (if required)
	if (!skyBoxShader.isValid()) {
		skyBoxShader.initialize("OtSkyboxVS", "OtSkyboxFS");
	}

	// run the shader
	skyBoxShader.setState(OtStateWriteRgb | OtStateWriteA);
	backgroundPass.runShader(skyBoxShader);
}


//
//	OtSceneRenderer::renderSkySphere
//

void OtSceneRenderer::renderSkySphere(OtSkySphereComponent& component) {
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
	skySphereSampler.submit(0, component.texture, "s_skySphereTexture");

	// load the shader (if required)
	if (!skySphereShader.isValid()) {
		skySphereShader.initialize("OtSkySphereVS", "OtSkySphereFS");
	}

	// run the shader
	skySphereShader.setState(OtStateWriteRgb | OtStateWriteA);
	backgroundPass.runShader(skySphereShader);
}
