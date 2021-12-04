//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "boxgeometry.h"


//
//	OtBoxGeometryClass::init
//

OtObject OtBoxGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 6:
				depthSegments = parameters[5]->operator int();

			case 5:
				heightSegments = parameters[4]->operator int();

			case 4:
				widthSegments = parameters[3]->operator int();

			case 3:
				depth = parameters[2]->operator float();

			case 2:
				height = parameters[1]->operator float();

			case 1:
				width = parameters[0]->operator float();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [BoxGeometry] constructor (max 6)", count);
		}

		refreshGeometry = true;
	}

	return nullptr;
}


//
//	OtBoxGeometryClass::setWidth
//

OtObject OtBoxGeometryClass::setWidth(float w) {
	width = w;
	refreshGeometry = true;
	return shared();
}


//
//	OtBoxGeometryClass::setHeight
//

OtObject OtBoxGeometryClass::setHeight(float h) {
	height = h;
	refreshGeometry = true;
	return shared();
}


//
//	OtBoxGeometryClass::setDepth
//

OtObject OtBoxGeometryClass::setDepth(float d) {
	depth = d;
	refreshGeometry = true;
	return shared();
}


//
//	OtBoxGeometryClass::setWidthSegments
//

OtObject OtBoxGeometryClass::setWidthSegments(int ws) {
	widthSegments = ws;
	refreshGeometry = true;
	return shared();
}


//
//	OtBoxGeometryClass::setHeightSegments
//

OtObject OtBoxGeometryClass::setHeightSegments(int hs) {
	heightSegments = hs;
	refreshGeometry = true;
	return shared();
}


//
//	OtBoxGeometryClass::setSegments
//

OtObject OtBoxGeometryClass::setDepthSegments(int ds) {
	depthSegments = ds;
	refreshGeometry = true;
	return shared();
}


//
//	OtBoxGeometryClass::fillGeometry
//

void OtBoxGeometryClass::fillGeometry() {
	// default culling
	culling = true;

	// create all six planes
	buildPlane(-1, -1, depth, height,  width, depthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(w, v, u); });
	buildPlane( 1, -1, depth, height, -width, depthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(w, v, u); });
	buildPlane( 1,  1, width, depth,  height, widthSegments,  depthSegments, [](float u, float v, float w) { return glm::vec3(u, w, v); });
	buildPlane( 1, -1, width, depth, -height, widthSegments,  depthSegments, [](float u, float v, float w) { return glm::vec3(u, w, v); });
	buildPlane( 1, -1, width, height,  depth, widthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(u, v, w); });
	buildPlane(-1, -1, width, height, -depth, widthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(u, v, w); });
}


//
//	OtBoxGeometryClass::buildPlane
//

void OtBoxGeometryClass::buildPlane(int udir, int vdir, float w, float h, float d, int gridX, int gridY, glm::vec3 (*cb)(float, float, float)) {
	// add vertices
	auto segmentWidth = w / gridX;
	auto segmentHeight = h / gridY;

	auto widthHalf = w / 2.0;
	auto heightHalf = h / 2.0;
	auto depthHalf = d / 2.0;

	auto gridX1 = gridX + 1;
	auto gridY1 = gridY + 1;

	auto offset = vertices.size();

	for (auto iy = 0; iy < gridY1; iy++) {
		auto y = iy * segmentHeight - heightHalf;

		for (auto ix = 0; ix < gridX1; ix++) {
			auto x = ix * segmentWidth - widthHalf;

			addVertex(OtVertex(
				cb(x * udir, y * vdir, depthHalf),
				cb(0, 0, (d > 0) ? 1 : - 1),
				glm::vec2((float) ix / gridX, (float) iy / gridY)));
		}

	}

	// add triangles and lines
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
//	OtBoxGeometryClass::getMeta
//

OtType OtBoxGeometryClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtBoxGeometryClass>("BoxGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtBoxGeometryClass::init));
		type->set("setWidth", OtFunctionClass::create(&OtBoxGeometryClass::setWidth));
		type->set("setHeight", OtFunctionClass::create(&OtBoxGeometryClass::setHeight));
		type->set("setDepth", OtFunctionClass::create(&OtBoxGeometryClass::setDepth));
		type->set("setWidthSegments", OtFunctionClass::create(&OtBoxGeometryClass::setWidthSegments));
		type->set("setHeightSegments", OtFunctionClass::create(&OtBoxGeometryClass::setHeightSegments));
		type->set("setDepthSegments", OtFunctionClass::create(&OtBoxGeometryClass::setDepthSegments));
	}

	return type;
}


//
//	OtBoxGeometryClass::create
//

OtBoxGeometry OtBoxGeometryClass::create() {
	OtBoxGeometry boxgeometry = std::make_shared<OtBoxGeometryClass>();
	boxgeometry->setType(getMeta());
	return boxgeometry;
}
