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
//	OtSceneRenderer::renderGeometryPass
//

void OtSceneRenderer::renderGeometryPass(OtScene* scene) {
	// update gbuffer
	gbuffer.update(width, height);

	// setup pass
	OtPass pass;
	pass.setClear(true, true, 0);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(gbuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render all non-transparent geometries (that have transform and material components)
	for (auto entity : scene->view<OtGeometryComponent, OtTransformComponent, OtMaterialComponent>()) {
		if (!scene->getComponent<OtGeometryComponent>(entity).transparent) {
			renderGeometry(pass, scene, entity);
		}
	}
}


//
//	OtSceneRenderer::renderGeometry
//

void OtSceneRenderer::renderGeometry(OtPass& pass, OtScene* scene, OtEntity entity) {
	// submit the material information
	submitMaterialUniforms(scene, entity);

	// submit the geometry
	auto& geometry = scene->getComponent<OtGeometryComponent>(entity);

	if (geometry.wireframe) {
		geometry.geometry->submitLines();

	} else {
		geometry.geometry->submitTriangles();
	}

	// set the shader state
	if (geometry.wireframe) {
		geometryShader.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateLines);

	} else if (geometry.cullback) {
		geometryShader.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw);

	} else {
		geometryShader.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess);
	}

	// set the transform
	geometryShader.setTransform(scene->getGlobalTransform(entity));

	// run the shader
	pass.runShader(geometryShader);
}
