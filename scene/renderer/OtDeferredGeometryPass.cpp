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
//	OtSceneRenderer::renderDeferredGeometryPass
//

void OtSceneRenderer::renderDeferredGeometryPass(OtScene* scene) {
	// setup pass
	OtPass pass;
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(gbuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render all non-transparent entities
	for (auto entity : visibleEntities) {
		if (!entity.transparent) {
			if (entity.model) {
				renderDeferredModel(pass, scene, entity.entity);

			} else {
				renderDeferredGeometry(pass, scene, entity.entity);
			}
		}
	}
}


//
//	OtSceneRenderer::renderDeferredGeometry
//

void OtSceneRenderer::renderDeferredGeometry(OtPass& pass, OtScene* scene, OtEntity entity) {
	// determine the program
	OtShaderProgram* program;
	auto& material = scene->getComponent<OtMaterialComponent>(entity).material;

	if (material.isKindOf<OtPbrMaterialClass>()) {
		program = &deferredPbrProgram;
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
			OtStateDepthTestLess);
	}

	// set the transform
	program->setTransform(scene->getGlobalTransform(entity));

	// run the program
	pass.runShaderProgram(*program);
}


//
//	OtSceneRenderer::renderDeferredModel
//

void OtSceneRenderer::renderDeferredModel(OtPass& pass, OtScene* scene, OtEntity entity) {
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
				deferredPbrProgram.setTransform(scene->getGlobalTransform(entity));

				// set the program state
				deferredPbrProgram.setState(
					OtStateWriteRgb |
					OtStateWriteA |
					OtStateWriteZ |
					OtStateDepthTestLess |
					OtStateCullCw);

				// run the program
				pass.runShaderProgram(deferredPbrProgram);
			}
		}
	}
}
