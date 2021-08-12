//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "ot/function.h"

#include "circle.h"


//
//	OtCircleClass::OtCircleClass()
//

OtCircleClass::OtCircleClass() {
	createTriangles();
}


//
//	OtCircleClass::init
//

OtObject OtCircleClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 4:
				thetaLength = parameters[3]->operator double();

			case 3:
				thetaStart = parameters[2]->operator double();

			case 2:
				segments = parameters[1]->operator int();

			case 1:
				radius = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Circle] contructor (max 4)", count);
		}

		createTriangles();
	}

	return nullptr;
}


//
//	OtCircleClass::createTriangles
//

void OtCircleClass::createTriangles() {
	// clear vertex/triangle lists
	clear();

	// add center
	addVertex(OtVertex(
		glm::vec3(0, 0, 0),
		glm::normalize(glm::vec3(0, 0, 1)),
		glm::vec2(0.5, 0.5)));

	// add circle points
	float delta = thetaLength / segments;

	for (auto c = 0; c <= segments; c++) {
		auto angle = thetaStart + c * delta;
		auto x = std::cosf(angle);
		auto y = std::sinf(angle);

		addVertex(OtVertex(
			glm::vec3(radius * x, radius * y, 0),
			glm::normalize(glm::vec3(0, 0, 1)),
			glm::vec2((x + 1.0) / 2.0, 1.0 - ((y + 1.0) / 2.0))));
	}

	// add all triangles
	for (auto c = 1; c <= segments; c++) {
		addTriangle(c, c + 1, 0);
	}
}


//
//	OtCircleClass::getMeta
//

OtType OtCircleClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCircleClass>("Circle", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtCircleClass::init));
	}

	return type;
}


//
//	OtCircleClass::create
//

OtCircle OtCircleClass::create() {
	OtCircle circle = std::make_shared<OtCircleClass>();
	circle->setType(getMeta());
	return circle;
}
