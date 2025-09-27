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
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtHighlightPass
//

class OtHighlightPass : public OtSceneRenderEntitiesPass {
public:
	// constructor
	OtHighlightPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx, OtEntity entity) {
		// see if we have a "highlightable" entity
		if (isHighlightable(ctx.scene, entity)) {
			// update framebuffer size
			selectedBuffer.update(ctx.camera.width, ctx.camera.height);

			// run both passes
			renderSelectedPass(ctx, entity);
			renderHighlightPass(ctx);
		}
	}

private:
	// render entities as opaque blobs
	void renderSelectedPass(OtSceneRendererContext& ctx, OtEntity entity) {
		// setup pass to render entities as opaque blobs
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(selectedBuffer);
		pass.setClear(true);
		pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);
		pass.touch();

		// highlight entity (and its children)
		renderHighlight(ctx, pass, entity);
	}

	// render the outline of the selected entity(s)
	void renderHighlightPass(OtSceneRendererContext& ctx) {
		// render the outline of the entity
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(framebuffer);
		pass.submitQuad(ctx.camera.width, ctx.camera.height);

		outlineUniforms.setValue(0, 1.0f / ctx.camera.width, 1.0f / ctx.camera.height, 0.0f, 0.0f);
		outlineUniforms.submit();

		selectedBuffer.bindColorTexture(selectedSampler, 0);
		pass.setState(OtStateWriteRgb | OtStateWriteA | OtStateBlendAlpha);
		pass.runShaderProgram(outlineProgram);
	}

	// recursive method to render an entity and it's children
	void renderHighlight(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity) {
		// render entity and its children
		renderEntity(ctx, pass, entity);
		OtEntity child = ctx.scene->getFirstChild(entity);

		while (ctx.scene->isValidEntity(child)) {
			renderHighlight(ctx, pass, child);
			child = ctx.scene->getNextSibling(child);
		}
	}

	// see if specified entity or one of the children is "highlightable"
	bool isHighlightable(OtScene* scene, OtEntity entity) {
		bool highlightable = false;

		if (scene->isValidEntity(entity)) {
			highlightable |= scene->hasComponent<OtGeometryComponent>(entity);
			highlightable |= scene->hasComponent<OtModelComponent>(entity);
			highlightable |= scene->hasComponent<OtTerrainComponent>(entity);

			OtEntity child = scene->getFirstChild(entity);

			while (scene->isValidEntity(child)) {
				highlightable |= isHighlightable(scene, child);
				child = scene->getNextSibling(child);
			}
		}

		return highlightable;
	}

protected:
	// methods that must be overriden by subclasses (when required)
	bool isRenderingOpaque() override { return true; };
	bool isRenderingTransparent() override { return true; };

	OtShaderProgram* getOpaqueProgram() override { return &opaqueProgram; }
	OtShaderProgram* getInstancedOpaqueProgram() override { return &instancedOpaqueProgram; }
	OtShaderProgram* getAnimatedOpaqueProgram() override { return &animatedOpaqueProgram; }
	OtShaderProgram* getTransparentProgram() override { return &transparentProgram; }
	OtShaderProgram* getInstancedTransparentProgram() override { return &instancedTransparentProgram; }
	OtShaderProgram* getTerrainProgram() override { return &terrainProgram; }
	OtShaderProgram* getGrassProgram() override { return &grassProgram; }

	inline uint64_t getNormalState() override { return OtStateWriteRgb; }
	inline uint64_t getCullBackState() override { return OtStateWriteRgb; };
	inline uint64_t getWireframeState() override { return OtStateWriteRgb; };

	inline void submitUniforms(OtSceneRendererContext& /* ctx */, Scope& scope) override {
		if (scope.isTransparent) { submitAlbedoUniforms(*scope.material); }
	}

private:
	// properties
	OtFrameBuffer& framebuffer;
	OtFrameBuffer selectedBuffer{OtTexture::r8Texture};

	OtUniformVec4 outlineUniforms{"u_outline", 1};

	OtSampler selectedSampler{"s_selectedTexture", OtTexture::pointSampling | OtTexture::clampSampling};

	OtShaderProgram opaqueProgram{"OtSelectVS", "OtSelectOpaqueFS"};
	OtShaderProgram instancedOpaqueProgram{"OtSelectInstancingVS", "OtSelectOpaqueFS"};
	OtShaderProgram animatedOpaqueProgram{"OtSelectAnimatedVS", "OtSelectOpaqueFS"};
	OtShaderProgram transparentProgram{"OtSelectVS", "OtSelectTransparentFS"};
	OtShaderProgram instancedTransparentProgram{"OtSelectInstancingVS", "OtSelectTransparentFS"};
	OtShaderProgram terrainProgram{"OtTerrainSimpleVS", "OtSelectOpaqueFS"};
	OtShaderProgram grassProgram{"OtGrassSimpleVS", "OtSelectOpaqueFS"};
	OtShaderProgram outlineProgram{"OtOutlineVS", "OtOutlineFS"};
};
