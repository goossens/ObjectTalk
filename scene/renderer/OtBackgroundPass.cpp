//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

void OtSceneRenderer::renderBackgroundPass(OtScene* scene) {
	// update composite buffer
	compositeBuffer.update(width, height);

	// determine background color
	glm::vec3 backgroundColor{0.0f};

	for (auto [entity, component] : scene->view<OtBackgroundComponent>().each()) {
		backgroundColor = component.color;
	}

	// setup pass
	OtPass pass;
	pass.setClear(true, !hasOpaqueEntities, glm::vec4(backgroundColor, 1.0f));
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(compositeBuffer);

	// copy depth buffer from gbuffer to the composite framebuffer (if we already rendered opaque objects)
	if (hasOpaqueEntities) {
		pass.blit(compositeBuffer.getDepthTexture(), 0, 0, gbuffer.getDepthTexture());
	}
}
