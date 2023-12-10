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

void OtSceneRenderer::renderHighlightPass(OtScene* scene, OtEntity entity) {
	// see if we have a "highlightable" entity
	if (isHighlightable(scene, entity)) {
		// update framebuffer size
		selectedBuffer.update(width, height);

		// setup pass to render entities as opaque blobs
		OtPass selectPass;
		selectPass.setClear(true, false);
		selectPass.setRectangle(0, 0, width, height);
		selectPass.setFrameBuffer(selectedBuffer);
		selectPass.setTransform(viewMatrix, projectionMatrix);

		// highlight entity (and its children)
		renderHighlight(selectPass, scene, entity);

		// render the outline of the entity
		OtPass outlinePass;
		outlinePass.setRectangle(0, 0, width, height);
		outlinePass.setFrameBuffer(compositeBuffer);
		outlinePass.submitQuad(width, height);

		outlineUniforms.setValue(0, glm::vec4(1.0 / width, 1.0 / height, 0.0f, 0.0f));
		outlineUniforms.submit();

		selectedBuffer.bindColorTexture(selectedSampler, 0);
		outlineProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateBlendAlpha);
		outlinePass.runShaderProgram(outlineProgram);
	}
}


//
//	OtSceneRenderer::renderHighlight
//

void OtSceneRenderer::renderHighlight(OtPass& pass, OtScene* scene, OtEntity entity) {
	// only render if all components are available
	if (scene->hasComponent<OtGeometryComponent>(entity) && scene->hasComponent<OtMaterialComponent>(entity)) {
		// render geometry
		scene->getComponent<OtGeometryComponent>(entity).geometry->submitTriangles();
		selectProgram.setTransform(scene->getGlobalTransform(entity));
		selectProgram.setState(OtStateWriteRgb);
		pass.runShaderProgram(selectProgram);

	} else if (scene->hasComponent<OtModelComponent>(entity)) {
		// render model
		auto& model = scene->getComponent<OtModelComponent>(entity).model;

		if (model.isReady()) {
			for (auto& mesh : model->getMeshes()) {
				mesh.submitTriangles();
				selectProgram.setTransform(scene->getGlobalTransform(entity));
				selectProgram.setState(OtStateWriteRgb);
				pass.runShaderProgram(selectProgram);
			}
		}
	}

	// also render all the children
	OtEntity child = scene->getFirstChild(entity);

	while (scene->isValidEntity(child)) {
		renderHighlight(pass, scene, child);
		child = scene->getNextSibling(child);
	}
}
