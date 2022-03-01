//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "debugdraw.h"
#include "glm/glm.hpp"

#include "OtRenderingContext.h"


//
//	OtFrustum
//

class OtFrustum
{
public:
	// constructors
	OtFrustum() = default;

	OtFrustum(const glm::mat4& matrix) {
		// determine planes
		glm::mat4 m = glm::transpose(matrix);
		planes[left] = m[3] + m[0];
		planes[right] = m[3] - m[0];
		planes[bottom] = m[3] + m[1];
		planes[top] = m[3] - m[1];
		planes[near] = bgfx::getCaps()->homogeneousDepth ? m[3] + m[2] : m[2];
		planes[far] = m[3] - m[2];

		// normalize planes
		for (auto c = 0; c < planeCount; c++) {
			planes[c] /= glm::length(glm::vec3(planes[c]));
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

	// see if AABB box is visible
	bool isVisibleAABB(const glm::vec3& minp, const glm::vec3& maxp) const {
		// check box outside/inside of frustum
		for (auto i = 0; i < planeCount; i++) {
			if ((glm::dot(planes[i], glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
				(glm::dot(planes[i], glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
				(glm::dot(planes[i], glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
				(glm::dot(planes[i], glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
				(glm::dot(planes[i], glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
				(glm::dot(planes[i], glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
				(glm::dot(planes[i], glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
				(glm::dot(planes[i], glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0)) {
					return false;
			}
		}

		// check frustum outside/inside box
		int out;
		out = 0; for (auto i = 0; i < pointCount; i++) out += ((points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (auto i = 0; i < pointCount; i++) out += ((points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (auto i = 0; i < pointCount; i++) out += ((points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (auto i = 0; i < pointCount; i++) out += ((points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (auto i = 0; i < pointCount; i++) out += ((points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
		out = 0; for (auto i = 0; i < pointCount; i++) out += ((points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

		return true;
	}

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

	glm::vec4 planes[planeCount];
	glm::vec3 points[pointCount];
};
