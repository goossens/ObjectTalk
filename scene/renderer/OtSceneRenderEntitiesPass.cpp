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
	// remember pass
	ctx.pass = &pass;

	// render all opaque entities
	if (isRenderingOpaque() && ctx.hasOpaqueEntities) {
		// render geometries
		if (ctx.hasOpaqueGeometries) {
			for (auto entity : ctx.opaqueGeometryEntities) {
				auto& grd = ctx.geometryRenderData[entity];
				auto& camera = grd.cameras[ctx.cameraID];

				if (camera.visible) {
					renderOpaqueGeometry(ctx, grd);
				}
			}
		}

		// render models
		if (ctx.hasOpaqueModels) {
			for (auto entity : ctx.modelEntities) {
				auto& mrd = ctx.modelRenderData[entity];

				if (mrd.visible[ctx.cameraID]) {
					renderOpaqueModel(ctx, mrd);
				}
			}
		}

		// render terrain
		if (ctx.hasTerrainEntities) {
			ctx.scene->view<OtTerrainComponent>().each([&](auto entity, auto& terrain) {
				if (!ctx.renderingShadow || terrain.terrain->isCastingShadow()) {
					renderTerrain(ctx, entity, terrain);
				}
			});
		}

		// render grass
		if (ctx.hasGrassEntities) {
			ctx.scene->view<OtGrassComponent>().each([&](auto entity, auto& grass) {
				if (!ctx.renderingShadow || grass.grass->castShadow) {
					renderGrass(ctx, entity, grass);
				}
			});
		}
	}

	// render all transparent geometries
	if (isRenderingTransparent() && ctx.hasTransparentGeometries) {
		for (auto entity : ctx.transparentGeometryEntities) {
			auto& grd = ctx.geometryRenderData[entity];
			auto& camera = grd.cameras[ctx.cameraID];

			if (camera.visible) {
				renderTransparentGeometry(ctx, grd);
			}
		}
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
		auto& grd = ctx.geometryRenderData[entity];
		auto& camera = grd.cameras[ctx.cameraID];

		if (camera.visible) {
			auto& geometry = ctx.scene->getComponent<OtGeometryComponent>(entity);

			if (geometry.transparent) {
				renderTransparentGeometry(ctx, grd);

			} else {
				renderOpaqueGeometry(ctx, grd);
			}
		}
	}

	// render model (if required)
	if (ctx.scene->hasComponent<OtModelComponent>(entity)) {
		auto& mrd = ctx.modelRenderData[entity];

		if (mrd.visible[ctx.cameraID]) {
			renderOpaqueModel(ctx, mrd);
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
	OtGeometryRenderData& grd,
	uint64_t wireframeState,
	uint64_t solidState,
	MaterialSubmission materialSubmission,
	OtShaderProgram& singleProgram,
	OtShaderProgram& instanceProgram) {

	// submit geometry
	if (grd.component->wireframe) {
		grd.component->asset->getGeometry().submitLines();
		ctx.pass->setState(wireframeState);

	} else {
		grd.component->asset->getGeometry().submitTriangles();
		ctx.pass->setState(solidState);
	}

	// submit instance data (if required)
	if (grd.instances) {
		auto& instances = grd.cameras[ctx.cameraID].visibleInstances;
		ctx.idb.submit(instances.data(), instances.size(), sizeof(glm::mat4));
	}

	// submit material uniforms (if required)
	if (materialSubmission == MaterialSubmission::full) {
		ctx.submitMaterialUniforms(*grd.material);

	} else if (materialSubmission == MaterialSubmission::justAlbedo) {
		ctx.submitAlbedoUniforms(*grd.material);
	}

	ctx.pass->setTransform(grd.globalTransform);
	ctx.pass->runShaderProgram(grd.instances ? instanceProgram : singleProgram);
}


//
//	OtSceneRenderEntitiesPass::renderOpaqueModelHelper
//

void OtSceneRenderEntitiesPass::renderOpaqueModelHelper(
	OtSceneRendererContext& ctx,
	OtModelRenderData& mrd,
	uint64_t state,
	MaterialSubmission materialSubmission,
	OtShaderProgram& animatedProgram,
	OtShaderProgram& staticProgram) {

	// run the program
	ctx.pass->setState(state);

	// process all render commands
	auto globalTransform = ctx.scene->getGlobalTransform(mrd.entity);
	auto renderList = mrd.model->getRenderList(globalTransform);

	for (auto& cmd : renderList) {
		// submit the geometry and uniforms
		cmd.mesh->submitTriangles();

		// submit material uniforms (if required)
		if (materialSubmission != MaterialSubmission::none) {
			if (materialSubmission == MaterialSubmission::justAlbedo) {
				ctx.submitAlbedoUniforms(*cmd.material);

			} else {
				ctx.submitMaterialUniforms(*cmd.material);
			}
		}

		// handle animations
		if (cmd.animation) {
			ctx.pass->setTransforms(cmd.transforms.data(), cmd.transforms.size());
			ctx.pass->runShaderProgram(animatedProgram);

		} else {
			ctx.pass->setTransform(cmd.transforms[0]);
			ctx.pass->runShaderProgram(staticProgram);
		}
	}
}


//
//	OtSceneRenderEntitiesPass::renderTerrainHelper
//

void OtSceneRenderEntitiesPass::renderTerrainHelper(
	OtSceneRendererContext& ctx,
	OtTerrainComponent& component,
	uint64_t wireframeState,
	uint64_t solidState,
	OtShaderProgram& program) {

	// submit the geometry and uniforms
	auto& terrain = *component.terrain;
	terrain.getVertices().submit();
	ctx.submitTerrainUniforms(terrain);

	// process all the terrain meshes
	for (auto& mesh : terrain.getMeshes(ctx.camera)) {
		// submit the terrain
		if (terrain.isWireframe()) {
			mesh.tile.lines.submit();
			ctx.pass->setState(wireframeState);

		} else {
			mesh.tile.triangles.submit();
			ctx.pass->setState(solidState);
		}

		// run the program
		ctx.pass->setTransform(mesh.transform);
		ctx.pass->runShaderProgram(program, OtPass::discardNone);
	}

	// cleanup
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
	OtGeometryRenderData& grd,
	uint64_t wireframeState,
	uint64_t solidState,
	MaterialSubmission materialSubmission,
	OtShaderProgram& singleProgram,
	OtShaderProgram& instanceProgram) {

	if (grd.component->wireframe) {
		grd.component->asset->getGeometry().submitLines();
		ctx.pass->setState(wireframeState);

	} else {
		grd.component->asset->getGeometry().submitTriangles();
		ctx.pass->setState(solidState);
	}

	if (grd.instances) {
		auto& instances = grd.cameras[ctx.cameraID].visibleInstances;
		ctx.idb.submit(instances.data(), instances.size(), sizeof(glm::mat4));
	}

	// submit material uniforms (if required)
	if (materialSubmission == MaterialSubmission::full) {
		ctx.submitMaterialUniforms(*grd.material);

	} else if (materialSubmission == MaterialSubmission::justAlbedo) {
		ctx.submitAlbedoUniforms(*grd.material);
	}

	ctx.pass->setTransform(grd.globalTransform);
	ctx.pass->runShaderProgram(grd.instances ? instanceProgram : singleProgram);
}
