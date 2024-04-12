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
//	OtSceneRenderer::render
//

int OtSceneRenderer::render(OtCamera& camera, OtScene* scene, OtEntity selected) {
	// create rendering context
	OtSceneRendererContext ctx{camera, scene, &ibl, &csm};

	// update image based lighting (if required)
	if (ctx.hasImageBasedLighting) {
		ibl.update(ctx.scene->getComponent<OtIblComponent>(ctx.iblEntity));
	}

	// generate shadow maps (if required)
	if (ctx.castShadow) {
		shadowPass.render(ctx);
	}

	// render background items
	compositeBuffer.update(camera.width, camera.height);
	backgroundPass.render(ctx);

	if (ctx.hasSkyEntities) {
		skyPass.render(ctx);
	}

	// render opaque entities
	if (ctx.hasOpaqueEntities) {
		deferredRenderingBuffer.update(camera.width, camera.height);
		deferredPass.render(ctx);
	}

	// render transparent entities
	if (ctx.hasTransparentEntities) {
		forwardPass.render(ctx);
	}

	// generate water (if required)
	if (ctx.hasWaterEntities) {
		waterPass.render(ctx);
	}

	// render particles
	if (ctx.hasParticlesEntities) {
		particlePass.render(ctx);
	}

	// handle editor passes
	gridPass.render(ctx);
	highlightPass.render(ctx, selected);

	// post process frame
	return postProcessingPass.render(ctx);
}
