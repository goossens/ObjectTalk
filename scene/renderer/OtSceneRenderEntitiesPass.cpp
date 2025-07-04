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
			ctx.scene->view<OtGeometryComponent>().each([&](auto entity, auto& component) {
				// ensure geometry is ready to be rendered
				if (!component.transparent && component.asset.isReady()) {
					if (!ctx.renderingShadow || component.castShadow) {
						renderOpaqueGeometry(ctx, entity, component);
					}
				}
			});
		}

		// render models
		if (ctx.hasOpaqueModels) {
			ctx.scene->view<OtModelComponent>().each([&](auto entity, auto& component) {
				if (component.asset.isReady()) {
					if (!ctx.renderingShadow || component.castShadow) {
						renderOpaqueModel(ctx, entity, component);
					}
				}
			});
		}

		// render terrain
		if (ctx.hasTerrainEntities) {
			ctx.scene->view<OtTerrainComponent>().each([&](auto entity, auto& terrain) {
				if (!ctx.renderingShadow || terrain.terrain->isCastingShadow()) {
					renderTerrain(ctx, entity, terrain);
				}
			});
		}
	}

	// render all transparent geometries
	if (isRenderingTransparent() && ctx.hasTransparentEntities) {
		ctx.scene->view<OtGeometryComponent>().each([&](auto entity, auto& component) {
			if (component.transparent && component.asset.isReady()) {
				if (!ctx.renderingShadow || component.castShadow) {
					renderTransparentGeometry(ctx, entity, component);
				}
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
		auto& component = ctx.scene->getComponent<OtGeometryComponent>(entity);

		if (component.asset.isReady()) {
			if (!ctx.renderingShadow || component.castShadow) {
				if (isRenderingOpaque() && !component.transparent) {
					renderOpaqueGeometry(ctx, entity, component);

				} else if (isRenderingTransparent() && component.transparent) {
					renderTransparentGeometry(ctx, entity, component);
				}
			}
		}
	}

	// render model (if required)
	if (ctx.scene->hasComponent<OtModelComponent>(entity)) {
		auto& component = ctx.scene->getComponent<OtModelComponent>(entity);

		if (isRenderingOpaque() && component.asset.isReady()) {
			if (!ctx.renderingShadow || component.castShadow) {
				renderOpaqueModel(ctx, entity, component);
			}
		}
	}

	// render terrain
	if (ctx.scene->hasComponent<OtTerrainComponent>(entity)) {
		auto& terrain = ctx.scene->getComponent<OtTerrainComponent>(entity);

		if (!ctx.renderingShadow || terrain.terrain->isCastingShadow()) {
			renderTerrain(ctx, entity, terrain);
		}
	}
}


//
//	OtSceneRenderEntitiesPass::renderOpaqueGeometry
//

void OtSceneRenderEntitiesPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& component) {
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program = nullptr;

	// get geometry AABB
	auto& aabb = component.asset->getGeometry().getAABB();

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(ctx.camera, aabb)) {
			visible = true;
			program = getInstancedOpaqueProgram();
		}

	} else {
		// see if geometry is visible
		if (ctx.camera.isVisibleAABB(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
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
		if (component.wireframe) {
			component.asset->getGeometry().submitLines();

		} else {
			component.asset->getGeometry().submitTriangles();
		}

		// set the program state
		if (component.wireframe) {
			program->setState(getWireframeState());

		} else if (component.cullBack) {
			program->setState(getCullBackState());

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

	// get geometry AABB
	auto& model = component.asset->getModel();
	auto globalTransform = ctx.scene->getGlobalTransform(entity);
	auto aabb = model.getAABB().transform(globalTransform);

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(ctx.camera, aabb)) {
			visible = true;
			program = getInstancedOpaqueProgram();
		}

	} else {
		// see if model is visible
		if (ctx.camera.isVisibleAABB(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
			visible = true;
			program = getOpaqueProgram();
		}
	}

	if (visible) {
		// process all render commands
		auto renderList = model.getRenderList(globalTransform);

		for (auto& cmd : renderList) {
			// submit uniforms
			Scope scope{entity, false, false, cmd.material, nullptr};
			submitUniforms(ctx, scope);

			// submit the geometry
			cmd.mesh->submitTriangles();

			// run the program
			program->setState(getCullBackState());

			if (cmd.animation) {
				program->setTransforms(cmd.transforms.data(), cmd.transforms.size());
				ctx.pass->runShaderProgram(*getAnimatedOpaqueProgram());

			} else {
				program->setTransform(cmd.transforms[0]);
				ctx.pass->runShaderProgram(*program);
			}
		}
	 }
}


//
//	OtSceneRenderEntitiesPass::renderTerrain
//

void OtSceneRenderEntitiesPass::renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& component) {
	// process all the terrain meshes
	auto terrain = component.terrain;

	for (auto& mesh : terrain->getMeshes(ctx.camera)) {
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
		program->setState(terrain->isWireframe() ? getWireframeState() : getCullBackState());
		program->setTransform(mesh.transform);

		// run the program
		ctx.pass->runShaderProgram(*program);
	}
}


//
//	OtSceneRenderEntitiesPass::renderTransparentGeometry
//

void OtSceneRenderEntitiesPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& component) {
	// visibility flag and target program
	bool visible = false;
	OtShaderProgram* program = nullptr;

	// get geometry AABB
	auto& aabb =  component.asset->getGeometry().getAABB();

	// is this a case of instancing?
	if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
		// only render instances if we have a valid asset and at least one instance is visible
		auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

		if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(ctx.camera, aabb)) {
			visible = true;
			program = getInstancedTransparentProgram();
		}

	} else {
		// see if geometry is visible
		if (ctx.camera.isVisibleAABB(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
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
		if (component.wireframe) {
			component.asset->getGeometry().submitLines();

		} else {
			component.asset->getGeometry().submitTriangles();
		}

		// set the program state
		if (component.wireframe) {
			program->setState(getWireframeState());

		} else if (component.cullBack) {
			program->setState(getCullBackState());

		} else {
			program->setState(getNormalState());
		}

		// run the program
		program->setTransform(ctx.scene->getGlobalTransform(entity));
		ctx.pass->runShaderProgram(*program);
	}
}
