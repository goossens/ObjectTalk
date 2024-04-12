//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <chrono>
#include <cstdint>

#include "OtFrameBuffer.h"
#include "OtGbuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtUniformVec4.h"

#include "OtBackgroundPass.h"
#include "OtDeferredPass.h"
#include "OtForwardPass.h"
#include "OtSceneRenderPass.h"
#include "OtSkyPass.h"


//
//	OtWaterPass
//

class OtWaterPass : public OtSceneRenderPass {
public:
	// constructor
	OtWaterPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx) {
		// update the buffers
		width = ctx.camera.width;
		height = ctx.camera.height;

		renderingBuffer.update(width, height);
		reflectionBuffer.update(width, height);
		refractionBuffer.update(width, height);

		// render the three water passes
		auto& water = ctx.scene->getComponent<OtWaterComponent>(ctx.waterEntity);
		renderReflection(ctx, water);
		renderRefraction(ctx, water);
		renderWater(ctx, water);
	}

	// render the water's reflection
	void renderReflection(OtSceneRendererContext& ctx, OtWaterComponent& water) {
		// setup the reflection camera
		glm::vec3 reflectionCameraPos = ctx.camera.cameraPosition;
		reflectionCameraPos.y = water.level - (reflectionCameraPos.y - water.level);

		// determine new view matrix
		// see http://khayyam.kaplinski.com/2011/09/reflective-water-with-glsl-part-i.html
		// and http://bcnine.com/articles/water/water.md.html
		static const float flip[16] = { // these must be in column-wise order for glm library
			1.0f,  0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f,  0.0f, 1.0f, 0.0f,
			0.0f,  0.0f, 0.0f, 1.0f
		};

		static float reflection[16] = { // these must be in column-wise order for glm library
			1.0f,  0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f,  0.0f, 1.0f, 0.0f,
			0.0f,  0.0f, 0.0f, 1.0f
		};

		reflection[13] = 2.0f * water.level;

		glm::mat4 sceneCameraMatrix = glm::inverse(ctx.camera.viewMatrix);
		glm::mat4 reflectionCameraMatrix = glm::make_mat4(reflection) * sceneCameraMatrix * glm::make_mat4(flip);
		glm::mat4 reflectionViewMatrix = glm::inverse(reflectionCameraMatrix);

		// setup the reflection camera
		OtCamera reflectionCamera{width, height, reflectionCameraPos, ctx.camera.projectionMatrix, reflectionViewMatrix};

		// setup the renderer for the reflection
		OtSceneRendererContext reflectionContext{ctx};
		reflectionContext.camera = reflectionCamera;
		reflectionContext.clippingPlane = glm::vec4(0.0f, 1.0f, 0.0f, -(water.level - 1.0f));

		// render the scene
		backgroundReflectionPass.render(ctx);
		if (ctx.hasSkyEntities) { skyReflectionPass.render(reflectionContext); }
		if (ctx.hasOpaqueEntities) { deferredReflectionPass.render(reflectionContext); }
		if (ctx.hasTransparentEntities) { forwardReflectionPass.render(reflectionContext); }
	}

	// render the water's refraction
	void renderRefraction(OtSceneRendererContext& ctx, OtWaterComponent& water) {
		// setup the refraction camera
		OtCamera refractionCamera{width, height, ctx.camera.cameraPosition, ctx.camera.projectionMatrix, ctx.camera.viewMatrix};

		// setup the renderer for the refraction
		OtSceneRendererContext refractionContext{ctx};
		refractionContext.camera = refractionCamera;
		refractionContext.clippingPlane = glm::vec4(0.0f, -1.0f, 0.0f, water.level + 1.0f);

		// render the scene
		backgroundRefractionPass.render(ctx);
		if (ctx.hasSkyEntities) { skyRefractionPass.render(refractionContext); }
		if (ctx.hasOpaqueEntities) { deferredRefractionPass.render(refractionContext); }
		if (ctx.hasTransparentEntities) { forwardRefractionPass.render(refractionContext); }
	}

	// render the actual water
	void renderWater(OtSceneRendererContext& ctx, OtWaterComponent& water) {
		// setup the rendering pass
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(framebuffer);
		pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);

		// send out geometry
		static glm::vec3 vertices[] = {
			glm::vec3{-1.0, -1.0, 0.0},
			glm::vec3{1.0, -1.0, 0.0},
			glm::vec3{1.0, 1.0, 0.0},
			glm::vec3{-1.0, 1.0, 0.0}
		};

		OtTransientVertexBuffer vertexBuffer;
		vertexBuffer.submit(vertices, 4, OtVertexPos::getLayout());

		static uint32_t indices[] = {0, 1, 2, 2, 3, 0};
		OtTransientIndexBuffer indexBuffer;
		indexBuffer.submit(indices, 6);

		// determine time
		using namespace std::chrono;
		uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		static uint64_t start = 0;

		if (start == 0) {
			start = now;
		}

		float time = float(double(now - start) / 1000.0) * 0.1f * water.speed;

		// get maximum distance in clip space
		glm::vec4 farPoint = ctx.camera.projectionMatrix * glm::vec4(0.0, water.level, -water.distance, 1.0);
		float distance = farPoint.z / farPoint.w;

		// submit uniforms
		waterUniforms.setValue(0, water.level, distance, 0.0f, 0.0f);
		waterUniforms.setValue(1, water.scale, water.normals.isReady() ? water.normals->getTexture().getWidth() : 1, time, 0.0f);
		waterUniforms.setValue(2, water.metallic, water.roughness, water.ao, water.reflectivity);
		waterUniforms.setValue(3, water.color, float(water.useRefractance));
		waterUniforms.submit();

		submitLightingUniforms(ctx);
		submitShadowUniforms(ctx);

		// bind the textures
		submitTextureSampler(normalmapSampler, 0, water.normals);
		reflectionBuffer.bindColorTexture(reflectionSampler, 1);
		refractionBuffer.bindColorTexture(refractionSampler, 2);
		refractionBuffer.bindDepthTexture(refractionDepthSampler, 3);

		// run the waterProgram
		waterProgram.setState(
			OtStateWriteRgb |
			OtStateWriteA |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateBlendAlpha);

		pass.runShaderProgram(waterProgram);
	}

