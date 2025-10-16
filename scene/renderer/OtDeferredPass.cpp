//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "OtInstanceDataBuffer.h"
#include "OtTransientIndexBuffer.h"
#include "OtTransientVertexBuffer.h"
#include "OtVertex.h"

#include "OtDeferredPass.h"



//
//	OtDeferredPass::render
//

void OtDeferredPass::render(OtSceneRendererContext& ctx) {
	// run the geometry and lighting passes
	renderGeometry(ctx);
	renderDirectionalLight(ctx);

	if (ctx.hasPointLighting) {
		renderPointLights(ctx);
	}
}


//
//	OtDeferredPass::renderGeometry
//

void OtDeferredPass::renderGeometry(OtSceneRendererContext& ctx) {
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


//
//	OtDeferredPass::renderDirectionalLight
//

void OtDeferredPass::renderDirectionalLight(OtSceneRendererContext& ctx) {
	// setup pass
	OtPass pass;
	pass.setFrameBuffer(framebuffer);
	pass.submitQuad(ctx.camera.width, ctx.camera.height);

	// submit the uniforms
	ctx.submitLightingUniforms();
	ctx.submitShadowUniforms();

	ctx.invViewProjUniform.set(0, glm::inverse(ctx.camera.viewProjectionMatrix));
	ctx.invViewProjUniform.submit();

	// bind all textures
	gbuffer.bindAlbedoTexture(ctx.lightingAlbedoSampler, 0);
	gbuffer.bindNormalTexture(ctx.lightingNormalSampler, 1);
	gbuffer.bindPbrTexture(ctx.lightingPbrSampler, 2);
	gbuffer.bindEmissiveTexture(ctx.lightingEmissiveSampler, 3);
	gbuffer.bindDepthTexture(ctx.lightingDepthSampler, 4);

	// run the program
	pass.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateWriteZ |
		OtStateDepthTestLess);

	pass.runShaderProgram(directionalLightProgram);
}


//
//	OtDeferredPass::renderPointLights
//

void OtDeferredPass::renderPointLights(OtSceneRendererContext& ctx) {
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
	ctx.submitLightingUniforms();

	// bind all textures
	gbuffer.bindAlbedoTexture(ctx.lightingAlbedoSampler, 0);
	gbuffer.bindNormalTexture(ctx.lightingNormalSampler, 1);
	gbuffer.bindPbrTexture(ctx.lightingPbrSampler, 2);
	gbuffer.bindDepthTexture(ctx.lightingDepthSampler, 3);

	// run the program
	pass.setState(
		OtStateWriteRgb |
		OtStateDepthTestGreaterEqual |
		OtStateCullCcw |
		OtStateBlendAdd);

	pass.runShaderProgram(pointLightProgram);
}


//
//	OtDeferredPass::renderOpaqueGeometry
//

void OtDeferredPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& geometry, bool instancing) {
	ctx.submitClippingUniforms();

	renderOpaqueGeometryHelper(
		ctx,
		entity,
		geometry,
		OtStateWriteRgb | OtStateWriteA | OtStateWriteZ | OtStateDepthTestLess | OtStateLines,
		OtStateWriteRgb | OtStateWriteA | OtStateWriteZ | OtStateDepthTestLess | (geometry.cullBack ? OtStateCullCw : 0),
		false,
		instancing ? instancedOpaqueProgram : opaqueProgram);
}


//
//	OtDeferredPass::renderOpaqueModel
//

void OtDeferredPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtEntity entity, OtModelComponent& model, bool instancing) {
	ctx.submitClippingUniforms();

	renderOpaqueModelHelper(
		ctx,
		entity,
		model,
		OtStateWriteRgb | OtStateWriteA | OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
		false,
		animatedOpaqueProgram,
		instancing ? instancedOpaqueProgram : opaqueProgram);
}


//
//	OtDeferredPass::renderTerrain
//

void OtDeferredPass::renderTerrain(OtSceneRendererContext& ctx, [[maybe_unused]] OtEntity entity, OtTerrainComponent& terrain) {
	renderTerrainHelper(
		ctx,
		terrain,
		OtStateWriteRgb | OtStateWriteA | OtStateWriteZ | OtStateDepthTestLess | OtStateLines,
		OtStateWriteRgb | OtStateWriteA | OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
		terrainProgram);
}


//
//	OtDeferredPass::renderGrass
//

void OtDeferredPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	renderGrassHelper(
		ctx,
		entity,
		grass,
		OtStateWriteRgb | OtStateWriteA | OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
		grassProgram);
}
