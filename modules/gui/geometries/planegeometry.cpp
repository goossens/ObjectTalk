//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "planegeometry.h"


//
//	OtPlaneGeometryClass::init
//

OtObject OtPlaneGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	switch (count) {
		case 5:
			setHeightMap(parameters[4]);

		case 4:
			heightSegments = parameters[3]->operator int();

		case 3:
			widthSegments = parameters[2]->operator int();

		case 2:
			height = parameters[1]->operator float();

		case 1:
			width = parameters[0]->operator float();

		case 0:
			break;

		default:
			OtExcept("Too many parameters [%ld] for [PlaneGeometry] constructor (max 5)", count);
	}

	return nullptr;
}


//
//	OtPlaneGeometryClass::setWidth
//

OtObject OtPlaneGeometryClass::setWidth(float w) {
	width = w;
	refreshGeometry = true;
	return shared();
}


//
//	OtPlaneGeometryClass::setHeight
//

OtObject OtPlaneGeometryClass::setHeight(float h) {
	height = h;
	refreshGeometry = true;
	return shared();
}


//
//	OtPlaneGeometryClass::setWidthSegments
//

OtObject OtPlaneGeometryClass::setWidthSegments(int ws) {
	widthSegments = ws;
	refreshGeometry = true;
	return shared();
}


//
//	OtPlaneGeometryClass::setHeightSegments
//

OtObject OtPlaneGeometryClass::setHeightSegments(int hs) {
	heightSegments = hs;
	refreshGeometry = true;
	return shared();
}


//
//	OtPlaneGeometryClass::setHeightMap
//

OtObject OtPlaneGeometryClass::setHeightMap(OtObject object) {
	// ensure object is a material
	if (object->isKindOf("HeightMap")) {
		heightmap = object->cast<OtHeightMapClass>();

	} else {
		OtExcept("Expected a [HeightMap] object, not a [%s]", object->getType()->getName().c_str());
	}

	refreshGeometry = true;
	return shared();
}


//
//	OtPlaneGeometryClass::fillGeometry
//

void OtPlaneGeometryClass::fillGeometry() {
	// add vertices
	auto widthHalf = width / 2.0;
	auto heightHalf = height / 2.0;

	auto gridX1 = widthSegments + 1;
	auto gridY1 = heightSegments + 1;

	auto segmentWidth = width / (float) widthSegments;
	auto segmentHeight = height / (float) heightSegments;

	for (auto iy = 0; iy < gridY1; iy++) {
		auto y = iy * segmentHeight - heightHalf;

		for (auto ix = 0; ix < gridX1; ix++) {
			auto x = ix * segmentWidth - widthHalf;
			auto u = (float) ix / (float) widthSegments;
			auto v = (float) iy / (float) heightSegments;

			if (heightmap) {
				addVertex(OtVertex(
					glm::vec3(x, heightmap->getHeight(u, v), y),
					heightmap->getNormal(u, v),
					glm::vec2(u, v)));

			} else {
				addVertex(OtVertex(
					glm::vec3(x, -y, 0.0),
					glm::vec3(0.0, 0.0, 1.0),
					glm::vec2(u, v)));
			}
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
//	OtPlaneGeometryClass::getMeta
//

OtType OtPlaneGeometryClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtPlaneGeometryClass>("PlaneGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtPlaneGeometryClass::init));
		type->set("setWidth", OtFunctionClass::create(&OtPlaneGeometryClass::setWidth));
		type->set("setHeight", OtFunctionClass::create(&OtPlaneGeometryClass::setHeight));
		type->set("setWidthSegments", OtFunctionClass::create(&OtPlaneGeometryClass::setWidthSegments));
		type->set("setHeightSegments", OtFunctionClass::create(&OtPlaneGeometryClass::setHeightSegments));
		type->set("setHeightMap", OtFunctionClass::create(&OtPlaneGeometryClass::setHeightMap));
	}

	return type;
}


//
//	OtPlaneGeometryClass::create
//

OtPlaneGeometry OtPlaneGeometryClass::create() {
	OtPlaneGeometry planegeometry = std::make_shared<OtPlaneGeometryClass>();
	planegeometry->setType(getMeta());
	return planegeometry;
}
