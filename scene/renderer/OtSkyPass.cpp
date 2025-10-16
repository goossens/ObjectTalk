//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui.h"

#include "OtPass.h"

#include "OtSkyPass.h"


//
//	OtSkyPass::render
//

void OtSkyPass::render(OtSceneRendererContext& ctx) {
	// get the camera's view matrix and decompose it
	glm::vec3 scale;
	glm::quat rotate;
	glm::vec3 translate;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(ctx.camera.viewMatrix, scale, rotate, translate, skew, perspective);

	// submit inverse view projection matrix as a uniform (only preserving rotation)
	ctx.invViewProjUniform.set(0, glm::inverse(ctx.camera.projectionMatrix * glm::toMat4(rotate)));
	ctx.invViewProjUniform.submit();

	// setup pass
	OtPass pass;
	pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
	pass.setFrameBuffer(framebuffer);
	pass.submitQuad(ctx.camera.width, ctx.camera.height);

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
}


//
//	OtSkyPass::renderSky
//

void OtSkyPass::renderSky(OtSceneRendererContext& ctx, OtPass& pass, OtSkyComponent& sky) {
	// set the uniform values
	static float time = 0.0f;
	time += ImGui::GetIO().DeltaTime;

	ctx.skyUniforms.setValue(
		0,
		time * sky.speed / 10.0f,
		sky.cirrus,
		sky.cumulus,
		0.0f);

	ctx.skyUniforms.setValue(
		1,
		sky.rayleighCoefficient / 1000.0f,
		sky.mieCoefficient / 1000.0f,
		sky.mieScattering,
		0.0f);

	ctx.skyUniforms.setValue(2, sky.getDirectionToSun(), 0.0f);
	ctx.skyUniforms.submit();

	// run the program
	pass.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLessEqual);

	pass.runShaderProgram(skyProgram);
}


//
//	OtSkyPass::renderSkyBox
//

void OtSkyPass::renderSkyBox(OtSceneRendererContext& ctx, OtPass& pass, OtSkyBoxComponent& skybox) {
	// set the uniform values
	ctx.skyUniforms.setValue(0, skybox.brightness, skybox.gamma, 0.0f, 0.0f);
	ctx.skyUniforms.submit();

	// submit texture via sampler
	ctx.skySampler.submit(0, skybox.cubemap->getCubeMap());

	// run the program
	pass.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLessEqual);

	pass.runShaderProgram(skyBoxProgram);
}
