//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtRenderPipeline.h"

#include "OtSceneRendererContext.h"
#include "OtSceneRenderEntitiesPass.h"


//
//	OtShadowPass
//

class OtShadowPass : public OtSceneRenderEntitiesPass {
public:
	// render the pass
	void render(OtSceneRendererContext& ctx);

private:
	// properties
	OtRenderPipeline opaqueCullingPipeline;
	OtRenderPipeline opaqueNoCullingPipeline;
	OtRenderPipeline opaqueLinesPipeline;

	OtRenderPipeline opaqueInstancedCullingPipeline;
	OtRenderPipeline opaqueInstancedNoCullingPipeline;
	OtRenderPipeline opaqueInstancedLinesPipeline;

	OtRenderPipeline animatedPipeline;
	OtRenderPipeline terrainCullingPipeline;
	OtRenderPipeline terrainLinesPipeline;
	OtRenderPipeline grassPipeline;

	OtRenderPipeline transparentCullingPipeline;
	OtRenderPipeline transparentNoCullingPipeline;
	OtRenderPipeline transparentLinesPipeline;

	OtRenderPipeline transparentInstancedCullingPipeline;
	OtRenderPipeline transparentInstancedNoCullingPipeline;
	OtRenderPipeline transparentInstancedLinesPipeline;

	// support functions
	bool isRenderingOpaque() override { return true; }
	bool isRenderingTransparent() override { return true; }

	void renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) override;
	void renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) override;
	void renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& terrain)  override;
	void renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass)  override;
	void renderTransparentGeometry(OtSceneRendererContext&ctx , OtGeometryRenderData& grd) override;

	void initializeResources();
	bool resourcesInitialized = false;
};
