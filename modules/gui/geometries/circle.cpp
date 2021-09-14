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
				OtExcept("Too many parameters [%ld] for [Circle] constructor (max 4)", count);
		}

		refreshGeometry = true;
	}

	return nullptr;
}


//
//	OtCircleClass::setRadius
//

OtObject OtCircleClass::setRadius(double r) {
	radius = r;
	refreshGeometry = true;
	return shared();
}


//
//	OtCircleClass::setSegments
//

OtObject OtCircleClass::setSegments(int s) {
	segments = s;
	refreshGeometry = true;
	return shared();
}


//
//	OtCircleClass::setThetaStart
//

OtObject OtCircleClass::setThetaStart(double ts) {
	thetaStart = ts;
	refreshGeometry = true;
	return shared();
}


//
//	OtCircleClass::setThetaLength
//

OtObject OtCircleClass::setThetaLength(double tl) {
	thetaLength = tl;
	refreshGeometry = true;
	return shared();
}


//
//	OtCircleClass::fillGeometry
//

void OtCircleClass::fillGeometry() {
	// add center
	addVertex(OtVertex(
		glm::vec3(0, 0, 0),
		glm::normalize(glm::vec3(0, 0, 1)),
		glm::vec2(0.5, 0.5)));

	// add points on circle
	float delta = thetaLength / segments;

	for (auto c = 0; c <= segments; c++) {
		auto angle = thetaStart + c * delta;
		auto x = std::cos(angle);
		auto y = std::sin(angle);

		addVertex(OtVertex(
			glm::vec3(radius * x, radius * y, 0),
			glm::normalize(glm::vec3(0, 0, 1)),
			glm::vec2((x + 1.0) / 2.0, 1.0 - ((y + 1.0) / 2.0))));
	}

	// add triangles and lines
	for (auto c = 1; c <= segments; c++) {
		addTriangle(c, c + 1, 0);

		addLine(0, c);
		addLine(c, c + 1);

		if (c == segments) {
			addLine(0, c + 1);
		}
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
		type->set("setRadius", OtFunctionClass::create(&OtCircleClass::setRadius));
		type->set("setSegments", OtFunctionClass::create(&OtCircleClass::setSegments));
		type->set("setThetaStart", OtFunctionClass::create(&OtCircleClass::setThetaStart));
		type->set("setThetaLength", OtFunctionClass::create(&OtCircleClass::setThetaLength));
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
