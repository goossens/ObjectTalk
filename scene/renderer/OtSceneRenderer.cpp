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
//	OtSceneRenderer::render
//

int OtSceneRenderer::render(OtScene* scene, OtEntity selected) {
	// prepare all render passes
	renderPreProcessingPass(scene, selected);

	// see if we are doing some deferred rendering into a gbuffer?
	if (hasOpaqueEntities || hasTerrainEntities) {
		// update and clear gbuffer
		gbuffer.update(width, height);

		OtPass pass;
		pass.setClear(true, true, glm::vec4(0.0f));
		pass.setRectangle(0, 0, width, height);
		pass.setFrameBuffer(gbuffer);

		// render deferred entities
		if (hasOpaqueEntities) {
			renderDeferredGeometryPass(scene);
		}

		if (hasTerrainEntities) {
			renderDeferredTerrainPass(scene);
		}
	}

	// start rendering to composite buffer
	renderBackgroundPass(scene);

	if (hasSkyEntities) {
		renderSkyPass(scene);
	}

	if (hasOpaqueEntities) {
		renderDeferredLightingPass(scene);
	}

	if (hasTransparentEntities) {
		renderForwardGeometryPass(scene);
	}

	// handle editor passes
	if (gridScale > 0.0) {
		renderGridPass();
	}

	if (renderEntityHighlight) {
		renderHighlightPass(scene, selected);
	}

	// post process buffer
	renderPostProcessingPass(scene);
	return postProcessBuffer.getColorTextureIndex();
}
