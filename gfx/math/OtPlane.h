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
	inline OtPlane(float a, float b, float c, float d) : normal(glm::vec3(a, b, c)), distance(d) {}
	inline OtPlane(const glm::vec3& n, float d) : normal(n), distance(d) {}
	inline OtPlane(const glm::vec4& v) : normal(v.x, v.y, v.z), distance(v.w) {}

	inline OtPlane(const glm::vec3& n, const glm::vec3& p) : normal(n) {
		distance = -glm::dot(p, n);
	}

	inline OtPlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
		normal = glm::normalize(glm::cross((p2 - p1), (p3 - p1)));
		distance = -glm::dot(p1, normal);
	}

	// settters/getters
	inline void setNormal(const glm::vec3& n) { normal = n; }
	inline void setDistance(float d) { distance = d; }

	inline glm::vec3 getNormal() { return normal; }
	inline float getDistance() { return distance; }
	inline glm::vec4 getVec4() { return glm::vec4(normal, distance); }

	// normalize this plane
	void normalize();

	// negate plane
	void negate();

	// transform plane
	OtPlane transform(const glm::mat4& transform);

	// distance to other entities
	inline float distanceToPoint(float x, float y, float z) {
		return glm::dot(normal, glm::vec3(x, y, z)) + distance;
	}

	inline float distanceToVec3(glm::vec3& p) {
		return glm::dot(normal, p) + distance;
	}

	// debugging support
	void debug();

private:
	// normal and distance to origin
	glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);
	float distance = 0.0;
};
