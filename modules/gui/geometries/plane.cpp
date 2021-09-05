//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "plane.h"


//
//	OtPlaneClass::init
//

OtObject OtPlaneClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 4:
				heightSegments = parameters[3]->operator int();

			case 3:
				widthSegments = parameters[2]->operator int();

			case 2:
				height = parameters[1]->operator double();

			case 1:
				width = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Plane] constructor (max 4)", count);
		}

		refreshGeometry = true;
	}

	return nullptr;
}


//
//	OtPlaneClass::setWidth
//

OtObject OtPlaneClass::setWidth(double w) {
	width = w;
	refreshGeometry = true;
	return shared();
}


//
//	OtPlaneClass::setHeight
//

OtObject OtPlaneClass::setHeight(double h) {
	height = h;
	refreshGeometry = true;
	return shared();
}


//
//	OtPlaneClass::setWidthSegments
//

OtObject OtPlaneClass::setWidthSegments(int ws) {
	widthSegments = ws;
	refreshGeometry = true;
	return shared();
}


//
//	OtPlaneClass::setHeightSegments
//

OtObject OtPlaneClass::setHeightSegments(int hs) {
	heightSegments = hs;
	refreshGeometry = true;
	return shared();
}


//
//	OtPlaneClass::fillGeometry
//

void OtPlaneClass::fillGeometry() {
	// add vertices
	auto widthHalf = width / 2.0;
	auto heightHalf = height / 2.0;

	auto gridX1 = widthSegments + 1;
	auto gridY1 = heightSegments + 1;

	auto segmentWidth = width / widthSegments;
	auto segmentHeight = height / heightSegments;

	for (auto iy = 0; iy < gridY1; iy++) {
		auto y = iy * segmentHeight - heightHalf;

		for (auto ix = 0; ix < gridX1; ix++) {
			auto x = ix * segmentWidth - widthHalf;

			addVertex(OtVertex(
				glm::vec3(x, -y, 0.0),
				glm::vec3(0, 0, 1),
				glm::vec2((float) ix / widthSegments, (float) iy / heightSegments)));
		}
	}

	// add triangles and lines
	for (auto iy = 0; iy < heightSegments; iy++) {
		for (auto ix = 0; ix < widthSegments; ix++) {
			auto a = ix + gridX1 * iy;
			auto b = ix + gridX1 * (iy + 1);
			auto c = (ix + 1) + gridX1 * (iy + 1);
			auto d = (ix + 1) + gridX1 * iy;

			addTriangle(a, b, d);
			addTriangle(b, c, d);

			if (iy == 0) {
				addLine(a, d);
			}

			addLine(a, b);
			addLine(b, c);

			if (ix == widthSegments - 1) {
				addLine(c, d);
			}
		}
	}
}


//
//	OtPlaneClass::getMeta
//

OtType OtPlaneClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtPlaneClass>("Plane", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtPlaneClass::init));
		type->set("setWidth", OtFunctionClass::create(&OtPlaneClass::setWidth));
		type->set("setHeight", OtFunctionClass::create(&OtPlaneClass::setHeight));
		type->set("setWidthSegments", OtFunctionClass::create(&OtPlaneClass::setWidthSegments));
		type->set("setHeightSegments", OtFunctionClass::create(&OtPlaneClass::setHeightSegments));
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
