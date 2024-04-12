//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "glm/gtc/matrix_transform.hpp"

#include "OtCascadedShadowMap.h"
#include "OtGpu.h"


//
//	OtCascadedShadowMap::update
//

void OtCascadedShadowMap::update(OtCamera& camera, const glm::vec3& lightDirection, int size) {
	// create resources if required
	if (!isValid()) {
		for (auto& framebuffer : framebuffers) {
			framebuffer.initialize(OtTexture::noTexture, OtTexture::dFloatTexture);
		}
	}

	// update framebuffers if required
	if (framebuffers[0].getWidth() != size) {
		for (auto& framebuffer : framebuffers) {
			framebuffer.update(size, size);
		}
	}

	// the following is based on https://ahbejarano.gitbook.io/lwjglgamedev/chapter-17

	// calculate the best cascade splits
	float minZ, maxZ;
	camera.getNearFar(minZ, maxZ);
	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	float cascadeSplitLambda = 0.95f;
	float cascadeSplits[cascades];

	for (auto i = 0; i < cascades; i++) {
		float p = (float) (i + 1) / (float) cascades;
		float log = minZ * glm::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = cascadeSplitLambda * (log - uniform) + uniform;
		cascadeSplits[i] = (d - minZ) / range;
	}

	// determine furstum corners
	glm::mat4 invViewProj = glm::inverse(camera.viewProjectionMatrix);
	float n = OtGpuHasHomogeneousDepth() ? -1.0f : 0.0f;

	glm::vec4 frustumCorners[] = {
		// near face
		{  1.0f,  1.0f, n, 1.0f },
		{ -1.0f,  1.0f, n, 1.0f },
		{  1.0f, -1.0f, n, 1.0f },
		{ -1.0f, -1.0f, n, 1.0f },

		// far face
		{  1.0f,  1.0f, 1.0f, 1.0f },
		{ -1.0f,  1.0f, 1.0f, 1.0f },
		{  1.0f, -1.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f, 1.0f }
	};

	// project frustum corners into world space from clip space
	for (auto& frustumCorner : frustumCorners) {
		auto invCorner = invViewProj * frustumCorner;
		frustumCorner = invCorner / invCorner.w;
	}

	// calculate orthographic projection matrix for each cascade
	float lastSplitDist = 0.0f;

	for (auto cascade = 0; cascade < cascades; cascade++) {
		float splitDist = cascadeSplits[cascade];

		// determine casecade corners
		glm::vec4 cascadeCorners[8];

		for (auto i = 0; i < 4; i++) {
			auto dist = frustumCorners[i + 4] - frustumCorners[i];
			cascadeCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
			cascadeCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
		}

		// determine AABB for cascade
		aabb[cascade].clear();

		for (auto& cascadeCorner : cascadeCorners) {
			aabb[cascade].addPoint(cascadeCorner);
		}

		// get cascade center
		glm::vec3 cascadeCenter = glm::vec3(0.0f);

		for (glm::vec4& cascadeCorner : cascadeCorners) {
			cascadeCenter += glm::vec3(cascadeCorner);
		}

		cascadeCenter /= 8.0f;

		// determine sphere that encloses cascade
		float radius = 0.0f;

		for (glm::vec4& cascadeCorner : cascadeCorners) {
			float distance = glm::length(glm::vec3(cascadeCorner) - cascadeCenter);
			radius = glm::max(radius, distance);
		}

		radius = std::ceil(radius * 16.0f) / 16.0f;

		// calculate the view and projection matrix
		auto cameraPosition = cascadeCenter + lightDirection * radius;
		glm::mat4 lightViewMatrix = glm::lookAt(cameraPosition, cascadeCenter, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 lightProjectionMatrix = OtGpuHasHomogeneousDepth()
			? glm::orthoRH_NO(-radius, radius, -radius, radius, 0.0f, radius + radius)
			: glm::orthoRH_ZO(-radius, radius, -radius, radius, 0.0f, radius + radius);

		// offset to texel space to avoid shimmering
		// https://stackoverflow.com/questions/33499053/cascaded-shadow-map-shimmering
		glm::mat4 shadowMatrix = lightProjectionMatrix * lightViewMatrix;
		glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		shadowOrigin = shadowMatrix * shadowOrigin;
		float storedW = shadowOrigin.w;
		float size = static_cast<float>(framebuffers[cascade].getWidth());
		shadowOrigin = shadowOrigin * size / 2.0f;

		glm::vec4 roundedOrigin = glm::round(shadowOrigin);
		glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
		roundOffset = roundOffset * 2.0f / size;
		roundOffset.z = 0.0f;
		roundOffset.w = 0.0f;

		glm::mat4 shadowProj = lightProjectionMatrix;
		shadowProj[3] += roundOffset;
		lightProjectionMatrix = shadowProj;

		// store information
		distances[cascade] = minZ + splitDist * range;
		cameras[cascade] = OtCamera(size, size, cameraPosition, lightProjectionMatrix, lightViewMatrix);
		lastSplitDist = splitDist;
	}
}


//
//	OtCascadedShadowMap::clear
//

void OtCascadedShadowMap::clear() {
	for (auto& framebuffer : framebuffers) {
		framebuffer.clear();
	}
}
