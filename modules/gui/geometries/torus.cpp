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

#include "torus.h"


//
//	OtTorusClass::OtTorusClass()
//

OtTorusClass::OtTorusClass() {
	createTriangles();
}


//
//	OtTorusClass::init
//

double radius = 1.0;
double tube = 1.0;
int radialSegments = 8;
int tubularSegments = 6;
double arc = std::numbers::pi * 2.0;

OtObject OtTorusClass::init(size_t count, OtObject* parameters) {
	// default culling
	culling = true;

	// set attributes
	if (count) {
		switch (count) {
			case 5:
				arc = parameters[4]->operator double();
				culling = false;

			case 4:
				tubularSegments = parameters[3]->operator int();

			case 3:
				radialSegments = parameters[2]->operator int();

			case 2:
				tube = parameters[1]->operator double();

			case 1:
				radius = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Torus] contructor (max 5)", count);
		}

		createTriangles();
	}

	return nullptr;
}


//
//	OtTorusClass::createTriangles
//

void OtTorusClass::createTriangles() {
	// clear vertex/triangle lists
	clear();

	// generate vertices
	for (auto j = 0; j <= radialSegments; j++) {
		for (auto i = 0; i <= tubularSegments; i++) {
			auto u = (float) i / tubularSegments * arc;
			auto v = (float) j / radialSegments * std::numbers::pi * 2.0;

			auto x = (radius + tube * std::cosf(v)) * std::cosf(u);
			auto y = (radius + tube * std::cosf(v)) * std::sinf(u);
			auto z = tube * std::sinf(v);

			glm::vec3 pos = glm::vec3(x, y, z);
			glm::vec3 center = glm::vec3(radius * std::cosf(u), radius * std::sinf(u), 0.0);
			glm::vec3 normal = glm::normalize(pos - center);

			// add vertex
			addVertex(OtVertex(
				glm::vec3(x, y, z),
				glm::normalize(glm::vec3(x, y, z) - glm::vec3(radius * std::cosf(u), radius * std::sinf(u), 0.0)),
				glm::vec2((float) i / (float) tubularSegments, (float) j / (float) radialSegments)));
		}
	}

	// add triangles
	for (auto j = 1; j <= radialSegments; j++) {
		for (auto i = 1; i <= tubularSegments; i++) {
			auto a = (tubularSegments + 1) * j + i - 1;
			auto b = (tubularSegments + 1) * (j - 1) + i - 1;
			auto c = (tubularSegments + 1) * (j - 1) + i;
			auto d = (tubularSegments + 1) * j + i;

			addTriangle(a, b, d);
			addTriangle(b, c, d);
		}
	}
}


//
//	OtTorusClass::getMeta
//

OtType OtTorusClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtTorusClass>("Torus", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTorusClass::init));
	}

	return type;
}


//
//	OtTorusClass::create
//

OtTorus OtTorusClass::create() {
	OtTorus torus = std::make_shared<OtTorusClass>();
	torus->setType(getMeta());
	return torus;
}
