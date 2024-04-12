//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

#include "OtSceneRenderPass.h"


//
//	OtHighlightPass
//

class OtHighlightPass : public OtSceneRenderPass {
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

	// render entities as opaque blobs
	void renderSelectedPass(OtSceneRendererContext& ctx, OtEntity entity) {
		// setup pass to render entities as opaque blobs
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(selectedBuffer);
		pass.setClear(true, false);
		pass.setTransform(ctx.camera.viewMatrix, ctx.camera.projectionMatrix);

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
		outlineProgram.setState(OtStateWriteRgb | OtStateWriteA | OtStateBlendAlpha);
		pass.runShaderProgram(outlineProgram);
	}

	// recursive method to render an entity and it's children
	void renderHighlight(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity) {
		// only render if all components are available
		if (ctx.scene->hasComponent<OtGeometryComponent>(entity)) {
			// render geometry
			auto& asset = ctx.scene->getComponent<OtGeometryComponent>(entity).asset;

			if (asset.isReady()) {
				auto& geometry = asset->getGeometry();
				geometry.submitTriangles();

				// get camera frustum and geometry AABB
				auto& frustum = ctx.camera.frustum;
				auto& aabb =  geometry.getAABB();

				// is this a case of instancing?
				if (ctx.scene->hasComponent<OtInstancingComponent>(entity)) {
					// only render instances if we have a valid asset and at least one instance is visible
					auto& instancing = ctx.scene->getComponent<OtInstancingComponent>(entity);

					if (!instancing.asset.isNull() && instancing.asset->getInstances().submit(frustum, aabb)) {
						selectInstancingProgram.setTransform(ctx.scene->getGlobalTransform(entity));
						selectInstancingProgram.setState(OtStateWriteRgb);
						pass.runShaderProgram(selectInstancingProgram);
					}

				} else {
					// see if geometry is visible
					if (frustum.isVisibleAABB(aabb.transform(ctx.scene->getGlobalTransform(entity)))) {
						selectProgram.setTransform(ctx.scene->getGlobalTransform(entity));
						selectProgram.setState(OtStateWriteRgb);
						pass.runShaderProgram(selectProgram);
					}
				}
			}

		} else if (ctx.scene->hasComponent<OtModelComponent>(entity)) {
			// render model
			auto& model = ctx.scene->getComponent<OtModelComponent>(entity).model;

			if (model.isReady()) {
				for (auto& mesh : model->getMeshes()) {
					mesh.submitTriangles();
					selectProgram.setTransform(ctx.scene->getGlobalTransform(entity));
					selectProgram.setState(OtStateWriteRgb);
					pass.runShaderProgram(selectProgram);
				}
			}
		}

		// also render all the children
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
			highlightable |= scene->hasComponent<OtGeometryComponent>(entity) && scene->hasComponent<OtMaterialComponent>(entity);
			highlightable |= scene->hasComponent<OtModelComponent>(entity);

			OtEntity child = scene->getFirstChild(entity);

			while (scene->isValidEntity(child)) {
				highlightable |= isHighlightable(scene, child);
				child = scene->getNextSibling(child);
			}
		}

		return highlightable;
	}

private:
	// properties
	OtFrameBuffer& framebuffer;
	OtFrameBuffer selectedBuffer{OtTexture::r8Texture, OtTexture::noTexture};

	OtUniformVec4 outlineUniforms{"u_outline", 1};

	OtSampler selectedSampler{"s_selectedTexture", OtTexture::pointSampling | OtTexture::clampSampling};

	OtShaderProgram selectProgram{"OtSelectVS", "OtSelectFS"};
	OtShaderProgram selectInstancingProgram{"OtSelectInstancingVS", "OtSelectFS"};
	OtShaderProgram outlineProgram{"OtOutlineVS", "OtOutlineFS"};
};
