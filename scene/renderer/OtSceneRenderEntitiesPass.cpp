//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

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
			ctx.scene->view<OtGeometryComponent>().each([&](auto entity, auto& geometry) {
				// ensure geometry is ready to be rendered
				if (!geometry.transparent && geometry.asset.isReady()) {
					renderOpaqueGeometry(ctx, entity, geometry);
				}
			});
		}

		// render models
		if (ctx.hasOpaqueModels) {
			ctx.scene->view<OtModelComponent>().each([&](auto entity, auto& model) {
				if (model.model.isReady()) {
					renderOpaqueModel(ctx, entity, model);
				}
			});
		}

		// render terrain
		if (ctx.hasTerrainEntities) {
			ctx.scene->view<OtTerrainComponent>().each([&](auto entity, auto& terrain) {
				renderTerrain(ctx, entity, terrain);
			});
		}
	}

	// render all transparent geometries
	if (isRenderingTransparent() && ctx.hasTransparentEntities) {
		ctx.scene->view<OtGeometryComponent>().each([&](auto entity, auto& geometry) {
			if (geometry.transparent && geometry.asset.isReady()) {
				renderTransparentGeometry(ctx, entity, geometry);
			}
		});
	}
}


//
//	OtSceneRenderEntitiesPass::renderEntity
///

void OtSceneRenderEntitiesPass::renderEntity(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity) {
	// remember pass
	ctx.pass = &pass;

	// render geometry (if required)
	if (ctx.scene->hasComponent<OtGeometryComponent>(entity)) {
		auto& geometry = ctx.scene->getComponent<OtGeometryComponent>(entity);

		if (geometry.asset.isReady()) {
			if (isRenderingOpaque() && !geometry.transparent) {
				renderOpaqueGeometry(ctx, entity, geometry);

			} else if (isRenderingTransparent() && geometry.transparent) {
				renderTransparentGeometry(ctx, entity, geometry);
			}
		}
	}

	// render model (if required)
	if (ctx.scene->hasComponent<OtModelComponent>(entity)) {
		auto& model = ctx.scene->getComponent<OtModelComponent>(entity);

		if (isRenderingOpaque() && model.model.isReady()) {
			renderOpaqueModel(ctx, entity, model);
		}
	}

	// render terrain
	if (ctx.scene->hasComponent<OtTerrainComponent>(entity)) {
		auto& terrain = ctx.scene->getComponent<OtTerrainComponent>(entity);
		renderTerrain(ctx, entity, terrain);
	}
}


//
//	OtSceneRenderEntitiesPass::renderOpaqueGeometry
//

void OtSceneRenderEntitiesPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& geometry) {
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program = nullptr;

	// get camera frustum and geometry AABB
	auto& frustum = ctx.camera.frustum;
	auto& aabb = geometry.asset->getGeometry().getAABB();

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(frustum, aabb)) {
			visible = true;
			program = getInstancedOpaqueProgram();
		}

	} else {
		// see if geometry is visible
		if (frustum.isVisibleAABB(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
			visible = true;
			program = getOpaqueProgram();
		}
	}

	// ensure geometry is visible
	if (visible) {
		// get current material
		std::shared_ptr<OtMaterial> material;

		if (ctx.scene->hasComponent<OtMaterialComponent>(entity)) {
			material = ctx.scene->getComponent<OtMaterialComponent>(entity).material;

		} else {
			material = std::make_shared<OtMaterial>();
		}

		// submit uniforms
		Scope scope{entity, false, false, material, nullptr};
		submitUniforms(ctx, scope);

		// submit the geometry
		if (geometry.wireframe) {
			geometry.asset->getGeometry().submitLines();

		} else {
			geometry.asset->getGeometry().submitTriangles();
		}

		// set the program state
		if (geometry.wireframe) {
			program->setState(getWireframeState());

		} else if (geometry.cullback) {
			program->setState(getCullbackState());

		} else {
			program->setState(getNormalState());
		}

		// set the transform
		program->setTransform(ctx.scene->getGlobalTransform(entity));

		// run the program
		ctx.pass->runShaderProgram(*program);
	}
}


