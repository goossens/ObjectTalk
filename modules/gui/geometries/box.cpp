//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "box.h"


//
//	OtBoxClass::init
//

OtObject OtBoxClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 6:
				depthSegments = parameters[6]->operator int();

			case 5:
				heightSegments = parameters[5]->operator int();

			case 4:
				widthSegments = parameters[3]->operator int();

			case 3:
				height = parameters[2]->operator double();

			case 2:
				height = parameters[1]->operator double();

			case 1:
				width = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Box] contructor (max 6)", count);
		}

		refreshBuffers = true;
	}

	return nullptr;
}


//
//	OtBoxClass::setSize
//

OtObject OtBoxClass::setSize(double w, double h, double d) {
	width = w;
	height = h;
	depth = d;
	refreshBuffers = true;
	return shared();
}


//
//	OtBoxClass::setSegments
//

OtObject OtBoxClass::setSegments(int ws, int hs, int ds) {
	widthSegments = ws;
	heightSegments = hs;
	depthSegments = ds;
	refreshBuffers = true;
	return shared();
}


//
//	OtBoxClass::fillBuffers
//

void OtBoxClass::fillBuffers() {
	// clear geometry
	clear();

	// default culling
	culling = true;

	// create all six planes
	buildPlane(0, -1, -1, depth, height,  width, depthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(w, v, u); });
	buildPlane(1,  1, -1, depth, height, -width, depthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(w, v, u); });
	buildPlane(2,  1,  1, width, depth,  height, widthSegments,  depthSegments, [](float u, float v, float w) { return glm::vec3(u, w, v); });
	buildPlane(3,  1, -1, width, depth, -height, widthSegments,  depthSegments, [](float u, float v, float w) { return glm::vec3(u, w, v); });
	buildPlane(4,  1, -1, width, height,  depth, widthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(u, v, w); });
	buildPlane(5, -1, -1, width, height, -depth, widthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(u, v, w); });
}


//
//	OtBoxClass::buildPlane
//

void OtBoxClass::buildPlane(int side, int udir, int vdir, double w, double h, double d, int gridX, int gridY, glm::vec3 (*cb)(float, float, float)) {
	// add vertices
	auto segmentWidth = w / gridX;
	auto segmentHeight = h / gridY;

	auto widthHalf = w / 2.0;
	auto heightHalf = h / 2.0;
	auto depthHalf = d / 2.0;

	auto gridX1 = gridX + 1;
	auto gridY1 = gridY + 1;

	for (auto iy = 0; iy < gridY1; iy++) {
		auto y = iy * segmentHeight - heightHalf;

		for (auto ix = 0; ix < gridX1; ix++) {
			auto x = ix * segmentWidth - widthHalf;

			addVertex(OtVertex(
				cb(x * udir, y * vdir, depthHalf),
				cb(0, 0, (depth > 0) ? 1 : - 1),
				glm::vec2((float) ix / gridX, 1.0 - ((float) iy / gridY))));
		}

	}

	// add triangles and lines
	auto offset = side * gridX1 * gridY1;

	for (auto iy = 0; iy < gridY; iy++) {
		for (auto ix = 0; ix < gridX; ix++) {
			auto a = offset + ix + gridX1 * iy;
			auto b = offset + ix + gridX1 * (iy + 1);
			auto c = offset + (ix + 1) + gridX1 * (iy + 1);
			auto d = offset + (ix + 1) + gridX1 * iy;

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
//	OtBoxClass::getMeta
//

OtType OtBoxClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtBoxClass>("Box", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtBoxClass::init));
		type->set("setSize", OtFunctionClass::create(&OtBoxClass::setSize));
		type->set("setSegments", OtFunctionClass::create(&OtBoxClass::setSegments));
	}

	return type;
}


//
//	OtBoxClass::create
//

OtBox OtBoxClass::create() {
	OtBox box = std::make_shared<OtBoxClass>();
	box->setType(getMeta());
	return box;
}

OtBox OtBoxClass::create(double width, double height, double depth) {
	OtBox box = create();
	box->setSize(width, height, depth);
	return box;
}
