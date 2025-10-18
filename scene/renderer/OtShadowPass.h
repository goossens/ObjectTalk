//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCascadedShadowMap.h"
#include "OtShaderProgram.h"

#include "OtSceneRendererContext.h"
#include "OtSceneRenderEntitiesPass.h"


//
//	OtShadowPass
//

class OtShadowPass : public OtSceneRenderEntitiesPass {
public:
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
	OtShaderProgram opaqueProgram{"OtShadowVS", "OtShadowOpaqueFS"};
	OtShaderProgram instancedOpaqueProgram{"OtShadowInstancingVS", "OtShadowOpaqueFS"};
	OtShaderProgram animatedOpaqueProgram{"OtShadowAnimatedVS", "OtShadowOpaqueFS"};
	OtShaderProgram transparentProgram{"OtShadowVS", "OtShadowTransparentFS"};
	OtShaderProgram instancedTransparentProgram{"OtShadowInstancingVS", "OtShadowTransparentFS"};
	OtShaderProgram terrainProgram{"OtTerrainSimpleVS", "OtShadowOpaqueFS"};
	OtShaderProgram grassProgram{"OtGrassSimpleVS", "OtShadowOpaqueFS"};
};
