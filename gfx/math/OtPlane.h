//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"


//
//	OtPlane
//

class OtPlane {
public:
	// constructors
	OtPlane() = default;
	inline OtPlane(float a, float b, float c, float d) : normal(glm::vec3(a, b, c)), distanceToOrigin(d) {}
	inline OtPlane(const glm::vec3& n, float d) : normal(n), distanceToOrigin(d) {}
	inline OtPlane(const glm::vec4& v) : normal(v.x, v.y, v.z), distanceToOrigin(v.w) {}

	inline OtPlane(const glm::vec3& n, const glm::vec3& p) : normal(n) {
		distanceToOrigin = -glm::dot(p, n);
	}

	inline OtPlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
		normal = glm::normalize(glm::cross((p2 - p1), (p3 - p1)));
		distanceToOrigin = -glm::dot(p1, normal);
	}

	// settters/getters
	inline void setNormal(const glm::vec3& n) { normal = n; }
	inline void setDistance(float d) { distanceToOrigin = d; }

	inline glm::vec3 getNormal() { return normal; }
	inline float getDistance() { return distanceToOrigin; }
	inline glm::vec4 getVec4() { return glm::vec4(normal, distanceToOrigin); }

	// normalize this plane
	void normalize();

	// negate plane
	void negate();

	// transform plane
	OtPlane transform(const glm::mat4& transform);

	// distance to point
	inline float distance(float x, float y, float z) {
		return glm::dot(normal, glm::vec3(x, y, z)) + distanceToOrigin;
	}

	inline float distance(glm::vec3& p) {
		return glm::dot(normal, p) + distanceToOrigin;
	}

	// see if planes are identical
	inline bool operator==(OtPlane& rhs) { return normal == rhs.normal && distanceToOrigin == rhs.distanceToOrigin; }
	inline bool operator!=(OtPlane& rhs) { return !operator==(rhs); }
	inline bool operator==(const OtPlane& rhs) const { return normal == rhs.normal && distanceToOrigin == rhs.distanceToOrigin; }
	inline bool operator!=(const OtPlane& rhs) const { return !operator==(rhs); }

	// debugging support
	void debug();

private:
	// normal and distance to origin
	glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
	float distanceToOrigin = 0.0f;
};
