//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "guimath.h"


//
//	OtPlane
//

class OtPlaneClass;
typedef std::shared_ptr<OtPlaneClass> OtPlane;

class OtPlaneClass : public OtMathClass {
public:
	// constructors
	OtPlaneClass() {}
	OtPlaneClass(float a, float b, float c, float _d) : normal(glm::vec3(a, b, c)), d(_d) {}
	OtPlaneClass(const glm::vec3& n, float _d) : normal(n), d(_d) {}

	OtPlaneClass(const glm::vec3& n, const glm::vec3& p) : normal(n) {
		d = -glm::dot(p, n);
	}

	OtPlaneClass(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
		normal = glm::normalize(glm::cross((p3 - p2), (p1 - p2)));
		d = -glm::dot(p1, normal);
	}

	// initialize plane
	OtObject init(size_t count, OtObject* parameters);

	// normalize this plane
	void normalize();

	// negate plane
	void negate();

	// distance to ither entities
	float distanceToPoint(float x, float y, float z);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPlane create();

private:
	// normal and distance to origin
	glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);
	float d;
};
