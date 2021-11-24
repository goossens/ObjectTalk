//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "gui.h"


//
//
//

class OtFrustum
{
public:
	// constructors
	OtFrustum() {}

	OtFrustum(glm::mat4 m) {
		m = glm::transpose(m);
		planes[LEFT]   = m[3] + m[0];
		planes[RIGHT]  = m[3] - m[0];
		planes[BOTTOM] = m[3] + m[1];
		planes[TOP]    = m[3] - m[1];
		planes[NEAR]   = m[3] + m[2];
		planes[FAR]    = m[3] - m[2];

		glm::vec3 crosses[COMBINATIONS] = {
			glm::cross(glm::vec3(planes[LEFT]),   glm::vec3(planes[RIGHT])),
			glm::cross(glm::vec3(planes[LEFT]),   glm::vec3(planes[BOTTOM])),
			glm::cross(glm::vec3(planes[LEFT]),   glm::vec3(planes[TOP])),
			glm::cross(glm::vec3(planes[LEFT]),   glm::vec3(planes[NEAR])),
			glm::cross(glm::vec3(planes[LEFT]),   glm::vec3(planes[FAR])),
			glm::cross(glm::vec3(planes[RIGHT]),  glm::vec3(planes[BOTTOM])),
			glm::cross(glm::vec3(planes[RIGHT]),  glm::vec3(planes[TOP])),
			glm::cross(glm::vec3(planes[RIGHT]),  glm::vec3(planes[NEAR])),
			glm::cross(glm::vec3(planes[RIGHT]),  glm::vec3(planes[FAR])),
			glm::cross(glm::vec3(planes[BOTTOM]), glm::vec3(planes[TOP])),
			glm::cross(glm::vec3(planes[BOTTOM]), glm::vec3(planes[NEAR])),
			glm::cross(glm::vec3(planes[BOTTOM]), glm::vec3(planes[FAR])),
			glm::cross(glm::vec3(planes[TOP]),    glm::vec3(planes[NEAR])),
			glm::cross(glm::vec3(planes[TOP]),    glm::vec3(planes[FAR])),
			glm::cross(glm::vec3(planes[NEAR]),   glm::vec3(planes[FAR]))
		};

		points[0] = intersection<LEFT,  BOTTOM, NEAR>(crosses);
		points[1] = intersection<LEFT,  TOP,    NEAR>(crosses);
		points[2] = intersection<RIGHT, BOTTOM, NEAR>(crosses);
		points[3] = intersection<RIGHT, TOP,    NEAR>(crosses);
		points[4] = intersection<LEFT,  BOTTOM, FAR>(crosses);
		points[5] = intersection<LEFT,  TOP,    FAR>(crosses);
		points[6] = intersection<RIGHT, BOTTOM, FAR>(crosses);
		points[7] = intersection<RIGHT, TOP,    FAR>(crosses);
	}

	// see if AABB box is visible
	bool isVisibleAABB(const glm::vec3& minp, const glm::vec3& maxp) const {
		// check box outside/inside of frustum
		for (auto i = 0; i < COUNT; i++) {
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
		out = 0; for (int i = 0; i<8; i++) out += ((points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i<8; i++) out += ((points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i<8; i++) out += ((points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i<8; i++) out += ((points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i<8; i++) out += ((points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i<8; i++) out += ((points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

		return true;
	}

private:
	enum Planes {
		LEFT = 0,
		RIGHT,
		BOTTOM,
		TOP,
		NEAR,
		FAR,
		COUNT,
		COMBINATIONS = COUNT * (COUNT - 1) / 2
	};

	template<Planes i, Planes j>
	struct ij2k {
		enum { k = i * (9 - i) / 2 + j - 1 };
	};

	template<Planes a, Planes b, Planes c>
	glm::vec3 intersection(const glm::vec3* crosses) const {
		float D = glm::dot(glm::vec3(planes[a]), crosses[ij2k<b, c>::k]);

		glm::vec3 res =
			glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
			glm::vec3(planes[a].w, planes[b].w, planes[c].w);

		return res * (-1.0f / D);
	}

	glm::vec4 planes[COUNT];
	glm::vec3 points[8];
};
