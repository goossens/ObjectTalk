//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "ot/function.h"

#include "spheregeometry.h"


//
//	OtSphereGeometryClass::init
//

OtObject OtSphereGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 7:
				thetaLength = parameters[6]->operator float();

			case 6:
				thetaStart = parameters[5]->operator float();

			case 5:
				phiLength = parameters[4]->operator float();

			case 4:
				phiStart = parameters[3]->operator float();

			case 3:
				heightSegments = parameters[2]->operator int();

			case 2:
				widthSegments = parameters[1]->operator int();

			case 1:
				radius = parameters[0]->operator float();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [SphereGeometry] constructor (max 7)", count);
		}

		refreshGeometry = true;
	}

	return nullptr;
}


//
//	OtSphereGeometryClass::setRadius
//

OtObject OtSphereGeometryClass::setRadius(float r) {
	radius = r;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setWidthSegments
//

OtObject OtSphereGeometryClass::setWidthSegments(int segments) {
	widthSegments = segments;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setHeightSegments
//

OtObject OtSphereGeometryClass::setHeightSegments(int segments) {
	heightSegments = segments;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setPhiStart
//

OtObject OtSphereGeometryClass::setPhiStart(float ps) {
	phiStart = ps;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setPhiLength
//

OtObject OtSphereGeometryClass::setPhiLength(float pl) {
	phiLength = pl;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setThetaStart
//

OtObject OtSphereGeometryClass::setThetaStart(float ts) {
	thetaStart = ts;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setThetaLength
//

OtObject OtSphereGeometryClass::setThetaLength(float tl) {
	thetaLength = tl;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::fillGeometry
//

void OtSphereGeometryClass::fillGeometry() {
	// default culling
	culling = true;

	// get increments
	float ringDelta = thetaLength / heightSegments;
	float segDelta = phiLength / widthSegments;

	// address each ring
	for (auto ring = 0; ring <= heightSegments; ring++) {
		auto theta = thetaStart + ring * ringDelta;
		auto r0 = radius * std::sin(theta);
		auto y0 = radius * std::cos(theta);

		// address each segment
		for (auto seg = 0; seg <= widthSegments; seg++) {
			auto phi = phiStart + seg * segDelta;
			auto x0 = r0 * -std::sin(phi);
			auto z0 = r0 * -std::cos(phi);

			// add vertex
			addVertex(OtVertex(
				glm::vec3(x0, y0, z0),
				glm::normalize(glm::vec3(x0, y0, z0)),
				glm::vec2((float) seg / (float) widthSegments, (float) ring / (float) heightSegments)));
		}
	}

	// add triangles and lines
	for (auto ring = 0; ring < heightSegments; ring++) {
		for (auto seg = 0; seg < widthSegments; seg++) {
			auto a = ring * (widthSegments + 1) + seg;
			auto b = a + (widthSegments + 1);
			auto c = b + 1;
			auto d = a + 1;

			addTriangle(a, b, d);
			addTriangle(b, c, d);

			if (ring == 0) {
				addLine(a, d);
			}

			addLine(a, b);
			addLine(b, c);
		}
	}
}


//
//	OtSphereGeometryClass::getMeta
//

OtType OtSphereGeometryClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtSphereGeometryClass>("SphereGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtSphereGeometryClass::init));
		type->set("setRadius", OtFunctionClass::create(&OtSphereGeometryClass::setRadius));
		type->set("setWidthSegments", OtFunctionClass::create(&OtSphereGeometryClass::setWidthSegments));
		type->set("setHeightSegments", OtFunctionClass::create(&OtSphereGeometryClass::setHeightSegments));
		type->set("setPhiStart", OtFunctionClass::create(&OtSphereGeometryClass::setPhiStart));
		type->set("setPhiLength", OtFunctionClass::create(&OtSphereGeometryClass::setPhiLength));
		type->set("setThetaStart", OtFunctionClass::create(&OtSphereGeometryClass::setThetaStart));
		type->set("setThetaLength", OtFunctionClass::create(&OtSphereGeometryClass::setThetaLength));
	}

	return type;
}


//
//	OtSphereGeometryClass::create
//

OtSphereGeometry OtSphereGeometryClass::create() {
	OtSphereGeometry spheregeometry = std::make_shared<OtSphereGeometryClass>();
	spheregeometry->setType(getMeta());
	return spheregeometry;
}
