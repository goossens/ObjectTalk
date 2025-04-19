//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFrustum.h"
#include "OtGlm.h"
#include "OtGpu.h"


//
//	OtFrustum::OtFrustum
//

OtFrustum::OtFrustum() {
	for (auto& plane : planes) {
		plane = OtPlane();
	}

	for (auto& point : corners) {
		point = glm::vec3(0.0f);
	}
}

OtFrustum::OtFrustum(const glm::mat4& matrix) {
	// determine planes
	glm::mat4 m = glm::transpose(matrix);
	planes[leftPlane] = OtPlane(m[3] + m[0]);
	planes[rightPlane] = OtPlane(m[3] - m[0]);
	planes[bottomPlane] = OtPlane(m[3] + m[1]);
	planes[topPlane] = OtPlane(m[3] - m[1]);
	planes[nearPlane] = OtPlane(OtGpuHasHomogeneousDepth() ? m[3] + m[2] : m[2]);
	planes[farPlane] = OtPlane(m[3] - m[2]);

	// normalize planes
	for (auto& plane : planes) {
		plane.normalize();
	}

	// determine corners
	glm::mat4 inverse = glm::inverse(matrix);
	float n = OtGpuHasHomogeneousDepth() ? -1.0f : 0.0f;

	corners[nearBottomLeft] = OtGlmMul(inverse, glm::vec4(-1.0f, -1.0f, n, 1.0f));
	corners[nearTopLeft] = OtGlmMul(inverse, glm::vec4(-1.0f, 1.0f, n, 1.0f));
	corners[nearTopRight] = OtGlmMul(inverse, glm::vec4(1.0f, 1.0f, n, 1.0f));
	corners[nearBottomRight] = OtGlmMul(inverse, glm::vec4(1.0f, -1.0f, n, 1.0f));

	corners[farBottomLeft] = OtGlmMul(inverse, glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f));
	corners[farTopLeft] = OtGlmMul(inverse, glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f));
	corners[farTopRight] = OtGlmMul(inverse, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	corners[farBottomRight] = OtGlmMul(inverse, glm::vec4(1.0f, -1.0f, 1.0f, 1.0f));
}


//
//	OtFrustum::isVisiblePoint
//

bool OtFrustum::isVisiblePoint(const glm::vec3& point) {
	for (auto& plane : planes) {
		if (glm::dot(plane.getNormal(), point) + plane.getDistance() < 0) {
			return false;
		}
	}

	return true;
}


//
//	OtFrustum::isVisibleAABB
//

bool OtFrustum::isVisibleAABB(const OtAABB& aabb) {
	// see https://iquilezles.org/articles/frustumcorrect/
	glm::vec3 minp = aabb.getMin();
	glm::vec3 maxp = aabb.getMax();

	// check box outside/inside of frustum
	for (auto& plane : planes) {
		glm::vec4 g = plane.getVec4();

		if ((glm::dot(g, glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0f) &&
			(glm::dot(g, glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0f) &&
			(glm::dot(g, glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0f) &&
			(glm::dot(g, glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0f) &&
			(glm::dot(g, glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0f) &&
			(glm::dot(g, glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0f) &&
			(glm::dot(g, glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0f) &&
			(glm::dot(g, glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0f)) {

			// Not visible - all returned negative
			return false;
		}
	}

	// check frustum outside/inside box
	int out;
	out = 0; for (auto& corner : corners) { out += corner.x > maxp.x ? 1 : 0; } if (out == 8) { return false; }
	out = 0; for (auto& corner : corners) { out += corner.x < minp.x ? 1 : 0; } if (out == 8) { return false; }
	out = 0; for (auto& corner : corners) { out += corner.y > maxp.y ? 1 : 0; } if (out == 8) { return false; }
	out = 0; for (auto& corner : corners) { out += corner.y < minp.y ? 1 : 0; } if (out == 8) { return false; }
	out = 0; for (auto& corner : corners) { out += corner.z > maxp.z ? 1 : 0; } if (out == 8) { return false; }
	out = 0; for (auto& corner : corners) { out += corner.z < minp.z ? 1 : 0; } if (out == 8) { return false; }

	return true;
}


//
//	OtFrustum::isVisibleSphere
//

bool OtFrustum::isVisibleSphere(const glm::vec3& center, float radius) {
	for (auto& plane : planes) {
		if (glm::dot(plane.getNormal(), center) + plane.getDistance() + radius < 0) {
			return false;
		}
	}

	return true;
}


//
//	OtFrustum::getCenter
//

glm::vec3 OtFrustum::getCenter() {
	glm::vec3 center = glm::vec3(0.0f);

	for (auto& point : corners) {
		center += point;
	}

	return center / 8.0f;
}


//
//	OtFrustum::getAABB
//

OtAABB OtFrustum::getAABB() {
	OtAABB aabb;

	for (auto& point : corners) {
		aabb.addPoint(point);
	}

	return aabb;
}


//
//	OtFrustum::debugPlanes
//

void OtFrustum::debugPlanes() {
	for (auto& plane : planes) {
		plane.debug();
	}
}


//
//	OtFrustum::debugPoints
//

void OtFrustum::debugPoints() {
	OtGlmDebug("NearBottomLeft: ", corners[nearBottomLeft]);
	OtGlmDebug("NearTopLeft: ", corners[nearTopLeft]);
	OtGlmDebug("NearTopRight: ", corners[nearTopRight]);
	OtGlmDebug("NearBottomRight: ", corners[nearBottomRight]);
	OtGlmDebug("FarBottomLeft: ", corners[farBottomLeft]);
	OtGlmDebug("FarTopLeft: ", corners[farTopLeft]);
	OtGlmDebug("FarTopRight: ", corners[farTopRight]);
	OtGlmDebug("FarBottomRight: ", corners[farBottomRight]);
}
