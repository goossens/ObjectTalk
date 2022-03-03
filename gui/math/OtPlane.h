//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtMath.h"


//
//	OtPlane
//

class OtPlaneClass;
typedef std::shared_ptr<OtPlaneClass> OtPlane;

class OtPlaneClass : public OtMathClass {
public:
	// constructors
	OtPlaneClass() = default;
	OtPlaneClass(float a, float b, float c, float d) : normal(glm::vec3(a, b, c)), distance(d) {}
	OtPlaneClass(const glm::vec3& n, float d) : normal(n), distance(d) {}

	OtPlaneClass(const glm::vec4& v) {
		normal = glm::vec3(v);
		distance = v.w;
	}

	OtPlaneClass(const glm::vec3& n, const glm::vec3& p) : normal(n) {
		distance = -glm::dot(p, n);
	}

	OtPlaneClass(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
		normal = glm::normalize(glm::cross((p3 - p2), (p1 - p2)));
		distance = -glm::dot(p1, normal);
	}

	// settters/getters
	void setNormal(const glm::vec3& n) { normal = n; }
	void setDistance(float d) { distance = d; }

	glm::vec3 getNormal() { return normal; }
	float getDistance() { return distance; }

	// initialize plane
	OtObject init(size_t count, OtObject* parameters);

	// normalize this plane
	void normalize();

	// negate plane
	void negate();

	// distance to other entities
	float distanceToPoint(float x, float y, float z) {
		return glm::dot(normal, glm::vec3(x, y, z)) + distance;
	}

	float distanceToVec3(glm::vec3& p) {
		return glm::dot(normal, p) + distance;
	}

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPlane create();
	static OtPlane create(float a, float b, float c, float d);
	static OtPlane create(const glm::vec3& n, float d);
	static OtPlane create(const glm::vec4& v);
	static OtPlane create(const glm::vec3& n, const glm::vec3& p);
	static OtPlane create(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

private:
	// normal and distance to origin
	glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);
	float distance = 0.0;
};
