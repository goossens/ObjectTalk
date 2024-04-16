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
#include "OtGbuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtUniformMat4.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtDeferredPass
//

class OtDeferredPass : public OtSceneRenderEntitiesPass {
public:
	// constructor
	OtDeferredPass(OtGbuffer& gb, OtFrameBuffer& fb) : gbuffer(gb), framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx) {
		// run the geometry and lighting passes
		renderGeometry(ctx);
		renderLight(ctx);
	}

	void renderGeometry(OtSceneRendererContext& ctx) {
		// setup the rendering pass
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(gbuffer);
		pass.setClear(true, true, glm::vec4(0.0f));
		pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);
		pass.touch();

		// render all entities
		renderEntities(ctx, pass);
	}

	void renderLight(OtSceneRendererContext& ctx) {
		// setup pass
		OtPass pass;
		pass.setFrameBuffer(framebuffer);
		pass.submitQuad(ctx.camera.width, ctx.camera.height);

		// submit the uniforms
		submitLightingUniforms(ctx);
		submitShadowUniforms(ctx);

		invViewProjUniform.set(0, glm::inverse(ctx.camera.viewProjectionMatrix));
		invViewProjUniform.submit();

		// bind all textures
		gbuffer.bindAlbedoTexture(lightingAlbedoSampler, 0);
		gbuffer.bindNormalTexture(lightingNormalSampler, 1);
		gbuffer.bindPbrTexture(lightingPbrSampler, 2);
		gbuffer.bindEmissiveTexture(lightingEmissiveSampler, 3);
		gbuffer.bindDepthTexture(lightingDepthSampler, 4);

		// run the program
		program.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess);

		pass.runShaderProgram(program);
	}

protected:
	// methods that must be overriden by subclasses (when required)
	bool isRenderingOpaque() override { return true; };
	bool isRenderingTransparent() override { return false; };

	OtShaderProgram* getOpaqueProgram() override { return &opaqueProgram; }
	OtShaderProgram* getInstancedOpaqueProgram() override { return &instancedOpaqueProgram; }
	OtShaderProgram* getTransparentProgram() override { return nullptr; }
	OtShaderProgram* getInstancedTransparentProgram() override { return nullptr; }
	OtShaderProgram* getTerrainProgram() override { return &terrainProgram; }

	inline uint64_t getNormalState() override {
		return
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess;
	}

	inline uint64_t getCullbackState() override {
		return
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw;
	};

	inline uint64_t getWireframeState() override {
		return
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateLines;
	};

	inline void submitUniforms(OtSceneRendererContext& ctx, Scope& scope) override {
		submitClippingUniforms(ctx);

		if (scope.isTerrain) {
			submitTerrainUniforms(*scope.terrain);

		} else {
			submitMaterialUniforms(*scope.material);
		}
	}

private:
	// properties
	OtGbuffer& gbuffer;
	OtFrameBuffer& framebuffer;

	OtUniformMat4 invViewProjUniform{"u_invViewProjUniform", 1};

	OtSampler lightingAlbedoSampler{"s_lightingAlbedoTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler lightingNormalSampler{"s_lightingNormalTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler lightingPbrSampler{"s_lightingPbrTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler lightingEmissiveSampler{"s_lightingEmissiveTexture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtSampler lightingDepthSampler{"s_lightingDepthTexture", OtTexture::pointSampling | OtTexture::clampSampling};

	OtShaderProgram opaqueProgram{"OtDeferredVS", "OtDeferredPbrFS"};
	OtShaderProgram instancedOpaqueProgram{"OtDeferredInstancingVS", "OtDeferredPbrFS"};
	OtShaderProgram terrainProgram{"OtTerrainVS", "OtTerrainFS"};

	OtShaderProgram program{"OtDeferredLightingVS", "OtDeferredLightingFS"};
};
