//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderGeometryPass
//

void OtSceneRenderer::renderGeometryPass(OtScene* scene) {
	// setup pass
	OtPass pass;
	pass.setClear(false, false);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(gbuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render all non-transparent entities
	for (auto entity : visibleEntities) {
		if (!entity.transparent) {
			if (entity.model) {
				renderModel(pass, scene, entity.entity);

			} else {
				renderGeometry(pass, scene, entity.entity);
			}
		}
	}
}


//
//	OtSceneRenderer::renderGeometry
//

void OtSceneRenderer::renderGeometry(OtPass& pass, OtScene* scene, OtEntity entity) {
	// determine the program
	OtShaderProgram* program;
	auto& material = scene->getComponent<OtMaterialComponent>(entity).material;

	if (material.isKindOf<OtPbrMaterialClass>()) {
		program = &geometryPbrProgram;

	} else if (material.isKindOf<OtTerrainMaterialClass>()) {
		program = &geometryTerrainProgram;
	}

	// submit the material information
	submitMaterialUniforms(material);

	// submit the geometry
	auto& geometry = scene->getComponent<OtGeometryComponent>(entity);

	if (geometry.wireframe) {
		geometry.geometry->submitLines();

	} else {
		geometry.geometry->submitTriangles();
	}

	// set the program state
	if (geometry.wireframe) {
		program->setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateLines);

	} else if (geometry.cullback) {
		program->setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw);

	} else {
		program->setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw);
	}

	// set the transform
	program->setTransform(scene->getGlobalTransform(entity));

	// run the program
	pass.runShaderProgram(*program);
}


//
//	OtSceneRenderer::renderModel
//

void OtSceneRenderer::renderModel(OtPass& pass, OtScene* scene, OtEntity entity) {
	// get the model
	auto& model = scene->getComponent<OtModelComponent>(entity).model;
	auto transform = scene->getComponent<OtTransformComponent>(entity).getTransform();

	// process all the meshes (if required)
	if (model.isReady()) {
		for (auto& mesh : model->getMeshes()) {
			// see if mesh is visible?
			auto aabb = mesh.getAABB();
			aabb.transform(transform);

			if (frustum.isVisibleAABB(aabb)) {
				// submit the material information
				submitPbrUniforms(model->getMaterials()[mesh.getMaterialIndex()].getPbrMaterial());

				// submit the geometry
				mesh.submitTriangles();

				// set the transform
				geometryPbrProgram.setTransform(scene->getGlobalTransform(entity));

				// set the program state
				geometryPbrProgram.setState(
					OtStateWriteRgb |
					OtStateWriteA |
					OtStateWriteZ |
					OtStateDepthTestLess |
					OtStateCullCw);

				// run the program
				pass.runShaderProgram(geometryPbrProgram);
			}
		}
	}
}
