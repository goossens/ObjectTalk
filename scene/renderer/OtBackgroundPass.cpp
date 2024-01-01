//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderBackgroundPass
//

void OtSceneRenderer::renderBackgroundPass(OtSceneRendererContext& ctx) {
	// update composite buffer
	ctx.compositeBuffer.update(ctx.camera.width, ctx.camera.height);

	// determine background color
	glm::vec3 backgroundColor{0.0f};

	for (auto&& [entity, component] : ctx.scene->view<OtBackgroundComponent>().each()) {
		backgroundColor = component.color;
	}

	// setup pass
	OtPass pass;
	pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
	pass.setFrameBuffer(ctx.compositeBuffer);
	pass.setClear(true, true, glm::vec4(backgroundColor, 1.0f));
	pass.touch();
}