private:
	// give the debugger access to the inner circle
	friend class OtSceneRendererDebug;

	// properties
	OtFrameBuffer& framebuffer;

	OtGbuffer renderingBuffer;
	OtFrameBuffer reflectionBuffer{OtTexture::rgbaFloat16Texture, OtTexture::dFloatTexture};
	OtFrameBuffer refractionBuffer{OtTexture::rgbaFloat16Texture, OtTexture::dFloatTexture};

	OtUniformVec4 waterUniforms{"u_water", 4};

	OtSampler normalmapSampler{"s_normalmapTexture"};
	OtSampler reflectionSampler{"s_reflectionTexture"};
	OtSampler refractionSampler{"s_refractionTexture"};
	OtSampler refractionDepthSampler{"s_refractionDepthTexture"};

	OtShaderProgram waterProgram{"OtWaterVS", "OtWaterFS"};

	OtBackgroundPass backgroundReflectionPass{reflectionBuffer};
	OtDeferredPass deferredReflectionPass{renderingBuffer, reflectionBuffer};
	OtSkyPass skyReflectionPass{reflectionBuffer};
	OtForwardPass forwardReflectionPass{reflectionBuffer};

	OtBackgroundPass backgroundRefractionPass{refractionBuffer};
	OtDeferredPass deferredRefractionPass{renderingBuffer, refractionBuffer};
	OtSkyPass skyRefractionPass{refractionBuffer};
	OtForwardPass forwardRefractionPass{refractionBuffer};

	int width;
	int height;
};
