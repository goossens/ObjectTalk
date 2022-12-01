//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtPlane.h"


//
//	OtPlaneClass::init
//

void OtPlaneClass::init(size_t count, OtObject* parameters) {
	if (count == 2) {
		// TODO

	} else if (count == 3) {
		// TODO

	} else if (count == 4) {
		normal = glm::vec3(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		distance = parameters[3]->operator float();

	} else if (count != 0) {
		OtExcept("Invalid number of constructor parameters for [Plane]");
	}
}


//
//	OtPlaneClass::normalize
//

void OtPlaneClass::normalize() {
	auto l = glm::length(normal);
	normal /= l;
	distance /= l;
}


//
//	OtPlaneClass::negate
//

void OtPlaneClass::negate() {
	normal *= -1;
	distance *= -1;
}


//
//	OtPlaneClass::getMeta
//

OtType OtPlaneClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtPlaneClass>("Plane", OtMathClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtPlaneClass::init));

		type->set("normalize", OtFunctionClass::create(&OtPlaneClass::normalize));
		type->set("negate", OtFunctionClass::create(&OtPlaneClass::negate));
		type->set("distanceToPoint", OtFunctionClass::create(&OtPlaneClass::distanceToPoint));
	}

	return type;
}


//
//	OtPlaneClass::create
//

OtPlane OtPlaneClass::create() {
	OtPlane plane = std::make_shared<OtPlaneClass>();
	plane->setType(getMeta());
	return plane;
}

OtPlane OtPlaneClass::create(float a, float b, float c, float d) {
	OtPlane plane = std::make_shared<OtPlaneClass>(a, b, c, d);
	plane->setType(getMeta());
	return plane;
}

OtPlane OtPlaneClass::create(const glm::vec3& n, float d) {
	OtPlane plane = std::make_shared<OtPlaneClass>(n, d);
	plane->setType(getMeta());
	return plane;
}

OtPlane OtPlaneClass::create(const glm::vec4& v) {
	OtPlane plane = std::make_shared<OtPlaneClass>(v);
	plane->setType(getMeta());
	return plane;
}

OtPlane OtPlaneClass::create(const glm::vec3& n, const glm::vec3& p) {
	OtPlane plane = std::make_shared<OtPlaneClass>(n, p);
	plane->setType(getMeta());
	return plane;
}

OtPlane OtPlaneClass::create(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
	OtPlane plane = std::make_shared<OtPlaneClass>(p1, p2, p3);
	plane->setType(getMeta());
	return plane;
}
