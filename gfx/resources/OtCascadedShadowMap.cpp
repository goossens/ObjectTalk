//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "glm/gtc/matrix_transform.hpp"

#include "OtCascadedShadowMap.h"


//
//	OtCascadedShadowMap::update
//

void OtCascadedShadowMap::update(OtCamera& camera, const glm::vec3& lightDirection, int size) {
	// create framebuffers (if required)
	if (!isValid()) {
		for (auto& cascade : cascades) {
			cascade.framebuffer.initialize(OtTexture::Format::none, OtTexture::Format::d32);
		}
	}

	// update framebuffers (if required)
	if (cascades[0].framebuffer.getWidth() != size) {
		for (auto& cascade : cascades) {
			cascade.framebuffer.update(size, size);
		}
	}

	// the following is based on https://ahbejarano.gitbook.io/lwjglgamedev/chapter-17

	// calculate the best cascade splits
	// based on https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
	float minZ, maxZ;
	camera.getNearFar(minZ, maxZ);
	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	static constexpr float cascadeSplitLambda = 0.75f;
	float cascadeSplits[maxCascades];

	for (size_t i = 0; i < maxCascades; i++) {
		float p = static_cast<float>(i + 1) / static_cast<float>(maxCascades);
		float log = minZ * std::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = cascadeSplitLambda * (log - uniform) + uniform;
		cascadeSplits[i] = (d - minZ) / range;
	}

	// determine frustum corners in clip space
	glm::vec3 frustumCorners[] = {
		// near face
		{  1.0f,  1.0f, 0.0f },
		{ -1.0f,  1.0f, 0.0f },
		{  1.0f, -1.0f, 0.0f },
		{ -1.0f, -1.0f, 0.0f },

		// far face
		{  1.0f,  1.0f, 1.0f },
		{ -1.0f,  1.0f, 1.0f },
		{  1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f }
	};

	// project frustum corners into world space from clip space
	glm::mat4 invViewProj = glm::inverse(camera.viewProjectionMatrix);

	for (auto& frustumCorner : frustumCorners) {
		auto invCorner = invViewProj * glm::vec4(frustumCorner, 1.0f);
		frustumCorner = glm::vec3(invCorner / invCorner.w);
	}

	// calculate orthographic projection matrix for each cascade
	float lastSplitDist = 0.0f;

	for (size_t cascade = 0; cascade < maxCascades; cascade++) {
		float splitDist = cascadeSplits[cascade];

		// determine cascade corners (in world space)
		glm::vec3 cascadeWorldCorners[8];

		for (auto i = 0; i < 4; i++) {
			auto dist = frustumCorners[i + 4] - frustumCorners[i];
			cascadeWorldCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
			cascadeWorldCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
		}

		// determine AABB for cascade (in world space)
		cascades[cascade].aabb.clear();

		for (auto& cascadeWorldCorner : cascadeWorldCorners) {
			cascades[cascade].aabb.addPoint(cascadeWorldCorner);
		}

		// get cascade center (in world space)
		glm::vec3 cascadeCenter = glm::vec3(0.0f);

		for (auto& cascadeWorldCorner : cascadeWorldCorners) {
			cascadeCenter += cascadeWorldCorner;
		}

		cascadeCenter /= 8.0f;

		// determine sphere that encloses cascade
		float radius = 0.0f;

		for (auto& cascadeWorldCorner : cascadeWorldCorners) {
			float distance = glm::length(glm::vec3(cascadeWorldCorner) - cascadeCenter);
			radius = std::max(radius, distance);
		}

		radius = std::ceil(radius * 16.0f) / 16.0f;

		// calculate the view and projection matrix
		auto cameraPosition = cascadeCenter + lightDirection * radius;
		glm::mat4 lightViewMatrix = glm::lookAt(cameraPosition, cascadeCenter, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjectionMatrix = glm::orthoRH_ZO(-radius, radius, -radius, radius, -maxZ, radius + radius);

		// offset to texel space to avoid shimmering
		// https://stackoverflow.com/questions/33499053/cascaded-shadow-map-shimmering
		glm::mat4 shadowMatrix = lightProjectionMatrix * lightViewMatrix;
		glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		shadowOrigin = shadowMatrix * shadowOrigin;
		float mapSize = static_cast<float>(cascades[cascade].framebuffer.getWidth());
		shadowOrigin = shadowOrigin * mapSize / 2.0f;

		glm::vec4 roundedOrigin = glm::round(shadowOrigin);
		glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
		roundOffset = roundOffset * (2.0f / mapSize);
		roundOffset.z = 0.0f;
		roundOffset.w = 0.0f;

		glm::mat4 shadowProj = lightProjectionMatrix;
		shadowProj[3] += roundOffset;
		lightProjectionMatrix = shadowProj;

		// store information
		cascades[cascade].distance = minZ + splitDist * range;
		cascades[cascade].camera = OtCamera(static_cast<int>(mapSize), static_cast<int>(mapSize), lightProjectionMatrix, lightViewMatrix);
		lastSplitDist = splitDist;
	}
}


//
//	OtCascadedShadowMap::clear
//

void OtCascadedShadowMap::clear() {
	for (auto& cascade : cascades) {
		cascade.framebuffer.clear();
	}
}
