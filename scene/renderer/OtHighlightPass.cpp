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
//	OtSceneRenderer::renderHighlightPass
//

void OtSceneRenderer::renderHighlightPass(OtScene* scene, OtEntity selected) {
	// update framebuffer size
	selectedBuffer.update(width, height);

	// setup pass to render selected entities as opaque blobs
	OtPass selectPass;
	selectPass.setClear(true, false);
	selectPass.setRectangle(0, 0, width, height);
	selectPass.setFrameBuffer(selectedBuffer);
	selectPass.setTransform(viewMatrix, projectionMatrix);

	// render selected entity (and its children)
	renderHighlight(selectPass, scene, selected);

	// render the outline of the selected entity
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


//
//	OtSceneRenderer::renderHighlight
//

void OtSceneRenderer::renderHighlight(OtPass& pass, OtScene* scene, OtEntity selected) {
	// only render if all components are available
	if (scene->hasComponent<OtGeometryComponent>(selected) &&
		scene->hasComponent<OtTransformComponent>(selected) &&
		scene->hasComponent<OtMaterialComponent>(selected)) {

		// render geometry
		scene->getComponent<OtGeometryComponent>(selected).geometry->submitTriangles();
		selectProgram.setTransform(scene->getGlobalTransform(selected));
		selectProgram.setState(OtStateWriteRgb);
		pass.runShaderProgram(selectProgram);

	} else if (scene->hasComponent<OtModelComponent>(selected) && scene->hasComponent<OtTransformComponent>(selected)) {
		// render model
		auto& model = scene->getComponent<OtModelComponent>(selected).model;

		if (model.isReady()) {
			for (auto& mesh : model->getMeshes()) {
				mesh.submitTriangles();
				selectProgram.setTransform(scene->getGlobalTransform(selected));
				selectProgram.setState(OtStateWriteRgb);
				pass.runShaderProgram(selectProgram);
			}
		}
	}

	// also render all the children
	OtEntity child = scene->getFirstChild(selected);

	while (scene->isValidEntity(child)) {
		renderHighlight(pass, scene, child);
		child = scene->getNextSibling(child);
	}
}
