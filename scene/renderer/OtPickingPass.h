//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "glm/glm.hpp"

#include "OtAssert.h"

#include "OtFrameBuffer.h"
#include "OtGlm.h"
#include "OtGpu.h"
#include "OtPass.h"
#include "OtReadBackBuffer.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtPickingPass
//

class OtPickingPass : public OtSceneRenderEntitiesPass {
public:
	// render the pass
	inline void render(OtSceneRendererContext& ctx, glm::vec2 ndc, std::function<void(OtEntity)> callback) {
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

		OtCamera camera{bufferSize, bufferSize, nearPlane, farPlane, 1.0f, eye, at};

		// setup pass to render entities as opaque blobs
		OtPass pass;
		pass.setRectangle(0, 0, bufferSize, bufferSize);
		pass.setFrameBuffer(idBuffer);
		pass.setClear(true, true);
		pass.setTransform(camera.viewMatrix, camera.projectionMatrix);
		pass.touch();

		// create a new context
		OtSceneRendererContext pctx{ctx};
		pctx.camera = camera;

		// render all entity IDs into buffer
		entityMap.clear();
		nextID = 1;
		renderEntities(pctx, pass);

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

	// see if we are currently "picking"
	bool isPicking() { return picking; }

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

	inline uint64_t getNormalState() override {
		return
			OtStateWriteRgb |
			OtStateWriteZ |
			OtStateDepthTestLess;
	}

	inline uint64_t getCullBackState() override {
		return
			OtStateWriteRgb |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw;
	};

	inline uint64_t getWireframeState() override {
		return
			OtStateWriteRgb |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateLines;
	};

	inline void submitUniforms(OtSceneRendererContext& /* ctx */, Scope& scope) override {
		pickingUniforms.setValue(0, float(nextID) / 255.0f, 0.0f, 0.0f, 0.0f);
		pickingUniforms.submit();
		entityMap[nextID++] = scope.entity;

		if (scope.isTerrain) { submitTerrainUniforms(*scope.terrain); }
		if (scope.isTransparent) { submitAlbedoUniforms(*scope.material); }
	}

private:
	// properties
	OtFrameBuffer idBuffer{OtTexture::r8Texture, OtTexture::dFloatTexture};
	OtReadBackBuffer idReadback;
	static constexpr int bufferSize = 8;
	bool picking = false;

	std::unordered_map<int, OtEntity> entityMap;
	int nextID = 1;
	OtUniformVec4 pickingUniforms{"u_picking", 1};

	OtShaderProgram opaqueProgram{"OtPickingVS", "OtPickingOpaqueFS"};
	OtShaderProgram instancedOpaqueProgram{"OtPickingInstancingVS", "OtPickingOpaqueFS"};
	OtShaderProgram animatedOpaqueProgram{"OtPickingAnimatedVS", "OtPickingOpaqueFS"};
	OtShaderProgram transparentProgram{"OtPickingVS", "OtPickingTransparentFS"};
	OtShaderProgram instancedTransparentProgram{"OtPickingInstancingVS", "OtPickingTransparentFS"};
	OtShaderProgram terrainProgram{"OtTerrainSimpleVS", "OtPickingOpaqueFS"};
};
