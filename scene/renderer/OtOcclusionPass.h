//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtOcclusionPass
//

class OtOcclusionPass : public OtSceneRenderEntitiesPass {
public:
	// constructor
	OtOcclusionPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx) {
		// setup the rendering pass
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);
		pass.setFrameBuffer(framebuffer);
		pass.setClear(false, false);

		// render all entities
		renderEntities(ctx, pass);
	}

protected:
	// methods that must be overriden by subclasses (when required)
	bool isRenderingOpaque() override { return true; };
	bool isRenderingTransparent() override { return true; };

	OtShaderProgram* getOpaqueProgram() override { return &opaqueProgram; }
	OtShaderProgram* getInstancedOpaqueProgram() override { return &instancedOpaqueProgram; }
	OtShaderProgram* getTransparentProgram() override { return &transparentProgram; }
	OtShaderProgram* getInstancedTransparentProgram() override { return &instancedTransparentProgram; }
	OtShaderProgram* getTerrainProgram() override { return &terrainProgram; }

	inline uint64_t getNormalState() override { return OtStateWriteRgb; }
	inline uint64_t getCullbackState() override { return OtStateWriteRgb | OtStateCullCw; };
	inline uint64_t getWireframeState() override { return OtStateWriteRgb | OtStateLines; };

	inline void submitUniforms(OtSceneRendererContext& ctx, Scope& scope) override {
		if (scope.isTransparent) { submitAlbedoUniforms(*scope.material); }
		if (scope.isTerrain) { submitTerrainUniforms(*scope.terrain); }
	}

private:
	// properties
	OtFrameBuffer& framebuffer;
	OtShaderProgram opaqueProgram{"OtOcclusionVS", "OtOcclusionOpaqueFS"};
	OtShaderProgram instancedOpaqueProgram{"OtOcclusionInstancingVS", "OtOcclusionOpaqueFS"};
	OtShaderProgram transparentProgram{"OtOcclusionVS", "OtOcclusionTransparentFS"};
	OtShaderProgram instancedTransparentProgram{"OtOcclusionInstancingVS", "OtOcclusionTransparentFS"};
	OtShaderProgram terrainProgram{"OtTerrainSimpleVS", "OtOcclusionOpaqueFS"};
};
