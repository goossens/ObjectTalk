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

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"

#include "OtCamera.h"
#include "OtFrameBuffer.h"
#include "OtFrustum.h"
#include "OtSampler.h"


//
//	OtCascadedShadowMap
//

class OtCascadedShadowMap {
public:
	// update resources
	void update(int size, OtCamera camera, const glm::vec3& lightDirection);

	// get cascade specific information
	float getCascadeNear(int cascade) { return distances[cascade * 2]; }
	float getCascadeFar(int cascade) { return distances[cascade * 2 + 1]; }
	OtFrustum& getCascadeFrustum(int cascade) { return frustums[cascade]; }
	OtCamera getCascadeCamera(int cascade) { return cameras[cascade]; }
	glm::mat4& getCascadeTransform(int cascade) { return transforms[cascade]; }
	OtFrameBuffer& getCascadeFramebuffer(int cascade) { return framebuffers[cascade]; }
	bgfx::TextureHandle getCascadeDepthTexture(int cascade) { return framebuffers[cascade].getDepthTexture(); }
	int getCascadeDepthTextureIndex(int cascade) { return framebuffers[cascade].getDepthTextureIndex(); }

	// get dimensions
	int getSize() { return size; }

	// number of layers in shadowmap
	constexpr static int cascades = 4;

private:
	// dimensions of shadowmap;
	int size = -1;

	// cascade distances { near0, far0, near1, far1... nearN, farN }
	float distances[cascades * 2];

	// cascade frustums
	std::array<OtFrustum, cascades> frustums;

	// cascade cameras
	std::array<OtCamera, cascades> cameras;

	// cascade transforms
	std::array<glm::mat4, cascades> transforms;

	// GPU resources
	std::array<OtFrameBuffer, cascades> framebuffers;

	// initialization flag
	bool initialized = false;
};
