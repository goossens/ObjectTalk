//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	// get the water component
	auto& water = ctx.scene->getComponent<OtWaterComponent>(ctx.waterEntity);

	// setup the refraction camera
	OtCamera refractionCamera{
		ctx.camera.width / 4, ctx.camera.height / 4,
		ctx.camera.nearPlane, ctx.camera.farPlane, ctx.camera.fov,
		ctx.camera.cameraPosition, ctx.camera.viewMatrix};

	// setup the renderer for the refraction
	OtSceneRendererContext refractionContext{
		refractionCamera,
		reflectionRenderingBuffer, refractionCompositeBuffer, refractionBuffer,
		ctx.scene,
		glm::vec4(0.0f, -1.0f, 0.0f, water.level + 1.0f), false};

	// render the scene
	renderReflectionRefractionScene(refractionContext);

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

	OtCamera reflectionCamera{
		ctx.camera.width / 4, ctx.camera.height / 4,
		ctx.camera.nearPlane, ctx.camera.farPlane, ctx.camera.fov,
		reflectionCameraPos, reflectionViewMatrix};

	OtSceneRendererContext reflectionContext{
		reflectionCamera,
		reflectionRenderingBuffer, reflectionCompositeBuffer, reflectionBuffer,
		ctx.scene,
		glm::vec4(0.0f, 1.0f, 0.0f, -(water.level - 1.0f)), false};

	// render the scene
	renderReflectionRefractionScene(reflectionContext);
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
