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

ImTextureID OtSceneRenderer::render(OtCamera& camera, OtScene* scene) {
	// reset rendering context
	OtMeasureStopWatch stopwatch;
	ctx.initialize(scene, camera);
	ctxTime = stopwatch.lap();

	// generate shadow maps (if required)
	if (ctx.castShadow) {
		shadowPass.render(ctx);
	}

	shadowPassTime = stopwatch.lap();

	// render background items
	compositeBuffer.update(camera.width, camera.height);
	backgroundPass.render(ctx);
	backgroundPassTime = stopwatch.lap();

	// render opaque entities
	if (ctx.hasOpaqueEntities) {
		deferredRenderingBuffer.update(camera.width, camera.height);
		deferredPass.render(ctx);
	}

	opaquePassTime = stopwatch.lap();

	// render water (if required)
	if (ctx.hasWaterEntities) {
		waterPass.render(ctx);
	}

	waterPassTime = stopwatch.lap();

	// render sky (if required)
	if (ctx.hasSkyEntities) {
		skyPass.render(ctx);
	}

	skyPassTime = stopwatch.lap();

	// render particles
	if (ctx.hasParticlesEntities) {
		particlePass.render(ctx);
	}

	particlePassTime = stopwatch.lap();

	// render transparent entities
	if (ctx.hasTransparentGeometries) {
		forwardPass.render(ctx);
	}

	transparentPassTime = stopwatch.lap();

	gridPass.render(ctx);
	gridPassTime = stopwatch.lap();

	// post process frame
	auto output = postProcessingPass.render(ctx);
	postProcessingTime = stopwatch.lap();

	// handle editor passes
	if (scene->isValidEntity(selectedEntity)) {
		highlightPass.render(ctx, output, selectedEntity);
	}

	if (pickingCallback) {
		pickingCallback(pickingPass.render(ctx, pickingUV));
		pickingCallback = nullptr;
	}

	editorPassTime = stopwatch.lap();
	renderTime = stopwatch.elapsed();
	return output->getTextureID();
}
