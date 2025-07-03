//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"
#include "OtInstanceDataBuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtVertex.h"

#include "OtSceneRenderPass.h"


//
//	OtParticlesPass
//

class OtParticlesPass : public OtSceneRenderPass {
public:
	// constructor
	OtParticlesPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	inline void render(OtSceneRendererContext& ctx) {
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
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(framebuffer);
		pass.setTransform(view, ctx.camera.projectionMatrix);

		for (auto&& [entity, component] : ctx.scene->view<OtParticlesComponent>().each()) {
			// update the particle system
			auto& settings = component.settings;
			auto& particles = *component.particles;

			settings.cameraPosition = ctx.camera.position;
			settings.deltatime = ImGui::GetIO().DeltaTime;
			particles.update(settings);

			// send out geometry
			static glm::vec3 vertices[] = {
				glm::vec3{-0.5f, -0.5f, 0.0f},
				glm::vec3{0.5f, -0.5f, 0.0f},
				glm::vec3{0.5f, 0.5f, 0.0f},
				glm::vec3{-0.5f, 0.5f, 0.0f}
			};

			OtTransientVertexBuffer tvb;
			tvb.submit(vertices, 4, OtVertexPos::getLayout());

			static uint32_t indices[] = {0, 1, 2, 2, 3, 0};
			OtTransientIndexBuffer tib;
			tib.submit(indices, 6);

			// send out instance data
			OtInstanceDataBuffer idb;
			idb.submit(particles.getInstanceData(), particles.size(), particles.getInstanceStride());

			// bind the particle texture atlas
			submitTextureSampler(particlesSampler, 0, settings.atlas);

			// set the model matrix
			program.setTransform(ctx.scene->getGlobalTransform(entity));

			// run the program
			program.setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateBlendAlpha);

			pass.runShaderProgram(program);
		}
	}

private:
	// properties
	OtFrameBuffer& framebuffer;
	OtShaderProgram program{"OtParticlesVS", "OtParticlesFS"};
	OtSampler particlesSampler{"s_particlesTexture"};
};
