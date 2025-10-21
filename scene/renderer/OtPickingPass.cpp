//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtAssert.h"

#include "glm/glm.hpp"

#include "OtPass.h"

#include "OtPickingPass.h"


//
//	OtPickingPass::render
//

void OtPickingPass::render(OtSceneRendererContext& ctx, glm::vec2 ndc, std::function<void(OtEntity)> callback) {
	// sanity check
	OtAssert(!picking);
	picking = true;

	// update buffer (if required)
	idBuffer.update(bufferSize, bufferSize);

	// create a "picking" camera focused on the selected point
	glm::mat4 inverse = glm::inverse(ctx.camera.viewProjectionMatrix);
	glm::vec3 eye = OtGlmMul(inverse, glm::vec3(ndc, OtGpuHasHomogeneousDepth() ? -1.0 : 0.0));
	glm::vec3 at = OtGlmMul(inverse, glm::vec3(ndc, 1.0));

	float nearPlane, farPlane;
	ctx.camera.getNearFar(nearPlane, farPlane);

	OtCamera pickingCamera{bufferSize, bufferSize, nearPlane, farPlane, 1.0f, eye, at};

	// setup pass to render entities as opaque blobs
	OtPass pass;
	pass.setRectangle(0, 0, bufferSize, bufferSize);
	pass.setFrameBuffer(idBuffer);
	pass.setClear(true, true);
	pass.setTransform(pickingCamera.viewMatrix, pickingCamera.projectionMatrix);
	pass.touch();

	// update context
	auto camera = ctx.camera;
	ctx.camera = pickingCamera;

	// render all entity IDs into buffer
	entityMap.clear();
	nextID = 1;
	renderEntities(ctx, pass);
	ctx.camera = camera;

	// transfer ID buffer back to CPU (this takes 2 frames hence the callback)
	auto texture = idBuffer.getColorTexture();

	idReadback.readback(texture, [this, callback]() {
		// find entity ID that appears most often in readback buffer
		std::unordered_map<int, int> frequency;
		int maxCount = 0;
		int picked = 0;
		uint8_t* pixels = (uint8_t*) idReadback.getImage().getPixels();

		for (auto i = 0; i < bufferSize * bufferSize; i++) {
			int pick = pixels[i];

			if (pick) {
				int count = 1;

				if (frequency.find(pick) != frequency.end()) {
					count = frequency[pick] + 1;
				}

				frequency[pick] = count;

				if (count > maxCount) {
					maxCount = count;
					picked = pick;
				}
			}
		}

		// if something was hit, report it back
		// clear selection otherwise
		callback(maxCount ? entityMap[picked] : OtEntityNull);
		picking = false;
	});
}


//
//	OtPickingPass::renderOpaqueGeometry
//

void OtPickingPass::renderOpaqueGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	ctx.pickingUniforms.setValue(0, float(nextID) / 255.0f, 0.0f, 0.0f, 0.0f);
	ctx.pickingUniforms.submit();
	entityMap[nextID++] = grd.entity;

	renderOpaqueGeometryHelper(
		ctx,
		grd,
		OtStateWriteRgb | OtStateWriteZ | OtStateDepthTestLess | OtStateLines,
		OtStateWriteRgb | OtStateWriteZ | OtStateDepthTestLess | (grd.component->cullBack ? OtStateCullCw : 0),
		MaterialSubmission::none,
		opaqueProgram,
		instancedOpaqueProgram);
}


//
//	OtPickingPass::renderOpaqueModel
//

void OtPickingPass::renderOpaqueModel(OtSceneRendererContext& ctx, OtModelRenderData& mrd) {
	ctx.pickingUniforms.setValue(0, float(nextID) / 255.0f, 0.0f, 0.0f, 0.0f);
	ctx.pickingUniforms.submit();
	entityMap[nextID++] = mrd.entity;

	renderOpaqueModelHelper(
		ctx,
		mrd,
		OtStateWriteRgb | OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
		MaterialSubmission::none,
		animatedOpaqueProgram,
		opaqueProgram);
}


//
//	OtPickingPass::renderTerrain
//

void OtPickingPass::renderTerrain(OtSceneRendererContext& ctx, [[maybe_unused]] OtEntity entity, OtTerrainComponent& terrain) {
	ctx.pickingUniforms.setValue(0, float(nextID) / 255.0f, 0.0f, 0.0f, 0.0f);
	ctx.pickingUniforms.submit();
	entityMap[nextID++] = entity;

	renderTerrainHelper(
		ctx,
		terrain,
		OtStateWriteRgb | OtStateWriteZ | OtStateDepthTestLess | OtStateLines,
		OtStateWriteRgb | OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
		terrainProgram);
}


//
//	OtPickingPass::renderGrass
//

void OtPickingPass::renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& grass) {
	ctx.pickingUniforms.setValue(0, float(nextID) / 255.0f, 0.0f, 0.0f, 0.0f);
	ctx.pickingUniforms.submit();
	entityMap[nextID++] = entity;

	renderGrassHelper(
		ctx,
		entity,
		grass,
		OtStateWriteRgb | OtStateWriteZ | OtStateDepthTestLess | OtStateCullCw,
		grassProgram);
}


//
//	OtPickingPass::renderTransparentGeometry
//

void OtPickingPass::renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) {
	ctx.pickingUniforms.setValue(0, float(nextID) / 255.0f, 0.0f, 0.0f, 0.0f);
	ctx.pickingUniforms.submit();
	entityMap[nextID++] = grd.entity;

	renderTransparentGeometryHelper(
		ctx,
		grd,
		OtStateWriteRgb | OtStateWriteZ | OtStateDepthTestLess | OtStateLines,
		OtStateWriteRgb | OtStateWriteZ | OtStateDepthTestLess | (grd.component->cullBack ? OtStateCullCw : 0),
		MaterialSubmission::justAlbedo,
		transparentProgram,
		instancedTransparentProgram);
}
