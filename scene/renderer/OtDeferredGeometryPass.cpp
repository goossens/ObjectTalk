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

	// render all opaque geometries
	if (hasOpaqueGeometries) {
		scene->view<OtGeometryComponent, OtMaterialComponent>().each([&](auto entity, auto& geometry, auto& material) {
			if (!geometry.transparent) {
				renderDeferredGeometry(pass, scene, entity, geometry, material);
			}
		});
	}

	// render all opaque models
	if (hasOpaqueModels) {
		scene->view<OtModelComponent>().each([&](auto entity, auto& model) {
			renderDeferredModel(pass, scene, entity, model);
		});
	}

	// render all terrain entities
	if (hasTerrainEntities) {
		scene->view<OtTerrainComponent>().each([&](auto entity, auto& terrain) {
			renderDeferredTerrain(pass, terrain);
		});
	}

	// render all water entities
	if (hasWaterEntities) {
		scene->view<OtWaterComponent>().each([&](auto entity, auto& water) {
			renderDeferredWater(pass, water);
		});
	}
}


//
//	OtSceneRenderer::renderDeferredGeometry
//

void OtSceneRenderer::renderDeferredGeometry(OtPass& pass, OtScene* scene, OtEntity entity, OtGeometryComponent& geometry, OtMaterialComponent& material) {
	// see if geometry is visible
	auto transform = scene->getGlobalTransform(entity);
	auto aabb = geometry.geometry->getAABB().transform(transform);

	// is this entity visible
	if (frustum.isVisibleAABB(aabb)) {
		// determine the program
		OtShaderProgram* program;

		if (material.material.isKindOf<OtPbrMaterialClass>()) {
			program = &deferredPbrProgram;
		}

		// submit the material information
		submitMaterialUniforms(material.material);

		// submit the geometry
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
		program->setTransform(transform);

		// run the program
		pass.runShaderProgram(*program);
	}
}


//
//	OtSceneRenderer::renderDeferredModel
//

void OtSceneRenderer::renderDeferredModel(OtPass& pass, OtScene* scene, OtEntity entity, OtModelComponent& component) {
	// process all the meshes (if required)
	auto model = component.model;

	if (model->isReady()) {
		auto transform = scene->getGlobalTransform(entity);

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
				deferredPbrProgram.setTransform(transform);

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


//
//	OtSceneRenderer::renderDeferredTerrain
//

void OtSceneRenderer::renderDeferredTerrain(OtPass& pass, OtTerrainComponent& component) {
	// refresh heightmap and normalmap if the properties have changed
	auto terrain = component.terrain;

	if (terrain->heights.dirty) {
		terrain->heights.update(tileableFbm, normalMapper);
	}
	// process all the terrain meshes
	for (auto& mesh : terrain->getMeshes(frustum, cameraPosition)) {
		// submit the geometry
		mesh.tile.vertices.submit();

		// submit the terrain uniforms
		submitTerrainUniforms(terrain);

		if (terrain->isWireframe()) {
			mesh.tile.lines.submit();

		} else {
			mesh.tile.triangles.submit();
		}

		// set the program state
		if (terrain->isWireframe()) {
			deferredTerrainProgram.setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateLines);

		} else {
			deferredTerrainProgram.setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateCullCw);
		}

		// set the transform
		deferredTerrainProgram.setTransform(mesh.transform);

		// run the program
		pass.runShaderProgram(deferredTerrainProgram);
	}
}


//
//	OtSceneRenderer::renderDeferredWater
//

void OtSceneRenderer::renderDeferredWater(OtPass& pass, OtWaterComponent& water) {

}
