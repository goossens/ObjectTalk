//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "OtFrameBuffer.h"
#include "OtPass.h"
#include "OtShaderProgram.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtUniformVec4.h"
#include "OtVertex.h"

#include "OtSceneRenderPass.h"


//
//	OtGridPass
//

class OtGridPass : public OtSceneRenderPass {
public:
	// constructor
	OtGridPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// access properties
	void setGridScale(float gs) { gridScale = gs; }

	// render the pass
	void render(OtSceneRendererContext& ctx) {
		if (gridScale > 0.0f) {
			// setup pass
			OtPass pass;
			pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
			pass.setFrameBuffer(framebuffer);
			pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);

			// send out geometry
			static glm::vec3 vertices[] = {
				glm::vec3{-1.0, -1.0, 0.0},
				glm::vec3{1.0, -1.0, 0.0},
				glm::vec3{1.0, 1.0, 0.0},
				glm::vec3{-1.0, 1.0, 0.0}
			};

			static uint32_t indices[] = {0, 1, 2, 2, 3, 0};
			OtTransientVertexBuffer vertexBuffer;
			OtTransientIndexBuffer indexBuffer;
			vertexBuffer.submit(vertices, 4, OtVertexPos::getLayout());
			indexBuffer.submit(indices, 6);

			// set uniforms
			uniforms.setValue(0, gridScale, 0.0f, 0.0f, 0.0f);
			uniforms.submit();

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
	// grid scale (0.0 means no grid)
	float gridScale = 0.0f;

	// properties
	OtFrameBuffer& framebuffer;
	OtUniformVec4 uniforms{"u_grid", 1};
	OtShaderProgram program{"OtGridVS", "OtGridFS"};
};
