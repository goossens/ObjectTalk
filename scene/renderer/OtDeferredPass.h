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
#include "OtGbuffer.h"
#include "OtShaderProgram.h"

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

	// render all opaque geometry
	void renderGeometry(OtSceneRendererContext& ctx);

	// run direct lighting calculations
	void renderDirectionalLight(OtSceneRendererContext& ctx);

	// render point lights
	void renderPointLights(OtSceneRendererContext& ctx);

protected:
	// methods that must be overriden by subclasses (when required)
	inline bool isRenderingOpaque() override { return true; };
	inline bool isRenderingTransparent() override { return false; };

	void renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) override;
	void renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) override;
	void renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& terrain)  override;
	void renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass)  override;

private:
	// properties
	OtGbuffer& gbuffer;
	OtFrameBuffer& framebuffer;

	OtUniformMat4 invViewProjUniform{"u_invViewProjUniform", 1};
	OtShaderProgram opaqueProgram{"OtDeferredVS", "OtDeferredPbrFS"};
	OtShaderProgram instancedOpaqueProgram{"OtDeferredInstancingVS", "OtDeferredPbrFS"};
	OtShaderProgram animatedOpaqueProgram{"OtDeferredAnimatedVS", "OtDeferredPbrFS"};
	OtShaderProgram terrainProgram{"OtTerrainVS", "OtTerrainFS"};
	OtShaderProgram grassProgram{"OtGrassVS", "OtGrassFS"};

	OtShaderProgram directionalLightProgram{"OtDeferredLightingVS", "OtDeferredLightingFS"};
	OtShaderProgram pointLightProgram{"OtPointLightsVS", "OtPointLightsFS"};
};
