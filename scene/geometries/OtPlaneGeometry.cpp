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
//	OtPlaneGeometryClass::init
//

void OtPlaneGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	switch (count) {
		case 2:
			heightSegments = parameters[3]->operator int();

		case 1:
			widthSegments = parameters[2]->operator int();

		case 0:
			break;

		default:
			OtError("Too many parameters [%ld] for [PlaneGeometry] constructor (max 2)", count);
	}
}


//
//	OtPlaneGeometryClass::setWidthSegments
//

OtObject OtPlaneGeometryClass::setWidthSegments(int ws) {
	widthSegments = ws;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtPlaneGeometryClass::setHeightSegments
//

OtObject OtPlaneGeometryClass::setHeightSegments(int hs) {
	heightSegments = hs;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtPlaneGeometryClass::fillGeometry
//

void OtPlaneGeometryClass::fillGeometry() {
	// add vertices
	auto gridX1 = widthSegments + 1;
	auto gridY1 = heightSegments + 1;

	auto segmentWidth = 1.0f / (float) widthSegments;
	auto segmentHeight = 1.0f / (float) heightSegments;

	for (auto iy = 0; iy < gridY1; iy++) {
		auto y = 0.5f - iy * segmentHeight;

		for (auto ix = 0; ix < gridX1; ix++) {
			auto x = ix * segmentWidth - 0.5f;
			auto u = (float) ix / (float) widthSegments;
			auto v = (float) iy / (float) heightSegments;

			addVertex(OtVertex(
				glm::vec3(x, y, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f),
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
	bool changed = false;
	changed |= ImGui::SliderInt("Width Segments", &widthSegments, 1, 256);
	changed |= ImGui::SliderInt("Height Segments", &heightSegments, 1, 256);
	changed |= OtGeometryClass::renderGUI();

	if (changed) {
		refreshGeometry = true;
	}

	return changed;
}


//
//	OtPlaneGeometryClass::serialize
//

nlohmann::json OtPlaneGeometryClass::serialize(std::filesystem::path* basedir) {
	auto data = OtGeometryClass::serialize(basedir);
	data["type"] = name;
	data["widthSegments"] = widthSegments;
	data["heightSegments"] = heightSegments;
	return data;
}


//
//	OtPlaneGeometryClass::deserialize
//

void OtPlaneGeometryClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	OtGeometryClass::deserialize(data, basedir);
	widthSegments = data.value("widthSegments", 1);
	heightSegments = data.value("heightSegments", 1);
	refreshGeometry = true;
}


//
//	OtPlaneGeometryClass::getMeta
//

OtType OtPlaneGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPlaneGeometryClass>("PlaneGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunction::create(&OtPlaneGeometryClass::init));
		type->set("setWidthSegments", OtFunction::create(&OtPlaneGeometryClass::setWidthSegments));
		type->set("setHeightSegments", OtFunction::create(&OtPlaneGeometryClass::setHeightSegments));
	}

	return type;
}
