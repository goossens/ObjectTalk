//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "terrain.h"


//
//	OtTerrainClass::init
//

OtObject OtTerrainClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 5:
			setHeightMap(parameters[4]);

			case 4:
				depthSegments = parameters[3]->operator int();

			case 3:
				widthSegments = parameters[2]->operator int();

			case 2:
				depth = parameters[1]->operator double();

			case 1:
				width = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Terrain] constructor (max 4)", count);
		}

		refreshGeometry = true;
	}

	return nullptr;
}


//
//	OtTerrainClass::setWidth
//

OtObject OtTerrainClass::setWidth(double w) {
	width = w;
	refreshGeometry = true;
	return shared();
}


//
//	OtTerrainClass::setDepth
//

OtObject OtTerrainClass::setDepth(double d) {
	depth = d;
	refreshGeometry = true;
	return shared();
}


//
//	OtTerrainClass::setWidthSegments
//

OtObject OtTerrainClass::setWidthSegments(int ws) {
	widthSegments = ws;
	refreshGeometry = true;
	return shared();
}


//
//	OtTerrainClass::setDepthSegments
//

OtObject OtTerrainClass::setDepthSegments(int ds) {
	depthSegments = ds;
	refreshGeometry = true;
	return shared();
}


//
//	OtTerrainClass::setHeightMap
//

OtObject OtTerrainClass::setHeightMap(OtObject object) {
	// ensure object is a material
	if (object->isKindOf("HeightMap")) {
		heightmap = object->cast<OtHeightMapClass>();

	} else {
		OtExcept("Expected a [HeightMap] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtTerrainClass::fillGeometry
//

void OtTerrainClass::fillGeometry() {
	// sanity checks
	if (!heightmap) {
		OtExcept("No [HeightMap] specified for [Terrain]");
	}

	// add vertices
	auto widthHalf = width / 2.0;
	auto depthHalf = depth / 2.0;

	auto gridX1 = widthSegments + 1;
	auto gridZ1 = depthSegments + 1;

	auto segmentWidth = width / widthSegments;
	auto segmentDepth = depth / depthSegments;

	for (auto iz = 0; iz < gridZ1; iz++) {
		auto z = iz * segmentDepth - depthHalf;

		for (auto ix = 0; ix < gridX1; ix++) {
			auto x = ix * segmentWidth - widthHalf;
			auto u = (float) ix / widthSegments;
			auto v = (float) iz / depthSegments;

			addVertex(OtVertex(
				glm::vec3(x, heightmap->getHeight(u, v), -z),
				heightmap->getNormal(u, v),
				glm::vec2(u, -v)));
		}
	}

	// add triangles and lines
	for (auto iz = 0; iz < depthSegments; iz++) {
		for (auto ix = 0; ix < widthSegments; ix++) {
			auto a = ix + gridX1 * iz;
			auto b = ix + gridX1 * (iz + 1);
			auto c = (ix + 1) + gridX1 * (iz + 1);
			auto d = (ix + 1) + gridX1 * iz;

			addTriangle(a, b, d);
			addTriangle(b, c, d);

			if (iz == 0) {
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
//	OtTerrainClass::getMeta
//

OtType OtTerrainClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtTerrainClass>("Terrain", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTerrainClass::init));
		type->set("setWidth", OtFunctionClass::create(&OtTerrainClass::setWidth));
		type->set("setDepth", OtFunctionClass::create(&OtTerrainClass::setDepth));
		type->set("setWidthSegments", OtFunctionClass::create(&OtTerrainClass::setWidthSegments));
		type->set("setDepthSegments", OtFunctionClass::create(&OtTerrainClass::setDepthSegments));
		type->set("setHeightMap", OtFunctionClass::create(&OtTerrainClass::setHeightMap));
	}

	return type;
}


//
//	OtTerrainClass::create
//

OtTerrain OtTerrainClass::create() {
	OtTerrain plane = std::make_shared<OtTerrainClass>();
	plane->setType(getMeta());
	return plane;
}
