//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"

#include "OtHash.h"

#include "OtVertex.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderDeferredTerrainPass
//

void OtSceneRenderer::renderDeferredTerrainPass(OtScene* scene) {
	// setup pass
	OtPass pass;
	pass.setClear(false, false);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(gbuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render each terrain mesh
	for (auto&& [entity, terrain, material] : scene->view<OtTerrainComponent, OtMaterialComponent>().each()) {
		renderDeferredTerrain(pass, terrain, material);
	}
}


//
//	OtSceneRenderer::renderDeferredTerrain
//

void OtSceneRenderer::renderDeferredTerrain(OtPass& pass, OtTerrainComponent& terrain, OtMaterialComponent& material) {
	// determine the program
	OtShaderProgram* program;

	if (material.material.isKindOf<OtPbrMaterialClass>()) {
		program = &deferredTerrainPbrProgram;

	} else if (material.material.isKindOf<OtTerrainMaterialClass>()) {
		program = &deferredTerrainTerrainProgram;
	}

	// submit the material information
	submitMaterialUniforms(material.material);

	// process all the terrain meshes
	for (auto& mesh : terrain.terrain.getMeshes(frustum, cameraPosition)) {
		// submit the geometry
		mesh.tile.vertices.submit();

		if (terrain.terrain.isWireframe()) {
			mesh.tile.lines.submit();

		} else {
			mesh.tile.triangles.submit();
		}

		// set the program state
		if (terrain.terrain.isWireframe()) {
			program->setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateLines);

		} else {
			program->setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateCullCw);
		}

		// set the transform
		program->setTransform(mesh.transform);

		// run the program
		pass.runShaderProgram(*program);
	}
}
