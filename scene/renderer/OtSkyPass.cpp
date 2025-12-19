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

#include "OtRenderPass.h"

#include "OtSkyPass.h"
#include "OtShaders.h"


//
//	OtSkyPass::render
//

void OtSkyPass::render(OtSceneRendererContext& ctx) {
	// initialize resources (if required)
	if (!resourcesInitialized) {
		initializeResources();
		resourcesInitialized = true;
	}

	OtRenderPass pass;
	pass.start(framebuffer);
	ctx.pass = &pass;

	// get the camera's view matrix and decompose it
	glm::vec3 scale;
	glm::quat rotate;
	glm::vec3 translate;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(ctx.camera.viewMatrix, scale, rotate, translate, skew, perspective);

	// set vertex uniforms
	struct Uniforms {
		glm::mat4 invViewProj;
	} uniforms {
		glm::inverse(ctx.camera.projectionMatrix * glm::toMat4(rotate))
	};

	pass.bindVertexUniforms(0, &uniforms, sizeof(uniforms));

	// see if we have any sky components
	for (auto&& [entity, component] : ctx.scene->view<OtSkyComponent>().each()) {
		renderSky(ctx, component);
	};

	// see if we have any sky boxes
	for (auto&& [entity, component] : ctx.scene->view<OtSkyBoxComponent>().each()) {
		if (component.cubemap.isReady()) {
			renderSkyBox(ctx, component);
		}
	};

	pass.end();
}


//
//	OtSkyPass::renderSky
//

void OtSkyPass::renderSky(OtSceneRendererContext& ctx, OtSkyComponent& sky) {
	// bind pipeline
	ctx.pass->bindPipeline(skyPipeline);

	// set the uniform values
	static float time = 0.0f;
	time += ImGui::GetIO().DeltaTime;

	// set fragment uniforms
	struct Uniforms {
		glm::vec3 sunPosition;
		float time;
		float cirrus;
		float cumulus;
		float br;
		float bm;
		float g;
	} uniforms {
		sky.getDirectionToSun(),
		time * sky.speed / 10.0f,
		sky.cirrus,
		sky.cumulus,
		sky.rayleighCoefficient / 1000.0f,
		sky.mieCoefficient / 1000.0f,
		sky.mieScattering
	};

	ctx.pass->bindFragmentUniforms(0, &uniforms, sizeof(uniforms));

	// render sky
	ctx.pass->render(3);
}


//
//	OtSkyPass::renderSkyBox
//

void OtSkyPass::renderSkyBox(OtSceneRendererContext& ctx, OtSkyBoxComponent& skybox) {
	// bind pipeline
	ctx.pass->bindPipeline(skyBoxPipeline);

	// set fragment uniforms
	struct Uniforms {
		float brightness;
		float gamma;
	} uniforms {
		skybox.brightness,
		skybox.gamma
	};

	ctx.pass->bindFragmentUniforms(0, &uniforms, sizeof(uniforms));

	// submit texture via sampler
	ctx.pass->bindFragmentSampler(0, ctx.cubemapSampler, skybox.cubemap->getCubeMap());

	// render sky
	ctx.pass->render(3);
}


//
//	OtSkyPass::initializeResources
//

void OtSkyPass::initializeResources() {
	skyPipeline.setShaders(OtSkyVert, OtSkyVertSize, OtSkyFrag, OtSkyFragSize);
	skyPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	skyPipeline.setDepthTest(OtRenderPipeline::CompareOperation::lessEqual);

	skyBoxPipeline.setShaders(OtSkyVert, OtSkyVertSize, OtSkyBoxFrag, OtSkyBoxFragSize);
	skyBoxPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	skyBoxPipeline.setDepthTest(OtRenderPipeline::CompareOperation::lessEqual);
}
