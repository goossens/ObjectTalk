//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFrustum.h"
#include "OtGpu.h"


//
//	project
//

static inline glm::vec4 project(const glm::mat4& matrix, const glm::vec4& ndc) {
	glm::vec4 v = matrix * ndc;
	return glm::vec4(v.x / v.w, v.y / v.w, v.z / v.w, 1.0f);
}


//
//	OtFrustumClass::update
//

void OtFrustumClass::update(const glm::mat4& matrix) {
	// determine planes
	glm::mat4 m = glm::transpose(matrix);
	planes[left] = OtPlaneClass::create(m[3] + m[0]);
	planes[right] = OtPlaneClass::create(m[3] - m[0]);
	planes[bottom] = OtPlaneClass::create(m[3] + m[1]);
	planes[top] = OtPlaneClass::create(m[3] - m[1]);
	planes[near] = OtPlaneClass::create(OtGpuHasHomogeneousDepth() ? m[3] + m[2] : m[2]);
	planes[far] = OtPlaneClass::create(m[3] - m[2]);

	// normalize planes
	for (auto c = 0; c < planeCount; c++) {
		planes[c]->normalize();
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


//
//	OtFrustumClass::isVisiblePoint
//

bool OtFrustumClass::isVisiblePoint(const glm::vec3& point) {
	for (auto i = 0; i < planeCount; i++) {
		if (glm::dot(planes[i]->getNormal(), point) + planes[i]->getDistance() < 0) {
			return false;
		}
	}

	return true;
}


//
//	OtFrustumClass::isVisibleAABB
//

bool OtFrustumClass::isVisibleAABB(OtAABB aabb) {
	glm::vec3 minp = aabb->getMin();
	glm::vec3 maxp = aabb->getMax();

	for (auto i = 0; i < planeCount; i++) {
		glm::vec3 normal = planes[i]->getNormal();
		glm::vec3 v;

		v.x = normal.x < 0 ? minp.x : maxp.x;
		v.y = normal.y < 0 ? minp.y : maxp.y;
		v.z = normal.z < 0 ? minp.z : maxp.z;

		if (glm::dot(normal, v) + planes[i]->getDistance() < 0) {
			return false;
		}
	}

	return true;
}


//
//	OtFrustumClass::isVisibleSphere
//

bool OtFrustumClass::isVisibleSphere(const glm::vec3 &center, float radius) {
	for (auto i = 0; i < planeCount; i++) {
		if (glm::dot(planes[i]->getNormal(), center) + planes[i]->getDistance() + radius < 0) {
			return false;
		}
	}

	return true;
}


//
//	OtFrustumClass::getCenter
//

glm::vec3 OtFrustumClass::getCenter() {
	glm::vec3 center = glm::vec3(0.0);

	for (auto& point : points) {
		center += point;
	}

	return center / (float) pointCount;
}


//
//	OtFrustumClass::render
//

void OtFrustumClass::render(OtRenderer& renderer) {
	renderer.debugSetColor(0xff00ff00);

	renderer.debugMoveTo(points[nearBottomLeft]);
	renderer.debugLineTo(points[nearBottomRight]);
	renderer.debugLineTo(points[nearTopRight]);
	renderer.debugLineTo(points[nearTopLeft]);
	renderer.debugLineTo(points[nearBottomLeft]);

	renderer.debugMoveTo(points[farBottomLeft]);
	renderer.debugLineTo(points[farBottomRight]);
	renderer.debugLineTo(points[farTopRight]);
	renderer.debugLineTo(points[farTopLeft]);
	renderer.debugLineTo(points[farBottomLeft]);

	renderer.debugMoveTo(points[nearBottomLeft]);
	renderer.debugLineTo(points[farBottomLeft]);

	renderer.debugMoveTo(points[nearBottomRight]);
	renderer.debugLineTo(points[farBottomRight]);

	renderer.debugMoveTo(points[nearTopRight]);
	renderer.debugLineTo(points[farTopRight]);

	renderer.debugMoveTo(points[nearTopLeft]);
	renderer.debugLineTo(points[farTopLeft]);
}


//
//	OtFrustumClass::getMeta
//

OtType OtFrustumClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtFrustumClass>("Frustum", OtMathClass::getMeta());
	}

	return type;
}


//
//	OtFrustumClass::create
//

OtFrustum OtFrustumClass::create() {
	OtFrustum frustum = std::make_shared<OtFrustumClass>();
	frustum->setType(getMeta());
	return frustum;
}
