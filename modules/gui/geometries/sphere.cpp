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

#include "sphere.h"


//
//	OtSphereClass::OtSphereClass()
//

OtSphereClass::OtSphereClass() {
	createTriangles();
}


//
//	OtSphereClass::init
//

OtObject OtSphereClass::init(size_t count, OtObject* parameters) {
	// default culling
	culling = true;

	// set attributes
	if (count) {
		switch (count) {
			case 7:
				thetaLength = parameters[6]->operator double();

			case 6:
				thetaStart = parameters[5]->operator double();

			case 5:
				phiLength = parameters[4]->operator double();

			case 4:
				phiStart = parameters[3]->operator double();
				culling = false;

			case 3:
				heightSegments = parameters[2]->operator int();

			case 2:
				widthSegments = parameters[1]->operator int();

			case 1:
				radius = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Sphere] contructor (max 7)", count);
		}

		createTriangles();
	}

	return nullptr;
}


//
//	OtSphereClass::createTriangles
//

void OtSphereClass::createTriangles() {
	// clear vertex/triangle lists
	clear();

	// get increments
	float ringDelta = thetaLength / heightSegments;
	float segDelta = phiLength / widthSegments;

	// address each ring
	for (auto ring = 0; ring <= heightSegments; ring++) {
		auto theta = thetaStart + ring * ringDelta;
		auto r0 = radius * std::sinf(theta);
		auto y0 = radius * std::cosf(theta);

		// address each segment
		for (auto seg = 0; seg <= widthSegments; seg++) {
			auto phi = phiStart + seg * segDelta;
			auto x0 = r0 * -std::sinf(phi);
			auto z0 = r0 * -std::cosf(phi);

			// add vertex
			addVertex(OtVertex(
				glm::vec3(x0, y0, z0),
				glm::normalize(glm::vec3(x0, y0, z0)),
				glm::vec2((float) seg / (float) widthSegments, (float) ring / (float) heightSegments)));
		}
	}

	// add triangles
	for (auto ring = 0; ring < heightSegments; ring++) {
		for (auto seg = 0; seg < widthSegments; seg++) {
			auto a = ring * (widthSegments + 1) + seg;
			auto b = a + (widthSegments + 1);
			auto c = b + 1;
			auto d = a + 1;

			addTriangle(a, b, d);
			addTriangle(b, c, d);
		}
	}
}


//
//	OtSphereClass::getMeta
//

OtType OtSphereClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtSphereClass>("Sphere", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtSphereClass::init));
	}

	return type;
}


//
//	OtSphereClass::create
//

OtSphere OtSphereClass::create() {
	OtSphere sphere = std::make_shared<OtSphereClass>();
	sphere->setType(getMeta());
	return sphere;
}
