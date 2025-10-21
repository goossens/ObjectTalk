//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <unordered_map>

#include "OtFrameBuffer.h"
#include "OtReadBackBuffer.h"
#include "OtShaderProgram.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtPickingPass
//

class OtPickingPass : public OtSceneRenderEntitiesPass {
public:
	// render the pass
	void render(OtSceneRendererContext& ctx, glm::vec2 ndc, std::function<void(OtEntity)> callback);

	// see if we are currently "picking"
	inline bool isPicking() { return picking; }

protected:
	// methods that must be overriden by subclasses (when required)
	bool isRenderingOpaque() override { return true; };
	bool isRenderingTransparent() override { return true; };

	void renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) override;
	void renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) override;
	void renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& terrain)  override;
	void renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass)  override;
	void renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) override;

private:
	// properties
	OtFrameBuffer idBuffer{OtTexture::r8Texture, OtTexture::dFloatTexture};
	OtReadBackBuffer idReadback;
	static constexpr int bufferSize = 8;
	bool picking = false;

	std::unordered_map<int, OtEntity> entityMap;
	int nextID = 1;

	OtShaderProgram opaqueProgram{"OtPickingVS", "OtPickingOpaqueFS"};
	OtShaderProgram instancedOpaqueProgram{"OtPickingInstancingVS", "OtPickingOpaqueFS"};
	OtShaderProgram animatedOpaqueProgram{"OtPickingAnimatedVS", "OtPickingOpaqueFS"};
	OtShaderProgram transparentProgram{"OtPickingVS", "OtPickingTransparentFS"};
	OtShaderProgram instancedTransparentProgram{"OtPickingInstancingVS", "OtPickingTransparentFS"};
	OtShaderProgram terrainProgram{"OtTerrainSimpleVS", "OtPickingOpaqueFS"};
	OtShaderProgram grassProgram{"OtGrassSimpleVS", "OtPickingOpaqueFS"};
};
