//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "glm/gtx/quaternion.hpp"

#include "OtFrameBuffer.h"
#include "OtPass.h"
#include "OtShaderProgram.h"
#include "OtUniformMat4.h"

#include "OtSceneRenderPass.h"


//
//	OtSkyPass
//

class OtSkyPass : public OtSceneRenderPass {
public:
	// constructor
	OtSkyPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx) {
		// get the camera's view matrix and decompose it
		glm::vec3 scale;
		glm::quat rotate;
		glm::vec3 translate;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(ctx.camera.viewMatrix, scale, rotate, translate, skew, perspective);

		// submit inverse view projection matrix as a uniform (only preserving rotation)
		skyInvViewProjUniform.set(0, glm::inverse(ctx.camera.projectionMatrix * glm::toMat4(rotate)));
		skyInvViewProjUniform.submit();

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

	// render procedural sky
	void renderSky(OtSceneRendererContext& ctx, OtPass& pass, OtSkyComponent& component) {
		// set the uniform values
		static float time = 0.0f;
		time += ImGui::GetIO().DeltaTime;

		skyUniforms.setValue(
			0,
			time * component.speed / 10.0f,
			component.cirrus,
			component.cumulus,
			0.0f);

		skyUniforms.setValue(
			1,
			component.rayleighCoefficient / 1000.0f,
			component.mieCoefficient / 1000.0f,
			component.mieScattering,
			0.0f);

		skyUniforms.setValue(2, component.getDirectionToSun(), 0.0f);
		skyUniforms.submit();

		// run the program
		skyProgram.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLessEqual);

		pass.runShaderProgram(skyProgram);
	}

	// render skybox
	void renderSkyBox(OtSceneRendererContext& ctx, OtPass& pass, OtSkyBoxComponent& component) {
		// set the uniform values
		skyUniforms.setValue(0, component.brightness, component.gamma, 0.0f, 0.0f);
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

private:
	// properties
	OtFrameBuffer& framebuffer;

	OtUniformMat4 skyInvViewProjUniform{"u_skyInvViewProjUniform", 1};
	OtUniformVec4 skyUniforms{"u_sky", 3};

	OtSampler skySampler{"s_skyTexture"};

	OtShaderProgram skyProgram{"OtSkyVS", "OtSkyFS"};
	OtShaderProgram skyBoxProgram{"OtSkyVS", "OtSkyBoxFS"};
};
