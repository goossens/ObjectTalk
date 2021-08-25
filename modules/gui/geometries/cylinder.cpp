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

#include "cylinder.h"


//
//	OtCylinderClass::init
//

OtObject OtCylinderClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 5:
				thetaLength = parameters[4]->operator double();

			case 4:
				thetaStart = parameters[3]->operator double();

			case 3:
				segments = parameters[2]->operator int();

			case 2:
				height = parameters[1]->operator double();

			case 1:
				radius = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Cylinder] contructor (max 5)", count);
		}

		refreshBuffers = true;
	}

	return nullptr;
}


//
//	OtCylinderClass::setRadius
//

OtObject OtCylinderClass::setRadius(double r) {
	radius = r;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setHeight
//

OtObject OtCylinderClass::setHeight(double h) {
	height = h;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setSegments
//

OtObject OtCylinderClass::setSegments(int s) {
	segments = s;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setPartial
//

OtObject OtCylinderClass::setPartial(double ts, double tl) {
	thetaStart = ts;
	thetaLength = tl;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::fillBuffers
//

void OtCylinderClass::fillBuffers() {
	// clear geometry
	clear();

	// get increment
	float delta = thetaLength / segments;

	// address each segment
	for (auto seg = 0; seg <= segments; seg++) {
		auto x0 = radius * -std::sinf(thetaStart + seg * delta);
		auto z0 = radius * -std::cosf(thetaStart + seg * delta);

		// add a new vertices
		addVertex(OtVertex(
			glm::vec3(x0, height / 2.0, z0),
			glm::normalize(glm::vec3(x0, 0.0, z0)),
			glm::vec2((float) seg / (float) segments, 0.0)));

		addVertex(OtVertex(
			glm::vec3(x0, -height / 2.0, z0),
			glm::normalize(glm::vec3(x0, 0.0, z0)),
			glm::vec2((float) seg / (float) segments, 1.0)));
	}

	// add triangles and lines
	for (auto seg = 0; seg < segments; seg++) {
		auto a = seg * 2;
		auto b = a + 1;
		auto d = (seg + 1) * 2;
		auto c = d + 1;

		addTriangle(a, b, d);
		addTriangle(b, c, d);

		if (seg == 0) {
			addLine(a, b);
		}

		addLine(a, d);
		addLine(b, c);
		addLine(c, d);
	}
}


//
//	OtCylinderClass::getMeta
//

OtType OtCylinderClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCylinderClass>("Cylinder", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtCylinderClass::init));
		type->set("setRadius", OtFunctionClass::create(&OtCylinderClass::setRadius));
		type->set("setHeight", OtFunctionClass::create(&OtCylinderClass::setHeight));
		type->set("setSegments", OtFunctionClass::create(&OtCylinderClass::setSegments));
		type->set("setPartial", OtFunctionClass::create(&OtCylinderClass::setPartial));
	}

	return type;
}


//
//	OtCylinderClass::create
//

OtCylinder OtCylinderClass::create() {
	OtCylinder cylinder = std::make_shared<OtCylinderClass>();
	cylinder->setType(getMeta());
	return cylinder;
}

OtCylinder OtCylinderClass::create(double radius, double height, long segments) {
	OtCylinder cylinder = create();
	cylinder->setRadius(radius);
	cylinder->setHeight(height);
	cylinder->setSegments(segments);
	return cylinder;
}
