//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtMeasure.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::render
//

int OtSceneRenderer::render(OtCamera& camera, OtScene* scene) {
	// reset rendering context
	ctx.initialize(camera, scene, &ibl, &csm);
	OtMeasureStopWatch stopwatch;

	// update image based lighting (if required)
	if (ctx.hasImageBasedLighting) {
		ibl.update(ctx.scene->getComponent<OtIblComponent>(ctx.iblEntity));
	}

	iblPassTime = stopwatch.lap();

	// generate shadow maps (if required)
	if (ctx.castShadow) {
		shadowPass.render(ctx);
	}

	shadowPassTime = stopwatch.lap();

	// render background
	compositeBuffer.update(camera.width, camera.height);
	backgroundPass.render(ctx);

	backgroundPassTime = stopwatch.lap();

	// render opaque entities
	if (ctx.hasOpaqueEntities) {
		deferredRenderingBuffer.update(camera.width, camera.height);
		deferredPass.render(ctx);
	}

	opaquePassTime = stopwatch.lap();

	// render sky (if required)
	if (ctx.hasSkyEntities) {
		skyPass.render(ctx);
	}

	skyPassTime = stopwatch.lap();

	// render water (if required)
	if (ctx.hasWaterEntities) {
		waterPass.render(ctx);
	}

	waterPassTime = stopwatch.lap();

	// render particles
	if (ctx.hasParticlesEntities) {
		particlePass.render(ctx);
	}

	particlePassTime = stopwatch.lap();

	// render transparent entities
	if (ctx.hasTransparentEntities) {
		forwardPass.render(ctx);
	}

	transparentPassTime = stopwatch.lap();

	// handle editor passes
	gridPass.render(ctx);

	if (scene->isValidEntity(selectedEntity)) {
		highlightPass.render(ctx, selectedEntity);
	}

	if (pickingCallback) {
		pickingPass.render(ctx, pickingNDC, pickingCallback);
		pickingCallback = nullptr;
	}

	editorPassTime = stopwatch.lap();

	// post process frame
	auto textureIndex = postProcessingPass.render(ctx);
	postProcessingTime = stopwatch.lap();
	return textureIndex;
}
