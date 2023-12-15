//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtc/type_ptr.hpp"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderReflectionPass
//

void OtSceneRenderer::renderReflectionPass(OtSceneRendererContext& ctx) {
	// for now, we only support one water entity (last one in scene wins)
	OtEntity waterEntity;

	for (auto&& [entity, component] : ctx.scene->view<OtWaterComponent>().each()) {
		waterEntity = entity;
	}

	// get the water component
	auto& water = ctx.scene->getComponent<OtWaterComponent>(waterEntity);

	// setup the renderer for the refraction
	OtSceneRendererContext refractionContext{
		ctx.width / 4, ctx.height / 4,
		ctx.cameraPosition, ctx.viewMatrix, ctx.projectionMatrix,
		reflectionRenderingBuffer, reflectionCompositeBuffer, refractionBuffer,
		ctx.scene,
		glm::vec4(0.0f, -1.0f, 0.0f, water.level + 0.1), false};

	// render the scene
	renderReflectionRefractionScene(refractionContext);

	// setup the renderer for the reflection
	// determine position of reflection camera
	glm::vec3 reflectionCameraPos = ctx.cameraPosition;
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

	glm::mat4 sceneCameraMatrix = glm::inverse(ctx.viewMatrix);
	glm::mat4 reflectionCameraMatrix = glm::make_mat4(reflection) * sceneCameraMatrix * glm::make_mat4(flip);
	glm::mat4 reflectionViewMatrix = glm::inverse(reflectionCameraMatrix);

	OtSceneRendererContext reflectionContext{
		ctx.width / 4, ctx.height / 4,
		reflectionCameraPos, reflectionViewMatrix, ctx.projectionMatrix,
		reflectionRenderingBuffer, reflectionCompositeBuffer, reflectionBuffer,
		ctx.scene,
		glm::vec4(0.0f, 1.0f, 0.0f, -(water.level - 0.1f)), false};

	// render the scene
	renderReflectionRefractionScene(reflectionContext);

#if OT_DEBUG
	water.reflectionTextureIndex = reflectionBuffer.getColorTextureIndex();
	water.refractionTextureIndex = refractionBuffer.getColorTextureIndex();
#endif
}


//
//	OtSceneRenderer::renderReflectionRefractionScene
//

void OtSceneRenderer::renderReflectionRefractionScene(OtSceneRendererContext& ctx) {
	// see if we need to do some deferred rendering into a gbuffer?
	if (ctx.hasOpaqueEntities) {
		renderDeferredGeometryPass(ctx);
	}

	// start rendering to reflection buffer
	renderBackgroundPass(ctx);

	if (ctx.hasSkyEntities) {
		renderSkyPass(ctx);
	}

	if (ctx.hasOpaqueEntities) {
		renderDeferredLightingPass(ctx);
	}

	if (ctx.hasTransparentEntities) {
		renderForwardGeometryPass(ctx);
	}

	// post process buffer
	renderPostProcessingPass(ctx, false);
}
