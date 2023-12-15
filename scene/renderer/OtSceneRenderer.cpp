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
	// create rendering context
	OtSceneRendererContext context{
		width, height,
		cameraPosition, viewMatrix, projectionMatrix,
		deferredRenderingBuffer, deferredCompositeBuffer, postProcessBuffer,
		scene};

	// generate reflection (if required)
	if (context.hasWaterEntities) {
		renderReflectionPass(context);
	}

	// see if we need to do some deferred rendering into a gbuffer?
	if (context.hasOpaqueEntities) {
		// render deferred entities
		renderDeferredGeometryPass(context);
	}

	// start rendering to composite buffer
	renderBackgroundPass(context);

	if (context.hasSkyEntities) {
		renderSkyPass(context);
	}

	if (context.hasOpaqueEntities) {
		renderDeferredLightingPass(context);
	}

	if (context.hasTransparentEntities) {
		renderForwardGeometryPass(context);
	}

	// handle editor passes
	if (gridScale > 0.0f) {
		renderGridPass(context);
	}

	if (selected != OtEntityNull) {
		renderHighlightPass(context, selected);
	}

	// post process buffer
	renderPostProcessingPass(context);
	return postProcessBuffer.getColorTextureIndex();
}
