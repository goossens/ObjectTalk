//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCascadedShadowMap.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtShadowPass
//

class OtShadowPass : public OtSceneRenderEntitiesPass {
public:
	// render the pass
	void render(OtSceneRendererContext& ctx) {
		// update shadowmaps
		ctx.csm->update(ctx.camera, ctx.directionalLightDirection);

		// render each cascade
		for (size_t i = 0; i < OtCascadedShadowMap::maxCascades; i++) {

			// setup pass to render entities as opaque blobs
			OtPass pass;
			pass.setRectangle(0, 0, ctx.csm->getSize(), ctx.csm->getSize());
			pass.setFrameBuffer(ctx.csm->getFrameBuffer(i));
			pass.setClear(false, true);
			pass.setTransform(ctx.csm->getCamera(i).viewMatrix, ctx.csm->getCamera(i).projectionMatrix);
			pass.touch();

			OtSceneRendererContext sctx{ctx};
			sctx.camera = ctx.csm->getCamera(i);
			sctx.renderingShadow = true;

			// render all entities
			renderEntities(sctx, pass);
		}
	}

protected:
	// methods that must be overriden by subclasses (when required)
	bool isRenderingOpaque() override { return true; };
	bool isRenderingTransparent() override { return true; };

	OtShaderProgram* getOpaqueProgram() override { return &opaqueProgram; }
	OtShaderProgram* getInstancedOpaqueProgram() override { return &instancedOpaqueProgram; }
	OtShaderProgram* getAnimatedOpaqueProgram() override { return &animatedOpaqueProgram; }
	OtShaderProgram* getTransparentProgram() override { return &transparentProgram; }
	OtShaderProgram* getInstancedTransparentProgram() override { return &instancedTransparentProgram; }
	OtShaderProgram* getTerrainProgram() override { return &terrainProgram; }

	inline uint64_t getNormalState() override { return OtStateWriteZ | OtStateDepthTestLess; }
	inline uint64_t getCullBackState() override { return OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw; };
	inline uint64_t getWireframeState() override { return OtStateWriteZ | OtStateDepthTestLess | OtStateLines; };

	inline void submitUniforms(OtSceneRendererContext& /* ctx */, Scope& scope) override {
		if (scope.isTransparent) { submitAlbedoUniforms(*scope.material); }
		if (scope.isTerrain) { submitTerrainUniforms(*scope.terrain); }
	}

private:
	// properties
	OtShaderProgram opaqueProgram{"OtShadowVS", "OtShadowOpaqueFS"};
	OtShaderProgram instancedOpaqueProgram{"OtShadowInstancingVS", "OtShadowOpaqueFS"};
	OtShaderProgram animatedOpaqueProgram{"OtShadowAnimatedVS", "OtShadowOpaqueFS"};
	OtShaderProgram transparentProgram{"OtShadowVS", "OtShadowTransparentFS"};
	OtShaderProgram instancedTransparentProgram{"OtShadowInstancingVS", "OtShadowTransparentFS"};
	OtShaderProgram terrainProgram{"OtTerrainSimpleVS", "OtShadowOpaqueFS"};
};
