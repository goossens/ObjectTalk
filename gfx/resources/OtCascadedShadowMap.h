//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	inline bool isValid() { return framebuffers[0].isValid(); }

	// get dimensions
	inline int getSize() { return framebuffers[0].getWidth(); }

	// get cascade specific information
	inline float getDistance(int cascade) { return distances[cascade]; }
	inline OtCamera& getCamera(int cascade) { return cameras[cascade]; }
 	inline OtFrameBuffer& getFrameBuffer(int cascade) { return framebuffers[cascade]; }
	inline bgfx::TextureHandle getDepthTextureHandle(int cascade) { return framebuffers[cascade].getDepthTextureHandle(); }
	inline int getDepthTextureIndex(int cascade) { return framebuffers[cascade].getDepthTextureIndex(); }

	// number of cascades in shadowmap
	constexpr static int cascades = 4;

private:
	// cascade details
	float distances[cascades];
	std::array<OtCamera, cascades> cameras;
	std::array<OtAABB, cascades> aabb;
	std::array<OtFrameBuffer, cascades> framebuffers;
};
