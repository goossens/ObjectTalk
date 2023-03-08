//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	project
//

static inline glm::vec4 project(const glm::mat4& matrix, const glm::vec4& ndc) {
	glm::vec4 v = matrix * ndc;
	return glm::vec4(v.x / v.w, v.y / v.w, v.z / v.w, 1.0f);
}


//
//	OtFrustum::OtFrustum
//

OtFrustum::OtFrustum() {
	for (auto& plane : planes) {
		plane = OtPlane();
	}

	for (auto& point : points) {
		point = glm::vec3(0.0);
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
	float n = OtGpuHasHomogeneousDepth() ? -1.0 : 0.0;

	points[nearBottomLeft] = project(inverse, glm::vec4(-1.0, -1.0, n, 1.0));
	points[nearTopLeft] = project(inverse, glm::vec4(-1.0, 1.0, n, 1.0));
	points[nearTopRight] = project(inverse, glm::vec4(1.0, 1.0, n, 1.0));
	points[nearBottomRight] = project(inverse, glm::vec4(1.0, -1.0, n, 1.0));

	points[farBottomLeft] = project(inverse, glm::vec4(-1.0, -1.0, 1.0, 1.0));
	points[farTopLeft] = project(inverse, glm::vec4(-1.0, 1.0, 1.0, 1.0));
	points[farTopRight] = project(inverse, glm::vec4(1.0, 1.0, 1.0, 1.0));
	points[farBottomRight] = project(inverse, glm::vec4(1.0, -1.0, 1.0, 1.0));
}

OtFrustum::OtFrustum(glm::vec3 &nbl, glm::vec3 &ntl, glm::vec3 &ntr, glm::vec3 &nbr, glm::vec3 &fbl, glm::vec3 &ftl, glm::vec3 &ftr, glm::vec3 &fbr) {
	// set corners
	points[nearBottomLeft] = nbl;
	points[nearTopLeft] = ntl;
	points[nearTopRight] = ntr;
	points[nearBottomRight] = nbr;

	points[farBottomLeft] = fbl;
	points[farTopLeft] = ftl;
	points[farTopRight] = ftr;
	points[farBottomRight] = fbr;

	// set planes
	planes[leftPlane] = OtPlane(nbl, ntl, ftl);
	planes[rightPlane] = OtPlane(nbr, ntr, ftr);
	planes[bottomPlane] = OtPlane(nbl, nbr, fbr);
	planes[topPlane] = OtPlane(ntl, ntr, ftr);
	planes[nearPlane] = OtPlane(nbl, ntl, ntr);
	planes[farPlane] = OtPlane(fbl, ftl, ftr);
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
	glm::vec3 minp = aabb.getMin();
	glm::vec3 maxp = aabb.getMax();

	for (auto i = 0; i < planeCount; i++) {
		glm::vec3 normal = planes[i].getNormal();
		glm::vec3 v;

		v.x = normal.x < 0 ? minp.x : maxp.x;
		v.y = normal.y < 0 ? minp.y : maxp.y;
		v.z = normal.z < 0 ? minp.z : maxp.z;

		if (glm::dot(normal, v) + planes[i].getDistance() < 0) {
			return false;
		}
	}

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
//	OtFrustum::transform
//

OtFrustum OtFrustum::transform(const glm::mat4& transform) {
	glm::vec3 newPoints[pointCount] = {
		OtGlmMul(transform, points[nearBottomLeft]),
		OtGlmMul(transform, points[nearTopLeft]),
		OtGlmMul(transform, points[nearTopRight]),
		OtGlmMul(transform, points[nearBottomRight]),
		OtGlmMul(transform, points[farBottomLeft]),
		OtGlmMul(transform, points[farTopLeft]),
		OtGlmMul(transform, points[farTopRight]),
		OtGlmMul(transform, points[farBottomRight])
	};

	return OtFrustum(
		newPoints[0], newPoints[1], newPoints[2], newPoints[3],
		newPoints[4], newPoints[5], newPoints[6], newPoints[7]);
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
