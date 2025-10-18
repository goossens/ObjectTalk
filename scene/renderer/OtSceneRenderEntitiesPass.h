//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "OtPass.h"

#include "OtSceneRendererContext.h"


//
//	OtSceneRenderEntitiesPass
//

class OtSceneRenderEntitiesPass {
protected:
	// render all entities in scene and call appropriate render functions (see below)
	void renderEntities(OtSceneRendererContext& ctx, OtPass& pass);
	void renderEntity(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity);

	virtual void renderOpaqueGeometry(OtSceneRendererContext&, OtEntity, OtGeometryComponent&) {}
	virtual void renderOpaqueModel(OtSceneRendererContext&, OtEntity, OtModelComponent&) {}
	virtual void renderTerrain(OtSceneRendererContext&, OtEntity, OtTerrainComponent&) {}
	virtual void renderGrass(OtSceneRendererContext&, OtEntity, OtGrassComponent&) {}
	virtual void renderTransparentGeometry(OtSceneRendererContext&, OtEntity, OtGeometryComponent&) {}

	// methods that must be overriden by subclasses
	virtual bool isRenderingOpaque() = 0;
	virtual bool isRenderingTransparent() = 0;

	// helper functions
	void renderOpaqueGeometryHelper(
		OtSceneRendererContext& ctx,
		OtEntity entity,
		OtGeometryComponent& geometry,
		uint64_t wireframeState,
		uint64_t solidState,
		OtShaderProgram& singleProgram,
		OtShaderProgram& instanceProgram);

	void renderOpaqueModelHelper(
		OtSceneRendererContext& ctx,
		OtEntity entity,
		OtModelComponent& model,
		uint64_t state,
		OtShaderProgram& animatedProgram,
		OtShaderProgram& staticProgram);

	void renderTerrainHelper(
		OtSceneRendererContext& ctx,
		OtTerrainComponent& terrain,
		uint64_t wireframeState,
		uint64_t solidState,
		OtShaderProgram& program);

	void renderGrassHelper(
		OtSceneRendererContext& ctx,
		OtEntity entity,
		OtGrassComponent& grass,
		uint64_t state,
		OtShaderProgram& program);

	void renderTransparentGeometryHelper(
		OtSceneRendererContext& ctx,
		OtEntity entity,
		OtGeometryComponent& geometry,
		uint64_t wireframeState,
		uint64_t solidState,
		OtShaderProgram& singleProgram,
		OtShaderProgram& instanceProgram);
};

