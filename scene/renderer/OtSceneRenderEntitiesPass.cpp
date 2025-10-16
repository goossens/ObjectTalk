//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtTransientIndexBuffer.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtSceneRenderEntitiesPass::renderEntities
//

void OtSceneRenderEntitiesPass::renderEntities(OtSceneRendererContext& ctx, OtPass& pass) {
	// render all opaque entities
	if (isRenderingOpaque() && ctx.hasOpaqueEntities) {
		// render geometries
		if (ctx.hasOpaqueGeometries) {
			ctx.scene->view<OtGeometryComponent>().each([&](auto entity, auto&) {
				renderEntity(ctx, pass, entity);
			});
		}

		// render models
		if (ctx.hasOpaqueModels) {
			ctx.scene->view<OtModelComponent>().each([&](auto entity, auto&) {
				renderEntity(ctx, pass, entity);
			});
		}

		// render terrain
		if (ctx.hasTerrainEntities) {
			ctx.scene->view<OtTerrainComponent>().each([&](auto entity, auto&) {
				renderEntity(ctx, pass, entity);
			});
		}

		// render grass
		if (ctx.hasGrassEntities) {
			ctx.scene->view<OtGrassComponent>().each([&](auto entity, auto&) {
				renderEntity(ctx, pass, entity);
			});
		}
	}

	// render all transparent geometries
	if (isRenderingTransparent() && ctx.hasTransparentEntities) {
		ctx.scene->view<OtGeometryComponent>().each([&](auto entity, auto&) {
			renderEntity(ctx, pass, entity);
		});
	}
}


//
//	OtSceneRenderEntitiesPass::renderEntity
//

void OtSceneRenderEntitiesPass::renderEntity(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity) {
	// remember pass
	ctx.pass = &pass;

	// render geometry (if required)
	if (ctx.scene->hasComponent<OtGeometryComponent>(entity)) {
		auto& geometry = ctx.scene->getComponent<OtGeometryComponent>(entity);

		if (geometry.asset.isReady() && (!ctx.renderingShadow || geometry.castShadow)) {
			// see if entity is visible
			auto globalTransform = ctx.scene->getGlobalTransform(entity);
			auto aabb = geometry.asset->getGeometry().getAABB().transform(globalTransform);

			// is this a case of instancing?
			if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
				auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

				if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(ctx.camera, aabb)) {
					if (geometry.transparent) {
						renderTransparentGeometry(ctx, entity, geometry, true);

					} else {
						renderOpaqueGeometry(ctx, entity, geometry, true);
					}
				}

			} else {
				// see if geometry is visible
				if (ctx.camera.isVisibleAABB(aabb)) {
					if (geometry.transparent) {
						renderTransparentGeometry(ctx, entity, geometry, false);

					} else {
						renderOpaqueGeometry(ctx, entity, geometry, false);
					}
				}
			}
		}
	}

	// render model (if required)
	if (ctx.scene->hasComponent<OtModelComponent>(entity)) {
		auto& model = ctx.scene->getComponent<OtModelComponent>(entity);

		if (model.asset.isReady() && (!ctx.renderingShadow || model.castShadow)) {
			// see if model is visible
			auto globalTransform = ctx.scene->getGlobalTransform(entity);
			auto aabb = model.asset->getModel().getAABB().transform(globalTransform);

			// is this a case of instancing?
			if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
				auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

				if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(ctx.camera, aabb)) {
					renderOpaqueModel(ctx, entity, model, true);
				}

			} else {
				// see if model is visible
				if (ctx.camera.isVisibleAABB(aabb)) {
					renderOpaqueModel(ctx, entity, model, false);
				}
			}
		}
	}

	// render terrain (if required)
	if (ctx.scene->hasComponent<OtTerrainComponent>(entity)) {
		auto& terrain = ctx.scene->getComponent<OtTerrainComponent>(entity);

		if (!ctx.renderingShadow || terrain.terrain->isCastingShadow()) {
			renderTerrain(ctx, entity, terrain);
		}
	}

	// render grass (if required)
	if (ctx.scene->hasComponent<OtGrassComponent>(entity)) {
		auto& grass = ctx.scene->getComponent<OtGrassComponent>(entity);

		if (!ctx.renderingShadow || grass.grass->castShadow) {
			renderGrass(ctx, entity, grass);
		}
	}
}


//
//	OtSceneRenderEntitiesPass::renderOpaqueGeometryHelper
//

void OtSceneRenderEntitiesPass::renderOpaqueGeometryHelper(
	OtSceneRendererContext& ctx,
	OtEntity entity,
	OtGeometryComponent& geometry,
	uint64_t wireframeState,
	uint64_t solidState,
	bool simpleMaterial,
	OtShaderProgram& program) {

	if (geometry.wireframe) {
		geometry.asset->getGeometry().submitLines();
		ctx.pass->setState(wireframeState);

	} else {
		geometry.asset->getGeometry().submitTriangles();
		ctx.pass->setState(solidState);
	}

	auto material = ctx.scene->hasComponent<OtMaterialComponent>(entity)
		? ctx.scene->getComponent<OtMaterialComponent>(entity).material
		: std::make_shared<OtMaterial>();

	if (simpleMaterial) {
		ctx.submitAlbedoUniforms(*material);

	} else {
		ctx.submitMaterialUniforms(*material);
	}

	ctx.pass->setTransform(ctx.scene->getGlobalTransform(entity));
	ctx.pass->runShaderProgram(program);
}


