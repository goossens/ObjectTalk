//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <unordered_map>

#include "OtFrameBuffer.h"
#include "OtReadBackBuffer.h"
#include "OtRenderPipeline.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtPickingPass
//

class OtPickingPass : public OtSceneRenderEntitiesPass {
public:
	// render the pass
	OtEntity render(OtSceneRendererContext& ctx, glm::vec2 uv);

private:
	// properties
	OtFrameBuffer idBuffer{OtTexture::Format::r8, OtTexture::Format::d32};
	OtReadBackBuffer idReadback;
	static constexpr int bufferSize = 8;

	std::unordered_map<int, OtEntity> entityMap;
	int nextID = 1;

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

	void bindFragmentUniforms(OtSceneRendererContext& ctx, OtEntity entity);

	void initializeResources();
	bool resourcesInitialized = false;
};
