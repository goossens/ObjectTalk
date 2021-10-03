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

#include "torusgeometry.h"


//
//	OtTorusGeometryClass::init
//

OtObject OtTorusGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 8:
				tubularLength = parameters[7]->operator float();

			case 7:
				tubularStart = parameters[6]->operator float();

			case 6:
				radialLength = parameters[5]->operator float();

			case 5:
				radialStart = parameters[4]->operator float();

			case 4:
				tubularSegments = parameters[3]->operator int();

			case 3:
				radialSegments = parameters[2]->operator int();

			case 2:
				tubeRadius = parameters[1]->operator float();

			case 1:
				radius = parameters[0]->operator float();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [TorusGeometry] constructor (max 8)", count);
		}

		refreshGeometry = true;
	}

	return nullptr;
}


//
//	OtTorusGeometryClass::setRadius
//

OtObject OtTorusGeometryClass::setRadius(float r) {
	radius = r;
	refreshGeometry = true;
	return shared();
}


//
//	OtTorusGeometryClass::setTubeRadius
//

OtObject OtTorusGeometryClass::setTubeRadius(float tr) {
	tubeRadius = tr;
	refreshGeometry = true;
	return shared();
}


//
//	OtTorusGeometryClass::setRadialSegments
//

OtObject OtTorusGeometryClass::setRadialSegments(int segments) {
	radialSegments = segments;
	refreshGeometry = true;
	return shared();
}


//
//	OtTorusGeometryClass::setTubularSegments
//

OtObject OtTorusGeometryClass::setTubularSegments(int segments) {
	tubularSegments = segments;
	refreshGeometry = true;
	return shared();
}


//
//	OtTorusGeometryClass::setRadialStart
//

OtObject OtTorusGeometryClass::setRadialStart(float rs) {
	radialStart = rs;
	refreshGeometry = true;
	return shared();
}


//
//	OtTorusGeometryClass::setRadialLength
//

OtObject OtTorusGeometryClass::setRadialLength(float rl) {
	radialLength = rl;
	refreshGeometry = true;
	return shared();
}


//
//	OtTorusGeometryClass::setTubularStart
//

OtObject OtTorusGeometryClass::setTubularStart(float ts) {
	tubularStart = ts;
	refreshGeometry = true;
	return shared();
}


//
//	OtTorusGeometryClass::setTubularLength
//

OtObject OtTorusGeometryClass::setTubularLength(float tl) {
	tubularLength = tl;
	refreshGeometry = true;
	return shared();
}


//
//	OtTorusGeometryClass::fillGeometry
//

void OtTorusGeometryClass::fillGeometry() {
	// default culling
	culling = tubularLength == std::numbers::pi * 2.0 && radialLength == std::numbers::pi;

	// generate vertices
	for (auto j = 0; j <= radialSegments; j++) {
		for (auto i = 0; i <= tubularSegments; i++) {
			auto u = radialStart + (float) j / radialSegments * radialLength;
			auto v = tubularStart + (float) i / tubularSegments * tubularLength;

			auto x = (radius + tubeRadius * std::cos(v)) * std::cos(u);
			auto y = (radius + tubeRadius * std::cos(v)) * std::sin(u);
			auto z = tubeRadius * std::sin(v);

			glm::vec3 pos = glm::vec3(x, y, z);
			glm::vec3 center = glm::vec3(radius * std::cos(u), radius * std::sin(u), 0.0);
			glm::vec3 normal = glm::normalize(pos - center);

			// add vertex
			addVertex(OtVertex(
				glm::vec3(x, y, z),
				glm::normalize(glm::vec3(x, y, z) - glm::vec3(radius * std::cos(u), radius * std::sin(u), 0.0)),
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

			if (i == 1) {
				addLine(a, b);
			}

			addLine(b, c);
			addLine(c, d);
			addLine(d, a);
		}
	}
}


//
//	OtTorusGeometryClass::getMeta
//

OtType OtTorusGeometryClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtTorusGeometryClass>("TorusGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTorusGeometryClass::init));
		type->set("setRadius", OtFunctionClass::create(&OtTorusGeometryClass::setRadius));
		type->set("setTubeRadius", OtFunctionClass::create(&OtTorusGeometryClass::setTubeRadius));
		type->set("setRadialSegments", OtFunctionClass::create(&OtTorusGeometryClass::setRadialSegments));
		type->set("setTubularSegments", OtFunctionClass::create(&OtTorusGeometryClass::setTubularSegments));
		type->set("setRadialStart", OtFunctionClass::create(&OtTorusGeometryClass::setRadialStart));
		type->set("setRadialLength", OtFunctionClass::create(&OtTorusGeometryClass::setRadialLength));
		type->set("setTubularStart", OtFunctionClass::create(&OtTorusGeometryClass::setTubularStart));
		type->set("setTubularLength", OtFunctionClass::create(&OtTorusGeometryClass::setTubularLength));
	}

	return type;
}


//
//	OtTorusGeometryClass::create
//

OtTorusGeometry OtTorusGeometryClass::create() {
	OtTorusGeometry torusgeometry = std::make_shared<OtTorusGeometryClass>();
	torusgeometry->setType(getMeta());
	return torusgeometry;
}
