//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtPass.h"

#include "OtGeometryComponent.h"
#include "OtGrassComponent.h"
#include "OtHighlightPass.h"
#include "OtModelComponent.h"
#include "OtTerrainComponent.h"


//
//	OtHighlightPass::render
//

void OtHighlightPass::render(OtSceneRendererContext& ctx, OtFrameBuffer* framebuffer, OtEntity entity) {
	// see if we have a "highlightable" entity
	if (isHighlightable(ctx.scene, entity)) {
		// update framebuffer size
		selectedBuffer.update(ctx.camera.width, ctx.camera.height);

		// run both passes
		renderSelectedPass(ctx, entity);
		renderHighlightPass(ctx, framebuffer);
	}
}


//
//	OtHighlightPass::renderSelectedPass
//

void OtHighlightPass::renderSelectedPass(OtSceneRendererContext& ctx, OtEntity entity) {
	// setup pass to render selected entities as opaque blobs
	OtPass pass;
	pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
	pass.setFrameBuffer(selectedBuffer);
	pass.setClear(true);
	pass.setViewTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);
	pass.touch();

	// highlight entity (and its children)
	renderHighlight(ctx, pass, entity);
}


//
//	OtHighlightPass::renderHighlightPass
//

void OtHighlightPass::renderHighlightPass(OtSceneRendererContext& ctx, OtFrameBuffer* framebuffer) {
	// render the outline of the entity
	OtPass pass;
	pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
	pass.setFrameBuffer(*framebuffer);
	pass.submitQuad(ctx.camera.width, ctx.camera.height);

	ctx.highlightOutlineUniforms.setValue(0, 1.0f / ctx.camera.width, 1.0f / ctx.camera.height, 0.0f, 0.0f);
	ctx.highlightOutlineUniforms.submit();

	selectedBuffer.bindColorTexture(ctx.selectedSampler, 0);
	pass.setState(OtPass::stateWriteRgb | OtPass::stateWriteA | OtPass::stateBlendAlpha);
	pass.runShaderProgram(outlineProgram);
}


//
//	OtHighlightPass::renderHighlight
//

void OtHighlightPass::renderHighlight(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity) {
	// render entity and its children
	renderEntity(ctx, pass, entity);
	OtEntity child = ctx.scene->getFirstChild(entity);

	while (ctx.scene->isValidEntity(child)) {
		renderHighlight(ctx, pass, child);
		child = ctx.scene->getNextSibling(child);
	}
}


//
//	OtHighlightPass::isHighlightable
//

bool OtHighlightPass::isHighlightable(OtScene* scene, OtEntity entity) {
	// see if specified entity or one of the children is "highlightable"
	bool highlightable = false;

	if (scene->isValidEntity(entity)) {
		highlightable |= scene->hasComponent<OtGeometryComponent>(entity);
		highlightable |= scene->hasComponent<OtModelComponent>(entity);
		highlightable |= scene->hasComponent<OtTerrainComponent>(entity);
		highlightable |= scene->hasComponent<OtGrassComponent>(entity);

		OtEntity child = scene->getFirstChild(entity);

		while (scene->isValidEntity(child)) {
			highlightable |= isHighlightable(scene, child);
			child = scene->getNextSibling(child);
		}
	}

	return highlightable;
}


//
//	OtHighlightPass::renderOpaqueGeometry
//

void OtHighlightPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	renderOpaqueGeometryHelper(
		ctx,
		grd,
		OtPass::stateWriteRgb | OtPass::stateLines,
		OtPass::stateWriteRgb | (grd.component->cullBack ? OtPass::stateCullCw : 0),
		MaterialSubmission::none,
		opaqueProgram,
		instancedOpaqueProgram);
}


//
//	OtHighlightPass::renderOpaqueModel
//

void OtHighlightPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) {
	renderOpaqueModelHelper(
		ctx,
		mrd,
		OtPass::stateWriteRgb | OtPass::stateCullCw,
		MaterialSubmission::none,
		animatedOpaqueProgram,
		opaqueProgram);
}


//
//	OtHighlightPass::renderTerrain
//

void OtHighlightPass::renderTerrain(OtSceneRendererContext& ctx, [[maybe_unused]] OtEntity entity, OtTerrainComponent& terrain) {
	renderTerrainHelper(
		ctx,
		terrain,
		OtPass::stateWriteRgb | OtPass::stateLines,
		OtPass::stateWriteRgb | OtPass::stateCullCw,
		terrainProgram);
}


//
//	OtHighlightPass::renderGrass
//

void OtHighlightPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	renderGrassHelper(
		ctx,
		entity,
		grass,
		OtPass::stateWriteRgb | OtPass::stateCullCw,
		grassProgram);
}


//
//	OtHighlightPass::renderTransparentGeometry
//

void OtHighlightPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	renderTransparentGeometryHelper(
		ctx,
		grd,
		OtPass::stateWriteRgb | OtPass::stateLines,
		OtPass::stateWriteRgb | (grd.component->cullBack ? OtPass::stateCullCw : 0),
		MaterialSubmission::justAlbedo,
		transparentProgram,
		instancedTransparentProgram);
}
