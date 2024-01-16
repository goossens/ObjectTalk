//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "glm/glm.hpp"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderDeferredGeometryPass
//

void OtSceneRenderer::renderDeferredGeometryPass(OtSceneRendererContext& ctx) {
	// update gbuffer
	ctx.deferedBuffer.update(ctx.camera.width, ctx.camera.height);

	// setup pass
	OtPass pass;
	pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
	pass.setFrameBuffer(ctx.deferedBuffer);
	pass.setClear(true, true, glm::vec4(0.0f));
	pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);
	pass.touch();

	// render all opaque geometries
	if (ctx.hasOpaqueGeometries) {
		ctx.scene->view<OtGeometryComponent, OtMaterialComponent>().each([&](auto entity, auto& geometry, auto& material) {
			if (!geometry.transparent) {
				renderDeferredGeometry(ctx, pass, entity, geometry, material);
			}
		});
	}

	// render all opaque models
	if (ctx.hasOpaqueModels) {
		ctx.scene->view<OtModelComponent>().each([&](auto entity, auto& model) {
			renderDeferredModel(ctx, pass, entity, model);
		});
	}

	// render all terrain entities
	if (ctx.hasTerrainEntities) {
		ctx.scene->view<OtTerrainComponent>().each([&](auto entity, auto& terrain) {
			renderDeferredTerrain(ctx, pass, terrain);
		});
	}
}


//
//	OtSceneRenderer::renderDeferredGeometry
//

void OtSceneRenderer::renderDeferredGeometry(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtGeometryComponent& geometry, OtMaterialComponent& material) {
	// see if geometry is visible
	auto transform = ctx.scene->getGlobalTransform(entity);
	auto aabb = geometry.geometry->getAABB().transform(transform);

	// is this entity visible
	if (ctx.camera.frustum.isVisibleAABB(aabb)) {
		// determine the program
		OtShaderProgram* program;

		if (material.material.isKindOf<OtPbrMaterialClass>()) {
			program = &deferredPbrProgram;
		}

		// submit the material and clipping uniforms
		submitMaterialUniforms(material.material);
		submitClippingUniforms(ctx.clippingPlane);

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

void OtSceneRenderer::renderDeferredModel(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtModelComponent& component) {
	// process all the meshes (if required)
	auto model = component.model;

	if (model->isReady()) {
		auto transform = ctx.scene->getGlobalTransform(entity);

		for (auto& mesh : model->getMeshes()) {
			// see if mesh is visible?
			auto aabb = mesh.getAABB();
			aabb.transform(transform);

			if (ctx.camera.frustum.isVisibleAABB(aabb)) {
				// submit the material and clipping information
				submitPbrUniforms(model->getMaterials()[mesh.getMaterialIndex()].getPbrMaterial());
				submitClippingUniforms(ctx.clippingPlane);

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

void OtSceneRenderer::renderDeferredTerrain(OtSceneRendererContext& ctx, OtPass& pass, OtTerrainComponent& component) {
	// refresh heightmap and normalmap if the properties have changed
	auto terrain = component.terrain;

	if (terrain->heights.dirty) {
		terrain->heights.update(tileableFbm, normalMapper);
	}

	// process all the terrain meshes
	for (auto& mesh : terrain->getMeshes(ctx.camera.frustum, ctx.camera.cameraPosition)) {
		// submit the geometry
		mesh.tile.vertices.submit();

		// submit the terrain and uniforms
		submitTerrainUniforms(terrain);
		submitClippingUniforms(ctx.clippingPlane);

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
