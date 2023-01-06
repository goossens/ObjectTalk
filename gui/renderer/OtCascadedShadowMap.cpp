//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
#include "OtGlm.h"
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

void OtCascadedShadowMap::update(int s, OtCamera camera, const glm::vec3 &lightDirection) {
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
	auto tmpCamera = OtCameraClass::create(camera);

	for (auto cascade = 0, nn = 0, ff = 1; cascade < cascades; cascade++, nn += 2, ff += 2) {
		// get cascade frustum in worldspace
		tmpCamera->setPerspective(camera->getFOV(), distances[nn], distances[ff]);
		frustums[cascade] = tmpCamera->getFrustum();

		// get center of frustum in worldspace and calculate radius of enclosing sphere
		glm::vec3 center = frustums[cascade].getCenter();
		float radius = 0.0;

		for (auto corner = 0; corner < OtFrustum::pointCount; corner++) {
			radius = std::max(radius, glm::distance(center, frustums[cascade].getCorner(corner)));
		}

		// determine lightspace view matrix
		cameras[cascade]->setPositionVector(center - (glm::normalize(lightDirection) * (radius * 1.5f)));
		cameras[cascade]->setTargetVector(center);
		auto lightViewMatrix = cameras[cascade]->getViewMatrix();

		// transform frustum to lightspace and create AABB (from light's POV)
		auto lightSpaceFrustum = frustums[cascade].transform(lightViewMatrix);
		auto lightSpaceAABB = lightSpaceFrustum.getAABB();

		// set "light" camera projection and create the transformation matrix
		cameras[cascade]->setOrthographicCustom(
			lightSpaceAABB.getMin().x, lightSpaceAABB.getMax().x,
			lightSpaceAABB.getMin().y, lightSpaceAABB.getMax().y,
			-lightSpaceAABB.getMax().z, -lightSpaceAABB.getMin().z);

		transforms[cascade] = OtViewPortGetMatrix() * cameras[cascade]->getViewProjectionMatrix();
	}
}
