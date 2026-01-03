//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"
#include "OtGbuffer.h"
#include "OtIndexBuffer.h"
#include "OtVertexBuffer.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtDeferredPass
//

class OtDeferredPass : public OtSceneRenderEntitiesPass {
public:
	// constructor
	OtDeferredPass(OtGbuffer& gb, OtFrameBuffer& fb) : gbuffer(gb), framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx);

protected:
	bool isRenderingOpaque() override { return true; }
	bool isRenderingTransparent() override { return false; }

	void renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) override;
	void renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) override;
	void renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& terrain)  override;
	void renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass)  override;

private:
	// properties
	OtGbuffer& gbuffer;
	OtFrameBuffer& framebuffer;

	OtRenderPipeline cullingPipeline;
	OtRenderPipeline noCullingPipeline;
	OtRenderPipeline linesPipeline;

	OtRenderPipeline instancedCullingPipeline;
	OtRenderPipeline instancedNoCullingPipeline;
	OtRenderPipeline instancedLinesPipeline;

	OtRenderPipeline animatedPipeline;
	OtRenderPipeline terrainCullingPipeline;
	OtRenderPipeline terrainLinesPipeline;
	OtRenderPipeline grassPipeline;

	OtRenderPipeline directionalLightPipeline;
	OtRenderPipeline pointLightsPipeline;

	OtVertexBuffer pointLightVertices;
	OtVertexBuffer pointLightInstances;
	OtIndexBuffer pointLightIndices;

	// support functions
	void renderGeometry(OtSceneRendererContext& ctx);
	void renderDirectionalLight(OtSceneRendererContext& ctx);
	void renderPointLights(OtSceneRendererContext& ctx);

	void initializeResources();
	bool resourcesInitialized = false;
};
