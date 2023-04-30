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
	// update gbuffer
	gbuffer.update(width, height);

	// setup pass
	OtPass pass;
	pass.setClear(true, true, glm::vec4(0.0f));
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(gbuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render all non-transparent entities
	for (auto entity : visibleEntities) {
		if (entity.model) {
			renderModel(pass, scene, entity.entity);

		} else {
			renderGeometry(pass, scene, entity.entity);
		}
	}
}


//
//	OtSceneRenderer::renderGeometry
//

void OtSceneRenderer::renderGeometry(OtPass& pass, OtScene* scene, OtEntity entity) {
	// determine the shader
	OtShader* shader;
	auto& material = scene->getComponent<OtMaterialComponent>(entity).material;

	if (material.isKindOf<OtPbrMaterialClass>()) {
		shader = &geometryShader;

	} else if (material.isKindOf<OtTerrainMaterialClass>()) {
		shader = &terrainShader;
	}

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
		shader->setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateLines);

	} else if (geometry.cullback) {
		shader->setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw);

	} else {
		shader->setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw);
	}

	// set the transform
	shader->setTransform(scene->getGlobalTransform(entity));

	// run the shader
	pass.runShader(*shader);
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
				geometryShader.setTransform(scene->getGlobalTransform(entity));

				// set the shader state
				geometryShader.setState(
					OtStateWriteRgb |
					OtStateWriteA |
					OtStateWriteZ |
					OtStateDepthTestLess |
					OtStateCullCw);

				// run the shader
				pass.runShader(geometryShader);
			}
		}
	}
}
