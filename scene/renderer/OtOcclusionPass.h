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
#include "OtSceneRendererContext.h"


//
//	OtOcclusionPass
//

class OtOcclusionPass : public OtSceneRenderEntitiesPass {
public:
	// constructor
	OtOcclusionPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx);

protected:
	// methods that must be overriden by subclasses (when required)
	bool isRenderingOpaque() override { return true; };
	bool isRenderingTransparent() override { return true; };

	void renderOpaqueGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& geometry) override;
	void renderOpaqueModel(OtSceneRendererContext& ctx, OtEntity entity, OtModelComponent& model) override;
	void renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& terrain)  override;
	void renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass)  override;
	void renderTransparentGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& geometry) override;

private:
	// properties
	OtFrameBuffer& framebuffer;
	OtShaderProgram opaqueProgram{"OtOcclusionVS", "OtOcclusionOpaqueFS"};
	OtShaderProgram instancedOpaqueProgram{"OtOcclusionInstancingVS", "OtOcclusionOpaqueFS"};
	OtShaderProgram animatedOpaqueProgram{"OtOcclusionAnimatedVS", "OtOcclusionOpaqueFS"};
	OtShaderProgram transparentProgram{"OtOcclusionVS", "OtOcclusionTransparentFS"};
	OtShaderProgram instancedTransparentProgram{"OtOcclusionInstancingVS", "OtOcclusionTransparentFS"};
	OtShaderProgram terrainProgram{"OtTerrainSimpleVS", "OtOcclusionOpaqueFS"};
	OtShaderProgram grassProgram{"OtGrassSimpleVS", "OtOcclusionOpaqueFS"};
};
