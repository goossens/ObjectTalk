//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtRenderPass.h"
#include "OtVertex.h"

#include "OtParticlesPass.h"
#include "OtShaders.h"


//
//	OtParticlesPass::render
//

void OtParticlesPass::render(OtSceneRendererContext& ctx) {
	// initialize resources (if required)
	if (!resourcesInitialized) {
		initializeResources();
		resourcesInitialized = true;
	}

	// determine view matrix for billboards (by removing rotation)
	auto view = ctx.camera.viewMatrix;
	view[0][0] = 1.0f;
	view[1][0] = 0.0f;
	view[2][0] = 0.0f;
	view[0][1] = 0.0f;
	view[1][1] = 1.0f;
	view[2][1] = 0.0f;
	view[0][2] = 0.0f;
	view[1][2] = 0.0f;
	view[2][2] = 1.0f;

	// setup pass
	OtRenderPass pass;
	pass.start(framebuffer);
	pass.bindPipeline(pipeline);
	ctx.pass = &pass;

	for (auto&& [entity, component] : ctx.scene->view<OtParticlesComponent>().each()) {
		// update the particle system
		auto& settings = component.settings;
		auto& particles = *component.particles;

		settings.cameraPosition = ctx.camera.position;
		settings.deltatime = ImGui::GetIO().DeltaTime;
		particles.update(settings);

		// send out particle (instance) data
		particleBuffer.set(particles.data(), particles.size(), OtVertexParticle::getDescription());
		pass.setInstanceData(particleBuffer);

		// bind the particle texture atlas
		ctx.bindFragmentSampler(0, ctx.particlesSampler, settings.atlas);

		// set vertex uniforms
		struct Uniforms {
			glm::mat4 modelViewProjectionMatrix;
		} uniforms{
			ctx.camera.projectionMatrix * view * ctx.scene->getGlobalTransform(entity)
		};

		pass.bindVertexUniforms(0, &uniforms, sizeof(uniforms));

		// render particles
		pass.render(vertexBuffer, indexBuffer);
	}

	pass.end();
}


//
//	OtParticlesPass::initializeResources
//

void OtParticlesPass::initializeResources() {
	pipeline.setShaders(OtParticlesVert, OtParticlesVertSize, OtParticlesFrag, OtParticlesFragSize);
	pipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	pipeline.setVertexDescription(OtVertexPos::getDescription());
	pipeline.setInstanceDescription(OtVertexParticle::getDescription());
	pipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	pipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);

	static glm::vec3 vertices[] = {
		glm::vec3{-0.5f, -0.5f, 0.0f},
		glm::vec3{0.5f, -0.5f, 0.0f},
		glm::vec3{0.5f, 0.5f, 0.0f},
		glm::vec3{-0.5f, 0.5f, 0.0f}
	};

	static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

	vertexBuffer.set(vertices, sizeof(vertices) / sizeof(*vertices), OtVertexPos::getDescription());
	indexBuffer.set(indices, sizeof(indices) / sizeof(*indices));
}
