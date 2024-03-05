//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

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
			// ensure they are ready to be rendered
			if (!geometry.transparent && geometry.asset.isReady()) {
				renderDeferredGeometry(ctx, pass, entity, geometry, material);
			}
		});
	}

	// render all opaque models
	if (ctx.hasOpaqueModels) {
		ctx.scene->view<OtModelComponent>().each([&](auto entity, auto& model) {
			if (model.model.isReady()) {
				renderDeferredModel(ctx, pass, entity, model);
			}
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
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program;

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit()) {
			visible = true;
			program = &deferredInstancingProgram;
		}

	} else {
		// see if geometry is visible
		auto aabb = geometry.asset->getGeometry().getAABB().transform(ctx.scene->getGlobalTransform(entity));

		if (ctx.camera.frustum.isVisibleAABB(aabb)) {
			visible = true;
			program = &deferredPbrProgram;
		}
	}

	// ensure geometry is visible
	if (visible) {
		// submit the material and clipping uniforms
		submitMaterialUniforms(*material.material);
		submitClippingUniforms(ctx.clippingPlane);

		// submit the geometry
		auto geom = geometry.asset->getGeometry();

		if (geometry.wireframe) {
			geom.submitLines();

		} else {
			geom.submitTriangles();
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
		program->setTransform(ctx.scene->getGlobalTransform(entity));

		// run the program
		pass.runShaderProgram(*program);
	}
}


//
//	OtSceneRenderer::renderDeferredModel
//

void OtSceneRenderer::renderDeferredModel(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtModelComponent& component) {
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program;

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit()) {
			visible = true;
			program = &deferredInstancingProgram;
		}

	} else {
		// see if model is visible
		auto aabb = component.model->getAABB().transform(ctx.scene->getGlobalTransform(entity));

		if (ctx.camera.frustum.isVisibleAABB(aabb)) {
			visible = true;
			program = &deferredPbrProgram;
		}
	}

	if (visible) {
		// process all the meshes
		auto model = component.model;

		for (auto& mesh : model->getMeshes()) {
			// submit the material and clipping information
			submitMaterialUniforms(model->getMaterials()[mesh.getMaterialIndex()].getPbrMaterial());
			submitClippingUniforms(ctx.clippingPlane);

			// submit the geometry
			mesh.submitTriangles();

			// set the transform
			program->setTransform(ctx.scene->getGlobalTransform(entity));

			// set the program state
			program->setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateCullCw);

			// run the program
			pass.runShaderProgram(*program);
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
