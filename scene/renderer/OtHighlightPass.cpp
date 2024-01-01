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
//	isHighlightable
//

static bool isHighlightable(OtScene* scene, OtEntity entity) {
	// see if specified entity or one of the children is "highlightable"
	bool highlightable = false;
	highlightable |= scene->hasComponent<OtGeometryComponent>(entity) && scene->hasComponent<OtMaterialComponent>(entity);
	highlightable |= scene->hasComponent<OtModelComponent>(entity);

	OtEntity child = scene->getFirstChild(entity);

	while (scene->isValidEntity(child)) {
		highlightable |= isHighlightable(scene, child);
		child = scene->getNextSibling(child);
	}

	return highlightable;
}


//
//	OtSceneRenderer::renderHighlightPass
//

void OtSceneRenderer::renderHighlightPass(OtSceneRendererContext& ctx, OtEntity entity) {
	// see if we have a "highlightable" entity
	if (isHighlightable(ctx.scene, entity)) {
		// update framebuffer size
		selectedBuffer.update(ctx.camera.width, ctx.camera.height);

		// setup pass to render entities as opaque blobs
		OtPass selectPass;
		selectPass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		selectPass.setFrameBuffer(selectedBuffer);
		selectPass.setClear(true, false);
		selectPass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);

		// highlight entity (and its children)
		renderHighlight(ctx, selectPass, entity);

		// render the outline of the entity
		OtPass outlinePass;
		outlinePass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		outlinePass.setFrameBuffer(ctx.compositeBuffer);
		outlinePass.submitQuad(ctx.camera.width, ctx.camera.height);

		outlineUniforms.setValue(0, 1.0f / ctx.camera.width, 1.0f / ctx.camera.height, 0.0f, 0.0f);
		outlineUniforms.submit();

		selectedBuffer.bindColorTexture(selectedSampler, 0);
		outlineProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateBlendAlpha);
		outlinePass.runShaderProgram(outlineProgram);
	}
}


//
//	OtSceneRenderer::renderHighlight
//

void OtSceneRenderer::renderHighlight(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity) {
	// only render if all components are available
	if (ctx.scene->hasComponent<OtGeometryComponent>(entity) && ctx.scene->hasComponent<OtMaterialComponent>(entity)) {
		// render geometry
		ctx.scene->getComponent<OtGeometryComponent>(entity).geometry->submitTriangles();
		selectProgram.setTransform(ctx.scene->getGlobalTransform(entity));
		selectProgram.setState(OtStateWriteRgb);
		pass.runShaderProgram(selectProgram);

	} else if (ctx.scene->hasComponent<OtModelComponent>(entity)) {
		// render model
		auto& model = ctx.scene->getComponent<OtModelComponent>(entity).model;

		if (model.isReady()) {
			for (auto& mesh : model->getMeshes()) {
				mesh.submitTriangles();
				selectProgram.setTransform(ctx.scene->getGlobalTransform(entity));
				selectProgram.setState(OtStateWriteRgb);
				pass.runShaderProgram(selectProgram);
			}
		}
	}

	// also render all the children
	OtEntity child = ctx.scene->getFirstChild(entity);

	while (ctx.scene->isValidEntity(child)) {
		renderHighlight(ctx, pass, child);
		child = ctx.scene->getNextSibling(child);
	}
}
