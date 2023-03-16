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
//	OtSceneRenderer::renderTransparentPass
//

void OtSceneRenderer::renderTransparentPass(OtScene* scene) {
	// setup pass
	OtPass pass;
	pass.setClear(false, false);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(compositeBuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render all transparent geometries (in scene hierarchical order)
	scene->eachEntityDepthFirst([&](OtEntity entity) {
		if (scene->hasComponent<OtGeometryComponent>(entity) &&
			scene->hasComponent<OtTransformComponent>(entity) &&
			scene->hasComponent<OtMaterialComponent>(entity)) {

			if (scene->getComponent<OtGeometryComponent>(entity).transparent) {
				renderTransparentGeometry(pass, scene, entity);
			}
		}
	});
}


//
//	OtSceneRenderer::renderTransparentGeometry
//

void OtSceneRenderer::renderTransparentGeometry(OtPass& pass, OtScene* scene, OtEntity entity) {
	// submit the material and light uniforms
	submitMaterialUniforms(scene, entity);
	submitLightUniforms(scene);

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
			OtStateLines |
			OtStateBlendAlpha);

	} else if (geometry.cullback) {
		geometryShader.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw |
			OtStateBlendAlpha);

	} else {
		geometryShader.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateBlendAlpha);
	}

	// set the transform
	transparentShader.setTransform(scene->getGlobalTransform(entity));

	// run the shader
	pass.runShader(transparentShader);
}