//
//	OtSceneRenderEntitiesPass::renderOpaqueModel
//

void OtSceneRenderEntitiesPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtEntity entity, OtModelComponent& component) {
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program = nullptr;

	// get camera frustum and geometry AABB
	auto& frustum = ctx.camera.frustum;
	auto& aabb = component.model->getAABB();

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(frustum, aabb)) {
			visible = true;
			program = getInstancedOpaqueProgram();
		}

	} else {
		// see if model is visible
		if (frustum.isVisibleAABB(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
			visible = true;
			program = getOpaqueProgram();
		}
	}

	if (visible) {
		// process all the meshes
		auto model = component.model;

		for (auto& mesh : model->getMeshes()) {
			// submit uniforms
			Scope scope{entity, false, false, model->getMaterials()[mesh.getMaterialIndex()].getMaterial(), nullptr};
			submitUniforms(ctx, scope);

			// submit the geometry
			mesh.submitTriangles();

			// run the program
			program->setTransform(ctx.scene->getGlobalTransform(entity));
			program->setState(getCullbackState());
			ctx.pass->runShaderProgram(*program);
		}
	}
}


//
//	OtSceneRenderEntitiesPass::renderTerrain
//

void OtSceneRenderEntitiesPass::renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& component) {
	// process all the terrain meshes
	auto terrain = component.terrain;

	for (auto& mesh : terrain->getMeshes(ctx.camera.frustum, ctx.camera.position)) {
		// submit the geometry
		mesh.tile.vertices.submit();

		// submit uniforms
		Scope scope{entity, false, true, nullptr, terrain};
		submitUniforms(ctx, scope);

		// submit the terrain
		if (terrain->isWireframe()) {
			mesh.tile.lines.submit();

		} else {
			mesh.tile.triangles.submit();
		}

		// set the program state
		auto program = getTerrainProgram();
		program->setState(terrain->isWireframe() ? getWireframeState() : getCullbackState());
		program->setTransform(mesh.transform);

		// run the program
		ctx.pass->runShaderProgram(*program);
	}
}


//
//	OtSceneRenderEntitiesPass::renderTransparentGeometry
//

void OtSceneRenderEntitiesPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& geometry) {
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program = nullptr;

	// get camera frustum and geometry AABB
	auto& frustum = ctx.camera.frustum;
	auto& aabb =  geometry.asset->getGeometry().getAABB();

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(frustum, aabb)) {
			visible = true;
			program = getInstancedTransparentProgram();
		}

	} else {
		// see if geometry is visible
		if (frustum.isVisibleAABB(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
			visible = true;
			program = getTransparentProgram();
		}
	}

	// ensure geometry is visible
	if (visible) {
		// get current material
		std::shared_ptr<OtMaterial> material;

		if (ctx.scene->hasComponent<OtMaterialComponent>(entity)) {
			material = ctx.scene->getComponent<OtMaterialComponent>(entity).material;

		} else {
			material = std::make_shared<OtMaterial>();
		}

		// submit uniforms
		Scope scope{entity, true, false, material, nullptr};
		submitUniforms(ctx, scope);

		// submit the geometry
		if (geometry.wireframe) {
			geometry.asset->getGeometry().submitLines();

		} else {
			geometry.asset->getGeometry().submitTriangles();
		}

		// set the program state
		if (geometry.wireframe) {
			program->setState(getWireframeState());

		} else if (geometry.cullback) {
			program->setState(getCullbackState());

		} else {
			program->setState(getNormalState());
		}

		// run the program
		program->setTransform(ctx.scene->getGlobalTransform(entity));
		ctx.pass->runShaderProgram(*program);
	}
}
