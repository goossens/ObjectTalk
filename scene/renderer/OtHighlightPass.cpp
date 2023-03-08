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

void OtSceneRenderer::renderHighlightPass(std::shared_ptr<OtScene> scene, OtEntity entity) {
	// update framebuffer size
	selectedBuffer.update(width, height);

	// setup pass to render selected entities as opaque blobs
	OtPass selectPass;
	selectPass.reserveRenderingSlot();
	selectPass.setClear(true, false);
	selectPass.setRectangle(0, 0, width, height);
	selectPass.setFrameBuffer(selectedBuffer);
	selectPass.setTransform(viewMatrix, projectionMatrix);

	// render selected entity (and its children)
	renderHighlight(selectPass, scene, entity);

	// render the outline of the selected entity
	OtPass outlinePass;
	outlinePass.reserveRenderingSlot();
	outlinePass.setClear(false, false);
	outlinePass.setRectangle(0, 0, width, height);
	outlinePass.setFrameBuffer(compositeBuffer);
	outlinePass.submitQuad(width, height);

	outlineUniforms.set(0, glm::vec4(1.0 / width, 1.0 / height, 0.0f, 0.0f));
	outlineUniforms.submit();

	selectedBuffer.bindColorTexture(selectedSampler, 0);
	outlineShader.setState(OtStateWriteRgb | OtStateWriteA | OtStateBlendAlpha);
	outlinePass.runShader(outlineShader);
}


//
//	OtSceneRenderer::renderHighlight
//

void OtSceneRenderer::renderHighlight(OtPass& pass, std::shared_ptr<OtScene> scene, OtEntity entity) {
	// only render if all components are available
	if (scene->hasComponent<OtGeometryComponent>(entity) &&
		scene->hasComponent<OtTransformComponent>(entity) &&
		scene->hasComponent<OtMaterialComponent>(entity)) {

		// render geometry
		scene->getComponent<OtGeometryComponent>(entity).geometry->submitTriangles();
		selectShader.setTransform(scene->getGlobalTransform(entity));
		selectShader.setState(OtStateWriteRgb);
		pass.runShader(selectShader);
	}

	// also render all the children
	OtEntity child = scene->getFirstChild(entity);

	while (scene->isValidEntity(child)) {
		renderHighlight(pass, scene, child);
		child = scene->getNextSibling(child);
	}
}
