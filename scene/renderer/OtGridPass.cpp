//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtVertex.h"

#include "OtGridPass.h"


//
//	OtGridPass::render
//

void OtGridPass::render(OtSceneRendererContext& ctx, OtFrameBuffer* framebuffer) {
	if (gridScale > 0.0f) {
		// setup pass
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(*framebuffer);
		pass.setViewTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);

		// send out geometry
		static glm::vec3 vertices[] = {
			glm::vec3{-1.0f, -1.0f, 0.0f},
			glm::vec3{1.0f, -1.0f, 0.0f},
			glm::vec3{1.0f, 1.0f, 0.0f},
			glm::vec3{-1.0f, 1.0f, 0.0f}
		};

		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};
		OtTransientVertexBuffer vertexBuffer;
		OtTransientIndexBuffer indexBuffer;
		vertexBuffer.submit(vertices, 4, OtVertexPos::getLayout());
		indexBuffer.submit(indices, 6);

		// set uniforms
		ctx.gridUniforms.setValue(0, gridScale, 0.0f, 0.0f, 0.0f);
		ctx.gridUniforms.submit();

		// run the program
		pass.setState(
			OtPass::stateWriteRgb |
			OtPass::stateWriteA |
			OtPass::stateWriteZ |
			OtPass::stateDepthTestLess |
			OtPass::stateBlendAlpha);

		pass.runShaderProgram(program);
	}
}
