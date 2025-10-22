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


//
//	OtHighlightPass
//

class OtHighlightPass : public OtSceneRenderEntitiesPass {
public:
	// render the pass
	void render(OtSceneRendererContext& ctx, OtFrameBuffer* framebuffer, OtEntity entity);

private:
	// render entities as opaque blobs
	void renderSelectedPass(OtSceneRendererContext& ctx, OtEntity entity);

	// render the outline of the selected entity(s)
	void renderHighlightPass(OtSceneRendererContext& ctx, OtFrameBuffer* framebuffer);

	// recursive method to render an entity and it's children
	void renderHighlight(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity);

	// see if specified entity or one of the children is "highlightable"
	bool isHighlightable(OtScene* scene, OtEntity entity);

protected:
	// methods that must be overriden by subclasses (when required)
	inline bool isRenderingOpaque() override { return true; };
	inline bool isRenderingTransparent() override { return true; };

	void renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) override;
	void renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) override;
	void renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& terrain)  override;
	void renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass)  override;
	void renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) override;

private:
	// properties
	OtFrameBuffer selectedBuffer{OtTexture::r8Texture};

	OtShaderProgram opaqueProgram{"OtSelectVS", "OtSelectOpaqueFS"};
	OtShaderProgram instancedOpaqueProgram{"OtSelectInstancingVS", "OtSelectOpaqueFS"};
	OtShaderProgram animatedOpaqueProgram{"OtSelectAnimatedVS", "OtSelectOpaqueFS"};
	OtShaderProgram transparentProgram{"OtSelectVS", "OtSelectTransparentFS"};
	OtShaderProgram instancedTransparentProgram{"OtSelectInstancingVS", "OtSelectTransparentFS"};
	OtShaderProgram terrainProgram{"OtTerrainSimpleVS", "OtSelectOpaqueFS"};
	OtShaderProgram grassProgram{"OtGrassSimpleVS", "OtSelectOpaqueFS"};
	OtShaderProgram outlineProgram{"OtOutlineVS", "OtOutlineFS"};
};
