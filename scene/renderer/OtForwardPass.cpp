//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderForwardGeometryPass
//

void OtSceneRenderer::renderForwardGeometryPass(OtScene* scene) {
	// setup pass
	OtPass pass;
	pass.setClear(false, false);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(compositeBuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render all transparent entities
	for (auto entity : visibleEntities) {
		if (entity.transparent) {
			if (entity.instanced) {

			} else {
				if (!entity.model) {
					renderForwardGeometry(pass, scene, entity.entity);
				}
			}
		}
	}
}


//
//	OtSceneRenderer::renderForwardGeometry
//

void OtSceneRenderer::renderForwardGeometry(OtPass& pass, OtScene* scene, OtEntity entity) {
	// submit the material and light uniforms
	submitMaterialUniforms(scene->getComponent<OtMaterialComponent>(entity).material);
	submitLightUniforms(scene);

	// submit the geometry
	auto& geometry = scene->getComponent<OtGeometryComponent>(entity);

	if (geometry.wireframe) {
		geometry.geometry->submitLines();

	} else {
		geometry.geometry->submitTriangles();
	}

	// set the program state
	if (geometry.wireframe) {
		forwardPbrProgram.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateLines |
			OtStateBlendAlpha);

	} else if (geometry.cullback) {
		forwardPbrProgram.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw |
			OtStateBlendAlpha);

	} else {
		forwardPbrProgram.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateBlendAlpha);
	}

	// set the transform
	forwardPbrProgram.setTransform(scene->getGlobalTransform(entity));

	// run the program
	pass.runShaderProgram(forwardPbrProgram);
}
