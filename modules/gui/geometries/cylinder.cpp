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
//	OtCylinderClass::OtCylinderClass()
//

OtCylinderClass::OtCylinderClass() {
	createTriangles();
}


//
//	OtCylinderClass::init
//

OtObject OtCylinderClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 5:
				end = parameters[4]->operator double();

			case 4:
				start = parameters[3]->operator double();

			case 3:
				numSegments = parameters[2]->operator int();

			case 2:
				height = parameters[1]->operator double();

			case 1:
				radius = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Cylinder] contructor (max 5)", count);
		}

		createTriangles();
	}

	return nullptr;
}


//
//	OtCylinderClass::createTriangles
//

void OtCylinderClass::createTriangles() {
	// clear vertex/triangle lists
	clear();

	// get increment
	float segDelta = (end - start) / numSegments;

	// address each segment
	for (auto seg = 0; seg <= numSegments; seg++) {
		auto x0 = radius * -std::sinf(start + seg * segDelta);
		auto z0 = radius * -std::cosf(start + seg * segDelta);

		// add a new vertices
		addVertex(OtVertex(
			glm::vec3(x0, height / 2.0, z0),
			glm::normalize(glm::vec3(x0, 0.0, z0)),
			glm::vec2((float) seg / (float) numSegments, 0.0)));

		addVertex(OtVertex(
			glm::vec3(x0, -height / 2.0, z0),
			glm::normalize(glm::vec3(x0, 0.0, z0)),
			glm::vec2((float) seg / (float) numSegments, 1.0)));
	}

	// add triangles
	for (auto seg = 0; seg < numSegments; seg++) {
		auto a = seg * 2;
		auto b = a + 1;
		auto d = (seg + 1) * 2;
		auto c = d + 1;

		addTriangle(a, b, d);
		addTriangle(b, c, d);
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
