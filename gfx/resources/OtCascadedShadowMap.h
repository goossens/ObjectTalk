//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>

#include "glm/glm.hpp"

#include "OtCamera.h"
#include "OtFrameBuffer.h"


//
//	OtCascadedShadowMap
//

class OtCascadedShadowMap {
public:
	// update resources
	void update(OtCamera& camera, const glm::vec3& lightDirection, int size=1024);

	// clear all resources
	void clear();

	// see if shadowmap is valid
	inline bool isValid() { return cascades[0].framebuffer.isValid(); }

	// get dimensions
	inline int getSize() { return cascades[0].framebuffer.getWidth(); }

	// get cascade specific information
	inline float getDistance(size_t cascade) { return cascades[cascade].distance; }
	inline OtCamera& getCamera(size_t cascade) { return cascades[cascade].camera; }
	inline OtFrameBuffer& getFrameBuffer(size_t cascade) { return cascades[cascade].framebuffer; }
	inline OtTexture& getDepthTexture(size_t cascade) { return cascades[cascade].framebuffer.getDepthTexture(); }

	// number of cascades in shadowmap
	constexpr static size_t maxCascades = 4;

private:
	// cascade details
	struct Cascade {
		float distance;
		OtCamera camera;
		OtAABB aabb;
		OtFrameBuffer framebuffer;
	};

	std::array<Cascade, maxCascades> cascades;
};