//
//	OtSceneRenderEntitiesPass::renderOpaqueModelHelper
//

void OtSceneRenderEntitiesPass::renderOpaqueModelHelper(
	OtSceneRendererContext& ctx,
	OtEntity entity,
	OtModelComponent& model,
	uint64_t state,
	bool simpleMaterial,
	OtShaderProgram& animatedProgram,
	OtShaderProgram& staticProgram) {

	// run the program
	ctx.pass->setState(state);

	// process all render commands
	auto globalTransform = ctx.scene->getGlobalTransform(entity);
	auto renderList = model.asset->getModel().getRenderList(globalTransform);

	for (auto& cmd : renderList) {
		// submit the geometry
		cmd.mesh->submitTriangles();

		// set the uniforms
		if (simpleMaterial) {
			ctx.submitAlbedoUniforms(*cmd.material);

		} else {
			ctx.submitMaterialUniforms(*cmd.material);
		}

		// don't discard instancing and uniform data after shader run
		static constexpr uint8_t discard = OtDiscardIndexBuffer | OtDiscardVertexStreams;

		// handle animations
		if (cmd.animation) {
			ctx.pass->setTransforms(cmd.transforms.data(), cmd.transforms.size());
			ctx.pass->runShaderProgram(animatedProgram, discard);

		} else {
			ctx.pass->setTransform(cmd.transforms[0]);
			ctx.pass->runShaderProgram(staticProgram, discard);
		}
	}

	// cleanup (now we can discard instancing and uniform data)
	ctx.pass->discard();
}


//
//	OtSceneRenderEntitiesPass::renderTerrainHelper
//

void OtSceneRenderEntitiesPass::renderTerrainHelper(
	OtSceneRendererContext& ctx,
	OtTerrainComponent& terrain,
	uint64_t wireframeState,
	uint64_t solidState,
	OtShaderProgram& program) {

	// process all the terrain meshes
	for (auto& mesh : terrain.terrain->getMeshes(ctx.camera)) {
		// submit the geometry
		mesh.tile.vertices.submit();

		// submit the terrain
		if (terrain.terrain->isWireframe()) {
			mesh.tile.lines.submit();
			ctx.pass->setState(wireframeState);

		} else {
			mesh.tile.triangles.submit();
			ctx.pass->setState(solidState);
		}

		// submit uniforms
		ctx.submitTerrainUniforms(*terrain.terrain);

		// don't discard uniform data and samplers after shader run
		static constexpr uint8_t discard = OtDiscardIndexBuffer | OtDiscardVertexStreams;

		// run the program
		ctx.pass->setTransform(mesh.transform);
		ctx.pass->runShaderProgram(program, discard);
	}

	// cleanup (now we can discard uniform data)
	ctx.pass->discard();
}


//
//	OtSceneRenderEntitiesPass::renderGrassHelper
//

void OtSceneRenderEntitiesPass::renderGrassHelper(
	OtSceneRendererContext& ctx,
	OtEntity entity,
	OtGrassComponent& grass,
	uint64_t state,
	OtShaderProgram& program) {

	// submit geometry and uniforms
	grass.grass->submit();
	ctx.submitGrassUniforms(*grass.grass);

	// run the program
	ctx.pass->setState(state);
	ctx.pass->setTransform(ctx.scene->getGlobalTransform(entity));
	ctx.pass->setVertexCount((grass.grass->bladeSegments + 1) * 4);
	ctx.pass->setInstanceCount(grass.grass->blades);
	ctx.pass->runShaderProgram(program);
}


//
//	OtSceneRenderEntitiesPass::renderTransparentGeometryHelper
//

void OtSceneRenderEntitiesPass::renderTransparentGeometryHelper(
	OtSceneRendererContext& ctx,
	OtEntity entity,
	OtGeometryComponent& geometry,
	uint64_t wireframeState,
	uint64_t solidState,
	bool simpleMaterial,
	OtShaderProgram& program) {

	if (geometry.wireframe) {
		geometry.asset->getGeometry().submitLines();
		ctx.pass->setState(wireframeState);

	} else {
		geometry.asset->getGeometry().submitTriangles();
		ctx.pass->setState(solidState);
	}

	auto material = ctx.scene->hasComponent<OtMaterialComponent>(entity)
		? ctx.scene->getComponent<OtMaterialComponent>(entity).material
		: std::make_shared<OtMaterial>();

	if (simpleMaterial) {
		ctx.submitAlbedoUniforms(*material);

	} else {
		ctx.submitMaterialUniforms(*material);
	}

	ctx.pass->setTransform(ctx.scene->getGlobalTransform(entity));
	ctx.pass->runShaderProgram(program);
}
