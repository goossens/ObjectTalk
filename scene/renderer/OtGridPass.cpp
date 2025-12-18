//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "glm/glm.hpp"

#include "OtRenderPass.h"
#include "OtVertex.h"

#include "OtGridPass.h"

#include "OtGridVert.h"
#include "OtGridFrag.h"


//
//	OtGridPass::render
//

void OtGridPass::render(OtSceneRendererContext& ctx) {
	if (gridScale > 0.0f) {
		// initialize resources (if required)
		if (!resourcesInitialized) {
			initializeResources();
			resourcesInitialized = false;
		}

		// setup pass
		OtRenderPass pass;
		pass.start(framebuffer);
		pass.bindPipeline(pipeline);

		// set vertex uniforms
		struct VertexUniforms {
			glm::mat4 inverseViewProjectionMatrix;
		} vertexUniforms {
			glm::inverse(ctx.camera.viewProjectionMatrix)
		};

		ctx.pass->bindVertexUniforms(0, &vertexUniforms, sizeof(vertexUniforms));

		// set fragment uniforms
		struct FragmentUniforms {
			glm::mat4 viewProjectionMatrix;
			float gridScale;
		} fragmentUniforms {
			ctx.camera.viewProjectionMatrix,
			gridScale
		};

		pass.bindFragmentUniforms(0, &fragmentUniforms, sizeof(fragmentUniforms));

		pass.render(vertexBuffer, indexBuffer);
		pass.end();
	}
}


//
//	OtGridPass::initializeResources
//

void OtGridPass::initializeResources() {
	pipeline.setShaders(OtGridVert, sizeof(OtGridVert), OtGridFrag, sizeof(OtGridFrag));
	pipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	pipeline.setVertexDescription(OtVertexPos::getDescription());
	pipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	pipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);

	static glm::vec3 vertices[] = {
		glm::vec3{-1.0f, -1.0f, 0.0f},
		glm::vec3{1.0f, -1.0f, 0.0f},
		glm::vec3{1.0f, 1.0f, 0.0f},
		glm::vec3{-1.0f, 1.0f, 0.0f}
	};

	static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

	vertexBuffer.set(vertices, sizeof(vertices) / sizeof(*vertices), OtVertexPos::getDescription());
	indexBuffer.set(indices, sizeof(indices) / sizeof(*indices));
}
