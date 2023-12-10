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
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(compositeBuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render all transparent geometries
	scene->view<OtGeometryComponent, OtMaterialComponent>().each([&](auto entity, auto& geometry, auto& material) {
		if (geometry.transparent) {
			renderForwardGeometry(pass, scene, entity, geometry, material);
		}
	});
}


//
//	OtSceneRenderer::renderForwardGeometry
//

void OtSceneRenderer::renderForwardGeometry(OtPass& pass, OtScene* scene, OtEntity entity, OtGeometryComponent& geometry, OtMaterialComponent& material) {
	// submit the material and light uniforms
	submitMaterialUniforms(material.material);
	submitLightUniforms(scene);

	// submit the geometry
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
