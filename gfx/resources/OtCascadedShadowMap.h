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

#include "OtAABB.h"
#include "OtCamera.h"
#include "OtFrameBuffer.h"


//
//	OtCascadedShadowMap
//

class OtCascadedShadowMap {
public:
	// update resources
	void update(int size, OtCamera& camera, const glm::vec3& lightDirection);

	// clear all resources
	void clear();

	// see if shadowmap is valid
	inline bool isValid() { return framebuffers[0].isValid(); }

	// get dimensions
	inline int getSize() { return framebuffers[0].getWidth(); }

	// get cascade specific information
	inline float getDistance(int cascade) { return distances[cascade]; }
	inline glm::vec3& getCameraPosition(int cascade) { return cameraPosition[cascade]; }
	inline OtAABB& getAABB(int cascade) { return aabb[cascade]; }
	inline glm::mat4& getViewMatrix(int cascade) { return viewMatrix[cascade]; }
	inline glm::mat4& getProjectionMatrix(int cascade) { return projectionMatrix[cascade]; }
	inline glm::mat4 getViewProjectionMatrix(int cascade) { return projectionMatrix[cascade] * viewMatrix[cascade]; }
 	inline OtFrameBuffer& getFrameBuffer(int cascade) { return framebuffers[cascade]; }
	inline bgfx::TextureHandle getDepthTextureHandle(int cascade) { return framebuffers[cascade].getDepthTextureHandle(); }
	inline int getDepthTextureIndex(int cascade) { return framebuffers[cascade].getDepthTextureIndex(); }

	// number of cascades in shadowmap
	constexpr static int cascades = 4;

private:
	// cascade details
	float distances[cascades];
	std::array<glm::vec3, cascades> cameraPosition;
	std::array<OtAABB, cascades> aabb;
	std::array<glm::mat4, cascades> viewMatrix;
	std::array<glm::mat4, cascades> projectionMatrix;
	std::array<OtFrameBuffer, cascades> framebuffers;
};
