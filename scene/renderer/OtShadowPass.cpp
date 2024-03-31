//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtc/type_ptr.hpp"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderShadowPass
//

void OtSceneRenderer::renderShadowPass(OtSceneRendererContext& ctx) {
	// update the cascaded shadow map
	csm.update(shadowMapSize, ctx.camera, ctx.directionalLightDirection);

	// render each cascade
	for (auto i = 0; i < OtCascadedShadowMap::cascades; i++) {
		// setup pass to render entities as opaque blobs
		OtPass pass;
		pass.setRectangle(0, 0, shadowMapSize, shadowMapSize);
		pass.setFrameBuffer(csm.getFrameBuffer(i));
		pass.setClear(false, true);
		pass.setTransform(csm.getViewMatrix(i), csm.getProjectionMatrix(i));
		pass.touch();

		// camera's AABB
		auto aabb = csm.getAABB(i);

		// render all geometries
		ctx.scene->view<OtGeometryComponent>().each([&](auto entity, auto& geometry) {
			// ensure they are ready to be rendered
			if (geometry.asset.isReady()) {
				renderShadowGeometry(ctx, pass, entity, geometry, aabb);
			}
		});

		// render all models
		ctx.scene->view<OtModelComponent>().each([&](auto entity, auto& model) {
			if (model.model.isReady()) {
				renderShadowModel(ctx, pass, entity, model, aabb);
			}
		});

		// render terrain
		ctx.scene->view<OtTerrainComponent>().each([&](auto entity, auto& terrain) {
			renderShadowTerrain(ctx, pass, terrain, csm.getCameraPosition(i), aabb);
		});
	}
}


//
//	OtSceneRenderer::renderShadowGeometry
//

void OtSceneRenderer::renderShadowGeometry(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtGeometryComponent& geometry, OtAABB& camera) {
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program;

	// get AABB for cgeometry
	auto& aabb = geometry.asset->getGeometry().getAABB();

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(camera, aabb)) {
			visible = true;
			program = &shadowInstancingProgram;
		}

	} else {
		// see if geometry is visible
		if (camera.overlaps(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
			visible = true;
			program = &shadowProgram;
		}
	}

	// ensure geometry is visible
	if (visible) {
		// submit the geometry
		auto geom = geometry.asset->getGeometry();

		if (geometry.wireframe) {
			geom.submitLines();

		} else {
			geom.submitTriangles();
		}

		// set the program state
		if (geometry.wireframe) {
			program->setState(OtStateWriteZ | OtStateDepthTestLess | OtStateLines);

		} else if (geometry.cullback) {
			program->setState(OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw);

		} else {
			program->setState(OtStateWriteZ | OtStateDepthTestLess);
		}

		// set the transform
		program->setTransform(ctx.scene->getGlobalTransform(entity));

		// run the program
		pass.runShaderProgram(*program);
	}
}


//
//	OtSceneRenderer::renderShadowModel
//

void OtSceneRenderer::renderShadowModel(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity, OtModelComponent& component, OtAABB& camera) {
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program;

	// get geometry AABB
	auto& aabb = component.model->getAABB();

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(camera, aabb)) {
			visible = true;
			program = &shadowInstancingProgram;
		}

	} else {
		// see if model is visible
		if (camera.overlaps(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
			visible = true;
			program = &shadowProgram;
		}
	}

	if (visible) {
		// process all the meshes
		auto model = component.model;

		for (auto& mesh : model->getMeshes()) {
			// submit the geometry
			mesh.submitTriangles();

			// set the transform
			program->setTransform(ctx.scene->getGlobalTransform(entity));

			// set the program state
			program->setState(OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw);

			// run the program
			pass.runShaderProgram(*program);
		}
	}
}


//
//	OtSceneRenderer::renderShadowTerrain
//

void OtSceneRenderer::renderShadowTerrain(OtSceneRendererContext& ctx, OtPass& pass, OtTerrainComponent& component, glm::vec3& camera, OtAABB& aabb) {
	// refresh heightmap and normalmap if the properties have changed
	auto terrain = component.terrain;

	if (terrain->heights.dirty) {
		terrain->heights.update(tileableFbm, normalMapper);
	}

	// process all the terrain meshes
	for (auto& mesh : terrain->getMeshes(aabb, camera)) {
		// submit the geometry
		mesh.tile.vertices.submit();

		// submit the terrain and clipping uniforms
		submitTerrainUniforms(*terrain);
		submitClippingUniforms(ctx);

		// submit the terrain
		if (terrain->isWireframe()) {
			mesh.tile.lines.submit();

		} else {
			mesh.tile.triangles.submit();
		}

		// set the program state
		if (terrain->isWireframe()) {
			deferredTerrainProgram.setState(OtStateWriteZ | OtStateDepthTestLess | OtStateLines);

		} else {
			deferredTerrainProgram.setState(OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw);
		}

		// set the transform
		deferredTerrainProgram.setTransform(mesh.transform);

		// run the program
		pass.runShaderProgram(deferredTerrainProgram);
	}
}
