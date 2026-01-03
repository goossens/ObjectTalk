//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtRenderPass.h"
#include "OtVertex.h"

#include "OtForwardPass.h"
#include "OtShaders.h"


//
//	OtForwardPass::render
//

void OtForwardPass::render(OtSceneRendererContext& ctx) {
	// initialize resources (if required)
	if (!resourcesInitialized) {
		initializeResources();
		resourcesInitialized = true;
	}

	// setup pass
	OtRenderPass pass;
	pass.start(framebuffer);
	ctx.pass = &pass;

	// submit common fragment uniforms
	// uniform slot 0 and sampler slots 0-4 are used for materials
	ctx.setClippingUniforms(1);
	ctx.setLightingUniforms(2, 5);
	ctx.setShadowUniforms(3, 8);

	// render all entities
	renderEntities(ctx);

	// we're done
	pass.end();
}


//
//	OtForwardPass::renderTransparentGeometry
//

void OtForwardPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	renderGeometryHelper(
		ctx,
		grd,
		MaterialSubmission::full,
		cullingPipeline,
		noCullingPipeline,
		linesPipeline,
		instancedCullingPipeline,
		instancedNoCullingPipeline,
		instancedLinesPipeline);
}


//
//	OtForwardPass::initializeResources
//

void OtForwardPass::initializeResources() {
	cullingPipeline.setShaders(OtForwardVert, OtForwardVertSize, OtForwardPbrFrag, OtForwardPbrFragSize);
	cullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	cullingPipeline.setVertexDescription(OtVertex::getDescription());
	cullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	cullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	cullingPipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);

	noCullingPipeline.setShaders(OtForwardVert, OtForwardVertSize, OtForwardPbrFrag, OtForwardPbrFragSize);
	noCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	noCullingPipeline.setVertexDescription(OtVertex::getDescription());
	noCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	noCullingPipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);

	linesPipeline.setShaders(OtForwardVert, OtForwardVertSize, OtForwardPbrFrag, OtForwardPbrFragSize);
	linesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	linesPipeline.setVertexDescription(OtVertex::getDescription());
	linesPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	linesPipeline.setFill(false);

	linesPipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);

	instancedCullingPipeline.setShaders(OtForwardInstancingVert, OtForwardInstancingVertSize, OtForwardPbrFrag, OtForwardPbrFragSize);
	instancedCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	instancedCullingPipeline.setVertexDescription(OtVertex::getDescription());
	instancedCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	instancedCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);
	instancedCullingPipeline.setCulling(OtRenderPipeline::Culling::cw);

	instancedCullingPipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);

	instancedNoCullingPipeline.setShaders(OtForwardInstancingVert, OtForwardInstancingVertSize, OtForwardPbrFrag, OtForwardPbrFragSize);
	instancedNoCullingPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	instancedNoCullingPipeline.setVertexDescription(OtVertex::getDescription());
	instancedNoCullingPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	instancedNoCullingPipeline.setDepthTest(OtRenderPipeline::CompareOperation::less);

	instancedNoCullingPipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);

	instancedLinesPipeline.setShaders(OtForwardInstancingVert, OtForwardInstancingVertSize, OtForwardPbrFrag, OtForwardPbrFragSize);
	instancedLinesPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16d32);
	instancedLinesPipeline.setVertexDescription(OtVertex::getDescription());
	instancedLinesPipeline.setInstanceDescription(OtVertexMatrix::getDescription());
	instancedLinesPipeline.setFill(false);

	instancedLinesPipeline.setBlend(
		OtRenderPipeline::BlendOperation::add,
		OtRenderPipeline::BlendFactor::srcAlpha,
		OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
	);
}
