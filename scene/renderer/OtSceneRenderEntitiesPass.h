//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "OtInstances.h"
#include "OtMaterial.h"
#include "OtSampler.h"
#include "OtTextureAsset.h"

#include "OtGeometryRenderData.h"
#include "OtGrassComponent.h"
#include "OtModelRenderData.h"
#include "OtSceneRendererContext.h"
#include "OtTerrainComponent.h"


//
//	OtSceneRenderEntitiesPass
//

class OtSceneRenderEntitiesPass {
public:
	// destructor
	virtual ~OtSceneRenderEntitiesPass() {}

protected:
	// render all entities in scene and call appropriate render functions (see below)
	void renderEntities(OtSceneRendererContext& ctx);
	void renderEntity(OtSceneRendererContext& ctx, OtEntity entity);

	virtual void renderOpaqueGeometry(OtSceneRendererContext&, OtGeometryRenderData&) {}
	virtual void renderOpaqueModel(OtSceneRendererContext&, OtModelRenderData&) {}
	virtual void renderTerrain(OtSceneRendererContext&, OtEntity, OtTerrainComponent&) {}
	virtual void renderGrass(OtSceneRendererContext&, OtEntity, OtGrassComponent&) {}
	virtual void renderTransparentGeometry(OtSceneRendererContext&, OtGeometryRenderData&) {}

	// subclasses must overwrite these methods if required (these are called by renderEntities)
	virtual bool isRenderingOpaque() { return false; }
	virtual bool isRenderingTransparent() { return false; }

	// helper functions
	enum class MaterialSubmission {
		none,
		full,
		justAlbedo
	};

	void renderGeometryHelper(
		OtSceneRendererContext& ctx,
		OtGeometryRenderData& grd,
		MaterialSubmission materialSubmission,
		OtRenderPipeline& cullingPipeline,
		OtRenderPipeline& noCullingPipeline,
		OtRenderPipeline& linesPipeline,
		OtRenderPipeline& instancedCullingPipeline,
		OtRenderPipeline& instancedNoCullingPipeline,
		OtRenderPipeline& instancedLinesPipeline);

	void renderModelHelper(
		OtSceneRendererContext& ctx,
		OtModelRenderData& mrd,
		MaterialSubmission materialSubmission,
		OtRenderPipeline& staticPipeline,
		OtRenderPipeline& animatedPipeline);

	void renderTerrainHelper(
		OtSceneRendererContext& ctx,
		OtTerrainComponent& terrain,
		bool setFragmentUniforms,
		OtRenderPipeline& cullingPipeline,
		OtRenderPipeline& linesPipeline);

	void renderGrassHelper(
		OtSceneRendererContext& ctx,
		OtEntity entity,
		OtGrassComponent& grass,
		OtRenderPipeline& pipeline);

	void setMaterialUniforms(OtSceneRendererContext& ctx, size_t uniformSlot, size_t samplerSlot, std::shared_ptr<OtMaterial> material);
	void setMaterialUniforms(OtSceneRendererContext& ctx, size_t uniformSlot, size_t samplerSlot, OtEntity entity);

	void setAlbedoUniforms(OtSceneRendererContext& ctx, size_t uniformSlot, size_t samplerSlot, std::shared_ptr<OtMaterial> material);
	void setAlbedoUniforms(OtSceneRendererContext& ctx, size_t uniformSlot, size_t samplerSlot, OtEntity entity);

	size_t materialUniformSlot = 0;
	size_t materialSamplerSlot = 0;

	size_t albedoUniformSlot = 0;
	size_t albedoSamplerSlot = 0;
};
