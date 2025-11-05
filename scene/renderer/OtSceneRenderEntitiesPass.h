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

#include "OtGeometryRenderData.h"
#include "OtGrassComponent.h"
#include "OtModelRenderData.h"
#include "OtSceneRendererContext.h"
#include "OtTerrainComponent.h"


//
//	OtSceneRenderEntitiesPass
//

class OtSceneRenderEntitiesPass {
protected:
	// render all entities in scene and call appropriate render functions (see below)
	void renderEntities(OtSceneRendererContext& ctx, OtPass& pass);
	void renderEntity(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity);

	virtual void renderOpaqueGeometry(OtSceneRendererContext&, OtGeometryRenderData&) {}
	virtual void renderOpaqueModel(OtSceneRendererContext&, OtModelRenderData&) {}
	virtual void renderTerrain(OtSceneRendererContext&, OtEntity, OtTerrainComponent&) {}
	virtual void renderGrass(OtSceneRendererContext&, OtEntity, OtGrassComponent&) {}
	virtual void renderTransparentGeometry(OtSceneRendererContext&, OtGeometryRenderData&) {}

	// methods that must be overriden by subclasses
	virtual bool isRenderingOpaque() = 0;
	virtual bool isRenderingTransparent() = 0;

	// helper functions
	enum class MaterialSubmission {
		none,
		full,
		justAlbedo
	};

	void renderOpaqueGeometryHelper(
		OtSceneRendererContext& ctx,
		OtGeometryRenderData& grd,
		uint64_t wireframeState,
		uint64_t solidState,
		MaterialSubmission materialSubmission,
		OtShaderProgram& singleProgram,
		OtShaderProgram& instanceProgram);

	void renderOpaqueModelHelper(
		OtSceneRendererContext& ctx,
		OtModelRenderData& mrd,
		uint64_t state,
		MaterialSubmission materialSubmission,
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
		OtGeometryRenderData& grd,
		uint64_t wireframeState,
		uint64_t solidState,
		MaterialSubmission materialSubmission,
		OtShaderProgram& singleProgram,
		OtShaderProgram& instanceProgram);
};

