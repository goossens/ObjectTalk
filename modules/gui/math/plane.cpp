//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/function.h"

#include "plane.h"


//
//	OtPlaneClass::init
//

OtObject OtPlaneClass::init(size_t count, OtObject* parameters) {
	if (count == 2) {
		// TODO

	} else if (count == 3) {
		// TODO

	} else if (count == 4) {
		normal = glm::vec3(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		d = parameters[3]->operator float();

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
	d /= l;
}


//
//	OtPlaneClass::negate
//

void OtPlaneClass::negate() {
	normal *= -1;
	d *= -1;
}


//
//	OtPlaneClass::distanceToPoint
//

float OtPlaneClass::distanceToPoint(float x, float y, float z) {
	return glm::dot(normal, glm::vec3(x, y, z)) + d;
}


//
//	OtPlaneClass::getMeta
//

OtType OtPlaneClass::getMeta() {
	static OtType type = nullptr;

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
