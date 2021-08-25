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

#include "cone.h"


//
//	OtConeClass::init
//

OtObject OtConeClass::init(size_t count, OtObject* parameters) {
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
				OtExcept("Too many parameters [%ld] for [Cone] contructor (max 5)", count);
		}

		refreshBuffers = true;
	}

	return nullptr;
}


//
//	OtConeClass::setRadius
//

OtObject OtConeClass::setRadius(double r) {
	radius = r;
	refreshBuffers = true;
	return shared();
}


//
//	OtConeClass::setHeight
//

OtObject OtConeClass::setHeight(double h) {
	height = h;
	refreshBuffers = true;
	return shared();
}


//
//	OtConeClass::setSegments
//

OtObject OtConeClass::setSegments(int s) {
	segments = s;
	refreshBuffers = true;
	return shared();
}


//
//	OtConeClass::setPartial
//

OtObject OtConeClass::setPartial(double ts, double tl) {
	thetaStart = ts;
	thetaLength = tl;
	refreshBuffers = true;
	return shared();
}


//
//	OtConeClass::fillBuffers
//

void OtConeClass::fillBuffers() {
	// clear geometry
	clear();

	// get increment
	float delta = thetaLength / segments;

	// address each segment
	for (auto seg = 0; seg <= segments; seg++) {
		auto angle = thetaStart + seg * delta;
		auto x0 = radius * -std::sinf(angle);
		auto z0 = radius * -std::cosf(angle);

		// add a new vertices
		addVertex(OtVertex(
			glm::vec3(0.0, height, 0.0),
			glm::normalize(glm::vec3(0.0, height, 0.0)),
			glm::vec2((float) seg / (float) segments, 0.0)));

		addVertex(OtVertex(
			glm::vec3(x0, 0.0, z0),
			glm::normalize(glm::vec3(x0, 0.0, z0)),
			glm::vec2((float) seg / (float) segments, 1.0)));
	}

	// add triangles and lines
	for (auto seg = 0; seg < segments; seg++) {
		auto a = seg * 2;
		auto b = seg * 2 + 1;
		auto c = (seg + 1) * 2 + 1;
		auto d = (seg + 1) * 2;

		addTriangle(a, b, d);
		addTriangle(b, c, d);

		if (seg == 0) {
			addLine(a, b);
		}

		addLine(b, c);
		addLine(c, d);
	}
}


//
//	OtConeClass::getMeta
//

OtType OtConeClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtConeClass>("Cone", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtConeClass::init));
		type->set("setRadius", OtFunctionClass::create(&OtConeClass::setRadius));
		type->set("setHeight", OtFunctionClass::create(&OtConeClass::setHeight));
		type->set("setSegments", OtFunctionClass::create(&OtConeClass::setSegments));
		type->set("setPartial", OtFunctionClass::create(&OtConeClass::setPartial));
	}

	return type;
}


//
//	OtConeClass::create
//

OtCone OtConeClass::create() {
	OtCone cone = std::make_shared<OtConeClass>();
	cone->setType(getMeta());
	return cone;
}

OtCone OtConeClass::create(double radius, double height, long segments) {
	OtCone cone = create();
	cone->setRadius(radius);
	cone->setHeight(height);
	cone->setSegments(segments);
	return cone;
}
