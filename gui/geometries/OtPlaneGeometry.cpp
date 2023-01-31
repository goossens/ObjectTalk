//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtFunction.h"

#include "OtPlaneGeometry.h"


//
//	OtPlaneGeometryClass::~OtPlaneGeometryClass
//

OtPlaneGeometryClass::~OtPlaneGeometryClass() {
	if (heightmap) {
		heightmap->detach(heightmapID);
	}

	if (noisemap) {
		noisemap->detach(noisemapID);
	}
}


//
//	OtPlaneGeometryClass::init
//

void OtPlaneGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	switch (count) {
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
			OtExcept("Too many parameters [%ld] for [PlaneGeometry] constructor (max 4)", count);
	}
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
	// sanity check
	object->expectKindOf("HeightMap");

	// cleanup
	if (heightmap) {
		heightmap->detach(heightmapID);
		heightmap = nullptr;
	}

	if (noisemap) {
		noisemap->detach(noisemapID);
		noisemap = nullptr;
	}

	// set new heightmap
	heightmap = object->cast<OtHeightMapClass>();
	refreshGeometry = true;

	heightmapID = heightmap->attach([this]() {
		refreshGeometry = true;
	});

	return shared();
}


//
//	OtPlaneGeometryClass::setNoiseMap
//

OtObject OtPlaneGeometryClass::setNoiseMap(OtObject object, int xo, int yo) {
	// sanity check
	object->expectKindOf("NoiseMap");

	// cleanup
	if (heightmap) {
		heightmap->detach(heightmapID);
		heightmap = nullptr;
	}

	if (noisemap) {
		noisemap->detach(noisemapID);
		noisemap = nullptr;
	}

	// set new noisemap
	noisemap = object->cast<OtNoiseMapClass>();
	refreshGeometry = true;

	noisemapID = noisemap->attach([this]() {
		refreshGeometry = true;
	});

	xoffset = xo;
	yoffset = yo;

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
		auto y = heightHalf - iy * segmentHeight;

		for (auto ix = 0; ix < gridX1; ix++) {
			auto x = ix * segmentWidth - widthHalf;
			auto u = (float) ix / (float) widthSegments;
			auto v = (float) iy / (float) heightSegments;
			float z = 0.0;

			if (heightmap) {
				z = heightmap->getHeight(u, v);

			} else if (noisemap) {
				z = noisemap->getNoise(u * width + xoffset, v * height + yoffset);
			}

			addVertex(OtVertex(
				glm::vec3(x, y, z),
				glm::vec3(0.0, 0.0, 1.0),
				glm::vec2(u, v)));
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
//	OtPlaneGeometryClass::renderGUI
//

bool OtPlaneGeometryClass::renderGUI() {
	return false;
}


//
//	OtPlaneGeometryClass::serialize
//

nlohmann::json OtPlaneGeometryClass::serialize() {
	auto data = nlohmann::json::object();
	return data;
}


//
//	OtPlaneGeometryClass::deserialize
//

void OtPlaneGeometryClass::deserialize(nlohmann::json data) {
}


//
//	OtPlaneGeometryClass::getMeta
//

OtType OtPlaneGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtPlaneGeometryClass>("PlaneGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtPlaneGeometryClass::init));
		type->set("setWidth", OtFunctionClass::create(&OtPlaneGeometryClass::setWidth));
		type->set("setHeight", OtFunctionClass::create(&OtPlaneGeometryClass::setHeight));
		type->set("setWidthSegments", OtFunctionClass::create(&OtPlaneGeometryClass::setWidthSegments));
		type->set("setHeightSegments", OtFunctionClass::create(&OtPlaneGeometryClass::setHeightSegments));
		type->set("setHeightMap", OtFunctionClass::create(&OtPlaneGeometryClass::setHeightMap));
		type->set("setNoiseMap", OtFunctionClass::create(&OtPlaneGeometryClass::setNoiseMap));
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
