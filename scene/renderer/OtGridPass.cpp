//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtVertex.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderGridPass
//

void OtSceneRenderer::renderGridPass() {
	// setup pass
	OtPass pass;
	pass.setClear(false, false);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(compositeBuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

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
	gridUniforms.set(0, glm::vec4(gridScale, 0.0f, 0.0f, 0.0f));
	gridUniforms.submit();

	// run the program
	gridProgram.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLess |
		OtStateBlendAlpha |
		OtStateMsaa);

	pass.runShaderProgram(gridProgram);
}
