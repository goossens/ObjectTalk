//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"
#include "OtRenderPipeline.h"
#include "OtSampler.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtHighlightPass
//

class OtHighlightPass : public OtSceneRenderEntitiesPass {
public:
	// render the pass
	void render(OtSceneRendererContext& ctx, OtTexture* texture, OtEntity entity);

private:
	// render entities as opaque blobs
	void renderSelectedPass(OtSceneRendererContext& ctx, OtEntity entity);

	// render the outline of the selected entity(s)
	void renderHighlightPass(OtSceneRendererContext& ctx, OtTexture* texture);

	// recursive method to render an entity and it's children
	void renderHighlight(OtSceneRendererContext& ctx, OtEntity entity);

	// see if specified entity or one of the children is "highlightable"
	bool isHighlightable(OtScene* scene, OtEntity entity);

private:
	// properties
	OtFrameBuffer selectedBuffer{OtTexture::Format::r8};
	OtSampler sampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};

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

	OtRenderPipeline outlinePipeline;

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
