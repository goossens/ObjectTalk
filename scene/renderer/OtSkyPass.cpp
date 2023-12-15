//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include <glm/gtx/quaternion.hpp>
#include "glm/gtx/matrix_decompose.hpp"
#include "imgui.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderSkyPass
//

void OtSceneRenderer::renderSkyPass(OtSceneRendererContext& ctx) {
	// get the camera's view matrix and decompose it
	glm::vec3 scale;
	glm::quat rotate;
	glm::vec3 translate;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(ctx.viewMatrix, scale, rotate, translate, skew, perspective);

	// create a new matrix that only honors the rotation
	glm::mat4 newViewMatrix = glm::toMat4(rotate);

	// setup pass
	OtPass pass;
	pass.setRectangle(0, 0, ctx.width, ctx.height);
	pass.setFrameBuffer(ctx.compositeBuffer);
	pass.setTransform(newViewMatrix, ctx.projectionMatrix);

	// see if we have any sky components
	for (auto&& [entity, component] : ctx.scene->view<OtSkyComponent>().each()) {
		renderSky(ctx, pass, component);
	};

	// see if we have any sky boxes
	for (auto&& [entity, component] : ctx.scene->view<OtSkyBoxComponent>().each()) {
		if (component.cubemap.isReady()) {
			renderSkyBox(ctx, pass, component);
		}
	};

	// see if we have any sky spheres
	for (auto&& [entity, component] : ctx.scene->view<OtSkySphereComponent>().each()) {
		if (component.texture.isReady()) {
			renderSkySphere(ctx, pass, component);
		}
	};
}


//
//	OtSceneRenderer::renderSky
//

void OtSceneRenderer::renderSky(OtSceneRendererContext& ctx, OtPass& pass, OtSkyComponent& component) {
	// setup the mesh
	if (!unitySphereGeometry) {
		unitySphereGeometry = OtSphereGeometry::create();
	}

	unitySphereGeometry->submitTriangles();

	// set the uniform values
	static float time = 0.0f;
	time += ImGui::GetIO().DeltaTime;

	skyUniforms.setValue(0, glm::vec4(
		time * component.speed / 10.0f,
		component.cirrus,
		component.cumulus,
		0.0f));

	skyUniforms.setValue(1, glm::vec4(
		component.rayleighCoefficient / 1000.0f,
		component.mieCoefficient / 1000.0f,
		component.mieScattering,
		0.0f));

	skyUniforms.setValue(2, glm::vec4(component.getDirectionToSun(), 0.0f));
	skyUniforms.submit();

	// run the program
	skyProgram.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLessEqual);

	pass.runShaderProgram(skyProgram);

}


//
//	OtSceneRenderer::renderSkyBox
//

void OtSceneRenderer::renderSkyBox(OtSceneRendererContext& ctx, OtPass& pass, OtSkyBoxComponent& component) {
	// setup the mesh
	if (!unityBoxGeometry) {
		unityBoxGeometry = OtBoxGeometry::create();
	}

	unityBoxGeometry->submitTriangles();

	// set the uniform values
	skyUniforms.setValue(0, glm::vec4(component.brightness, component.gamma, 0.0f, 0.0f));
	skyUniforms.submit();

	// submit texture via sampler
	skySampler.submit(0, component.cubemap->getCubeMap());

	// run the program
	skyBoxProgram.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLessEqual);

	pass.runShaderProgram(skyBoxProgram);
}


//
//	OtSceneRenderer::renderSkySphere
//

void OtSceneRenderer::renderSkySphere(OtSceneRendererContext& ctx, OtPass& pass, OtSkySphereComponent& component) {
	// setup the mesh
	if (!unitySphereGeometry) {
		unitySphereGeometry = OtSphereGeometry::create();
	}

	unitySphereGeometry->submitTriangles();

	// set the uniform values
	skyUniforms.setValue(0, glm::vec4(component.brightness, component.gamma, 0.0f, 0.0f));
	skyUniforms.submit();

	// submit texture via sampler
	skySampler.submit(0, component.texture->getTexture());

	// run the program
	skySphereProgram.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLessEqual);

	pass.runShaderProgram(skySphereProgram);
}
