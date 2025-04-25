//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/gtx/matrix_decompose.hpp"

#include "glm/gtx/quaternion.hpp"

#include "OtFrameBuffer.h"
#include "OtGbuffer.h"
#include "OtInstanceDataBuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtUniformMat4.h"
#include "OtVertex.h"

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
		renderDirectionalLight(ctx);

		if (ctx.hasPointLighting) {
			renderPointLights(ctx);
		}
	}

	// render all opaque geometry
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

	// run direct lighting calculations
	void renderDirectionalLight(OtSceneRendererContext& ctx) {
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
		directionalLightProgram.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess);

		pass.runShaderProgram(directionalLightProgram);
	}

	// render point lights
	void renderPointLights(OtSceneRendererContext& ctx) {
		// setup pass
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(framebuffer);
		pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);

		// send out geometry
		static constexpr float LEFT = -1.0f, RIGHT = 1.0f, BOTTOM = -1.0f, TOP = 1.0f, FRONT = 1.0f, BACK = -1.0f;

		static glm::vec3 vertices[] = {
			glm::vec3{LEFT, BOTTOM, FRONT}, glm::vec3{ RIGHT, BOTTOM, FRONT}, glm::vec3{LEFT, TOP, FRONT}, glm::vec3{RIGHT, TOP, FRONT},
			glm::vec3{LEFT, BOTTOM, BACK},  glm::vec3{ RIGHT, BOTTOM, BACK},  glm::vec3{LEFT, TOP, BACK},  glm::vec3{RIGHT, TOP, BACK},
		};

		OtTransientVertexBuffer tvb;
		tvb.submit(vertices, sizeof(vertices) / sizeof(*vertices), OtVertexPos::getLayout());

		static uint32_t indices[] = {
			0, 1, 3, 3, 2, 0, // front
			5, 4, 6, 6, 7, 5, // back
			4, 0, 2, 2, 6, 4, // left
			1, 5, 7, 7, 3, 1, // right
			2, 3, 7, 7, 6, 2, // top
			4, 5, 1, 1, 0, 4  // bottom
		};

		OtTransientIndexBuffer tib;
		tib.submit(indices, sizeof(indices) / sizeof(*indices));

		// collect all point light data
		struct Light {
			Light(glm::vec3 p, float r, glm::vec3 c) : position(p), radius(r), color(c) {}
			glm::vec3 position;
			float radius;
			glm::vec3 color;
			float unused;
		};

		std::vector<Light> lights;

		for (auto&& [entity, component] : ctx.scene->view<OtPointLightComponent>().each()) {
			glm::mat4 transform = ctx.scene->getGlobalTransform(entity);

			glm::vec3 translate;
			glm::quat rotate;
			glm::vec3 scale;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(transform, scale, rotate, translate, skew, perspective);

			lights.emplace_back(scale * component.offset + translate, component.radius, component.color);
		}

		// send out instance data
		OtInstanceDataBuffer idb;
		idb.submit(lights.data(), lights.size(), sizeof(Light));

		// submit the uniforms
		submitLightingUniforms(ctx);

		// bind all textures
		gbuffer.bindAlbedoTexture(lightingAlbedoSampler, 0);
		gbuffer.bindNormalTexture(lightingNormalSampler, 1);
		gbuffer.bindPbrTexture(lightingPbrSampler, 2);
		gbuffer.bindDepthTexture(lightingDepthSampler, 3);

		// run the program
		pointLightProgram.setState(
			OtStateWriteRgb |
			OtStateDepthTestGreaterEqual |
			OtStateCullCcw |
			OtStateBlendAdd);

		pass.runShaderProgram(pointLightProgram);
	}

protected:
	// methods that must be overriden by subclasses (when required)
	bool isRenderingOpaque() override { return true; };
	bool isRenderingTransparent() override { return false; };

	OtShaderProgram* getOpaqueProgram() override { return &opaqueProgram; }
	OtShaderProgram* getInstancedOpaqueProgram() override { return &instancedOpaqueProgram; }
	OtShaderProgram* getAnimatedOpaqueProgram() override { return &animatedOpaqueProgram; }
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

	OtSampler lightingAlbedoSampler{"s_lightingAlbedoTexture"};
	OtSampler lightingNormalSampler{"s_lightingNormalTexture"};
	OtSampler lightingPbrSampler{"s_lightingPbrTexture"};
	OtSampler lightingEmissiveSampler{"s_lightingEmissiveTexture"};
	OtSampler lightingDepthSampler{"s_lightingDepthTexture"};

	OtShaderProgram opaqueProgram{"OtDeferredVS", "OtDeferredPbrFS"};
	OtShaderProgram instancedOpaqueProgram{"OtDeferredInstancingVS", "OtDeferredPbrFS"};
	OtShaderProgram animatedOpaqueProgram{"OtDeferredAnimatedVS", "OtDeferredPbrFS"};
	OtShaderProgram terrainProgram{"OtTerrainVS", "OtTerrainFS"};

	OtShaderProgram directionalLightProgram{"OtDeferredLightingVS", "OtDeferredLightingFS"};
	OtShaderProgram pointLightProgram{"OtPointLightsVS", "OtPointLightsFS"};
};
