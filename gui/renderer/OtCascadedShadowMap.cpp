//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <limits>

#include "OtAABB.h"
#include "OtCascadedShadowMap.h"
#include "OtViewPort.h"


//
//	calculateSplits
//

static void calculateSplits(float* result, size_t splits, float near, float far, float weight=0.75) {
	// return { near0, far0, near1, far1... nearN, farN }
	auto ratio = far / near;
	auto slices = splits * 2;

	result[0] = near;
	result[slices - 1] = far;

	for (auto nn = 2, ff = 1; nn < slices; nn += 2, ff += 2) {
		float si = float(ff) / float(slices);
		float nearp = weight * (near * std::pow(ratio, si)) + (1 - weight) * (near + (far - near) * si);
		result[nn] = nearp;
		result[ff] = nearp * 1.005;
	}
}


//
//	OtCascadedShadowMap::update
//

void OtCascadedShadowMap::update(int s, OtCamera camera, const glm::vec3& lightDirection) {
	// create resources if required
	if (!initialized) {
		for (auto i = 0; i < cascades; i++) {
			cameras[i] = OtCameraClass::create();
			framebuffers[i] = OtFrameBuffer(OtFrameBuffer::noTexture, OtFrameBuffer::dFloatTexture);
		}

		initialized = true;
	}

	// update framebuffers if required
	if (s != size) {
		size = s;

		for (auto& framebuffer : framebuffers) {
			framebuffer.update(size, size);
		}
	}

	// determine distances for cascaded frustums
	calculateSplits(distances, cascades, camera->getNearClip(), camera->getFarClip());

	// determine a transform for each cascade
	auto fov = camera->getFOV();
	float tanHalfHFOV = std::tan(glm::radians(fov / 2.0));
	float tanHalfVFOV = std::tan(glm::radians(fov / 2.0 / camera->getAspectRatio()));
	auto invViewMatrix = glm::inverse(camera->getViewMatrix());

	for (auto i = 0, nn = 0, ff = 1; i < cascades; i++, nn += 2, ff += 2) {
		// determine the world space frustum
		float xn = distances[nn] * tanHalfHFOV;
		float xf = distances[ff] * tanHalfHFOV;
		float yn = distances[nn] * tanHalfVFOV;
		float yf = distances[ff] * tanHalfVFOV;

		glm::vec3 corners[8] = {
			// near plane
			invViewMatrix * glm::vec4(-xn, -yn, -distances[nn], 1.0),
			invViewMatrix * glm::vec4(-xn, yn, -distances[nn], 1.0),
			invViewMatrix * glm::vec4(xn, yn, -distances[nn], 1.0),
			invViewMatrix * glm::vec4(xn, -yn, -distances[nn], 1.0),

			// far plane
			invViewMatrix * glm::vec4(-xf, -yf, -distances[ff], 1.0),
			invViewMatrix * glm::vec4(-xf, yf, -distances[ff], 1.0),
			invViewMatrix * glm::vec4(xf, yf, -distances[ff], 1.0),
			invViewMatrix * glm::vec4(xf, -yf, -distances[ff], 1.0)
		};

		frustums[i] = OtFrustum(corners[0], corners[1], corners[2], corners[3], corners[4], corners[5], corners[6], corners[7]);
		glm::vec3 center = frustums[i].getCenter();

		// determine light space matrix
		cameras[i] = OtCameraClass::create();
		cameras[i]->setPositionVector(center - lightDirection);
		cameras[i]->setTargetVector(center);
		auto lightViewMatrix = cameras[i]->getViewMatrix();

		// translate the frustum corners to light space and create AABB
		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::lowest();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::lowest();

		for (auto corner : corners) {
			auto lightSpace = lightViewMatrix * glm::vec4(corner, 1.0);

			minX = std::min(minX, lightSpace.x);
			maxX = std::max(maxX, lightSpace.x);
			minY = std::min(minY, lightSpace.y);
			maxY = std::max(maxY, lightSpace.y);
			minZ = std::min(minZ, lightSpace.z);
			maxZ = std::max(maxZ, lightSpace.z);
		}

		// increase near and far planes (to include out of sight objects that create shadows)
/*
		constexpr float factor = 1.1;

		if (minZ < 0.0) {
			minZ *= factor;

		} else {
			minZ /= factor;
		}

		if (maxZ < 0.0) {
			maxZ /= factor;

		} else {
			maxZ *= factor;
		}
*/

		// set camera projection and create the transformation matrix
		cameras[i]->setOrthographicCustom(minX, maxX, minY, maxY, -maxZ, -minZ);
		transforms[i] = OtViewPortGetMatrix() * cameras[i]->getViewProjectionMatrix();
	}
}
