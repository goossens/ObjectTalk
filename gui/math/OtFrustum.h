//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>

#include "bgfx/bgfx.h"
#include "debugdraw.h"
#include "glm/glm.hpp"

#include "OtMath.h"
#include "OtPlane.h"


//
//	OtFrustum
//

class OtFrustumClass;
typedef std::shared_ptr<OtFrustumClass> OtFrustum;

class OtFrustumClass : public OtMathClass {
public:
	void update(const glm::mat4& matrix) {
		// determine planes
		glm::mat4 m = glm::transpose(matrix);
		planes[left] = OtPlaneClass::create(m[3] + m[0]);
		planes[right] = OtPlaneClass::create(m[3] - m[0]);
		planes[bottom] = OtPlaneClass::create(m[3] + m[1]);
		planes[top] = OtPlaneClass::create(m[3] - m[1]);
		planes[near] = OtPlaneClass::create(bgfx::getCaps()->homogeneousDepth ? m[3] + m[2] : m[2]);
		planes[far] = OtPlaneClass::create(m[3] - m[2]);

		// normalize planes
		for (auto c = 0; c < planeCount; c++) {
			planes[c]->normalize();
		}

		// determine corners
		glm::mat4 inverse = glm::inverse(matrix);
		float n = bgfx::getCaps()->homogeneousDepth ? -1.0 : 0.0;

		points[nearBottomLeft] = project(inverse, glm::vec4(-1.0, -1.0, n, 1.0));
		points[nearTopLeft] = project(inverse, glm::vec4(-1.0, 1.0, n, 1.0));
		points[nearTopRight] = project(inverse, glm::vec4(1.0, 1.0, n, 1.0));
		points[nearBottomRight] = project(inverse, glm::vec4(1.0, -1.0, n, 1.0));

		points[farBottomLeft] = project(inverse, glm::vec4(-1.0, -1.0, 1.0, 1.0));
		points[farTopLeft] = project(inverse, glm::vec4(-1.0, 1.0, 1.0, 1.0));
		points[farTopRight] = project(inverse, glm::vec4(1.0, 1.0, 1.0, 1.0));
		points[farBottomRight] = project(inverse, glm::vec4(1.0, -1.0, 1.0, 1.0));
	}

	// see if a point is visible
	bool isVisiblePoint(const glm::vec3& point) {
		for (auto i = 0; i < planeCount; i++) {
			if (glm::dot(planes[i]->getNormal(), point) + planes[i]->getDistance() < 0) {
				return false;
			 }
		}

		return true;
	}

	// see if an AABB box is visible
	bool isVisibleAABB(OtAABB aabb) const {
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

	// see if a sphere is visible
	bool isVisibleSphere(const glm::vec3& center, float radius) {
		for (auto i = 0; i < planeCount; i++) {
			if (glm::dot(planes[i]->getNormal(), center) + planes[i]->getDistance() + radius < 0) {
				return false;
			}
		}

		return true;
	}

	// render the frustum for debugging purposes
	void render(DebugDrawEncoder* debugDraw) {
		debugDraw->setColor(0xff00ff00);

		debugDraw->moveTo(points[nearBottomLeft].x, points[nearBottomLeft].y, points[nearBottomLeft].z);
		debugDraw->lineTo(points[nearBottomRight].x, points[nearBottomRight].y, points[nearBottomRight].z);
		debugDraw->lineTo(points[nearTopRight].x, points[nearTopRight].y, points[nearTopRight].z);
		debugDraw->lineTo(points[nearTopLeft].x, points[nearTopLeft].y, points[nearTopLeft].z);
		debugDraw->close();

		debugDraw->moveTo(points[farBottomLeft].x, points[farBottomLeft].y, points[farBottomLeft].z);
		debugDraw->lineTo(points[farBottomRight].x, points[farBottomRight].y, points[farBottomRight].z);
		debugDraw->lineTo(points[farTopRight].x, points[farTopRight].y, points[farTopRight].z);
		debugDraw->lineTo(points[farTopLeft].x, points[farTopLeft].y, points[farTopLeft].z);
		debugDraw->close();

		debugDraw->moveTo(points[nearBottomLeft].x, points[nearBottomLeft].y, points[nearBottomLeft].z);
		debugDraw->lineTo(points[farBottomLeft].x, points[farBottomLeft].y, points[farBottomLeft].z);

		debugDraw->moveTo(points[nearBottomRight].x, points[nearBottomRight].y, points[nearBottomRight].z);
		debugDraw->lineTo(points[farBottomRight].x, points[farBottomRight].y, points[farBottomRight].z);

		debugDraw->moveTo(points[nearTopRight].x, points[nearTopRight].y, points[nearTopRight].z);
		debugDraw->lineTo(points[farTopRight].x, points[farTopRight].y, points[farTopRight].z);

		debugDraw->moveTo(points[nearTopLeft].x, points[nearTopLeft].y, points[nearTopLeft].z);
		debugDraw->lineTo(points[farTopLeft].x, points[farTopLeft].y, points[farTopLeft].z);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type;

		if (!type) {
			type = OtTypeClass::create<OtFrustumClass>("Frustum", OtMathClass::getMeta());
		}

		return type;
	}

	// create a new object
	static OtFrustum create() {
		OtFrustum frustum = std::make_shared<OtFrustumClass>();
		frustum->setType(getMeta());
		return frustum;
	}

private:
	static inline glm::vec4 project(const glm::mat4& matrix, const glm::vec4& ndc) {
		glm::vec4 v = matrix * ndc;
		return glm::vec4(v.x / v.w, v.y / v.w, v.z / v.w, 1.0f);
	}

	enum Planes {
		left = 0,
		right,
		bottom,
		top,
		near,
		far,
		planeCount
	};

	enum Points {
		nearBottomLeft = 0,
		nearTopLeft,
		nearTopRight,
		nearBottomRight,
		farBottomLeft,
		farTopLeft,
		farTopRight,
		farBottomRight,
		pointCount
	};

	std::array<OtPlane, planeCount> planes;
	glm::vec3 points[pointCount];
};
