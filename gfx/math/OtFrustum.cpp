//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

	for (auto& point : points) {
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
	for (auto c = 0; c < planeCount; c++) {
		planes[c].normalize();
	}

	// determine corners
	glm::mat4 inverse = glm::inverse(matrix);
	float n = OtGpuHasHomogeneousDepth() ? -1.0f : 0.0f;

	points[nearBottomLeft] = OtGlmMul(inverse, glm::vec4(-1.0f, -1.0f, n, 1.0f));
	points[nearTopLeft] = OtGlmMul(inverse, glm::vec4(-1.0f, 1.0f, n, 1.0f));
	points[nearTopRight] = OtGlmMul(inverse, glm::vec4(1.0f, 1.0f, n, 1.0f));
	points[nearBottomRight] = OtGlmMul(inverse, glm::vec4(1.0f, -1.0f, n, 1.0f));

	points[farBottomLeft] = OtGlmMul(inverse, glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f));
	points[farTopLeft] = OtGlmMul(inverse, glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f));
	points[farTopRight] = OtGlmMul(inverse, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	points[farBottomRight] = OtGlmMul(inverse, glm::vec4(1.0f, -1.0f, 1.0f, 1.0f));
}


//
//	OtFrustum::isVisiblePoint
//

bool OtFrustum::isVisiblePoint(const glm::vec3& point) {
	for (auto i = 0; i < planeCount; i++) {
		if (glm::dot(planes[i].getNormal(), point) + planes[i].getDistance() < 0) {
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
	for (auto i = 0; i < planeCount; i++) {
		glm::vec4 g = planes[i].getVec4();

		if ((glm::dot(g, glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(g, glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(g, glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(g, glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(g, glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
			(glm::dot(g, glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
			(glm::dot(g, glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
			(glm::dot(g, glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0)) {

			// Not visible - all returned negative
			return false;
		}
	}

	// check frustum outside/inside box
	int out;
	out = 0; for (auto i = 0; i < pointCount; i++) { out += points[i].x > maxp.x ? 1 : 0; } if (out == pointCount) { return false; }
	out = 0; for (auto i = 0; i < pointCount; i++) { out += points[i].x < minp.x ? 1 : 0; } if (out == pointCount) { return false; }
	out = 0; for (auto i = 0; i < pointCount; i++) { out += points[i].y > maxp.y ? 1 : 0; } if (out == pointCount) { return false; }
	out = 0; for (auto i = 0; i < pointCount; i++) { out += points[i].y < minp.y ? 1 : 0; } if (out == pointCount) { return false; }
	out = 0; for (auto i = 0; i < pointCount; i++) { out += points[i].z > maxp.z ? 1 : 0; } if (out == pointCount) { return false; }
	out = 0; for (auto i = 0; i < pointCount; i++) { out += points[i].z < minp.z ? 1 : 0; } if (out == pointCount) { return false; }

	return true;
}


//
//	OtFrustum::isVisibleSphere
//

bool OtFrustum::isVisibleSphere(const glm::vec3& center, float radius) {
	for (auto i = 0; i < planeCount; i++) {
		if (glm::dot(planes[i].getNormal(), center) + planes[i].getDistance() + radius < 0) {
			return false;
		}
	}

	return true;
}


//
//	OtFrustum::getCenter
//

glm::vec3 OtFrustum::getCenter() {
	glm::vec3 center = glm::vec3(0.0);

	for (auto& point : points) {
		center += point;
	}

	return center / (float) pointCount;
}


//
//	OtFrustum::getAABB
//

OtAABB OtFrustum::getAABB() {
	OtAABB aabb;

	for (auto& point : points) {
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
	for (auto& point : points) {
		OtGlmDebug(point);
	}
}
