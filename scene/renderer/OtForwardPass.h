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
#include "OtShaderProgram.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtForwardPass
//

class OtForwardPass : public OtSceneRenderEntitiesPass {
public:
	// constructor
	OtForwardPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx) {
		// setup the rendering pass
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(framebuffer);
		pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);
		pass.touch();

		// render all entities
		renderEntities(ctx, pass);
	}

protected:
	// methods that must be overriden by subclasses (when required)
	bool isRenderingOpaque() override { return false; };
	bool isRenderingTransparent() override { return true; };

	OtShaderProgram* getOpaqueProgram() override { return nullptr; }
	OtShaderProgram* getInstancedOpaqueProgram() override { return nullptr; }
	OtShaderProgram* getAnimatedOpaqueProgram() override { return nullptr; }
	OtShaderProgram* getTransparentProgram() override { return &transparentProgram; }
	OtShaderProgram* getInstancedTransparentProgram() override { return &instancedTransparentProgram; }
	OtShaderProgram* getTerrainProgram() override { return nullptr; }

	inline uint64_t getNormalState() override {
		return
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateBlendAlpha;
	}

	inline uint64_t getCullbackState() override {
		return
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw |
			OtStateBlendAlpha;
	};

	inline uint64_t getWireframeState() override {
		return
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateLines |
			OtStateBlendAlpha;
	};

	inline void submitUniforms(OtSceneRendererContext& ctx, Scope& scope) override {
		submitClippingUniforms(ctx);
		submitMaterialUniforms(*scope.material);
		submitLightingUniforms(ctx);
		submitShadowUniforms(ctx);
	}

private:
	// properties
	OtFrameBuffer& framebuffer;
	OtShaderProgram transparentProgram{"OtForwardVS", "OtForwardPbrFS"};
	OtShaderProgram instancedTransparentProgram{"OtForwardInstancingVS", "OtForwardPbrFS"};
};
