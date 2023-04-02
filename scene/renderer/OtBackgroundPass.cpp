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

	glm::u8vec3 result = glm::u8vec3(glm::round(backgroundColor * 255.0f));
	uint32_t clearColor = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | 255;

	// setup pass
	OtPass pass;
	pass.setClear(true, !hasOpaqueObjects, clearColor);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(compositeBuffer);

	// copy depth buffer from geometry gbuffer to the composite framebuffer (if we already rendered opaque objects)
	if (hasOpaqueObjects) {
		pass.blit(compositeBuffer.getDepthTexture(), 0, 0, gbuffer.getDepthTexture());
	}
}
