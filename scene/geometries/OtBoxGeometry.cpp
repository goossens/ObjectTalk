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

#include "OtBoxGeometry.h"


//
//	OtBoxGeometryClass::init
//

void OtBoxGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 3:
				depthSegments = parameters[5]->operator int();

			case 2:
				heightSegments = parameters[4]->operator int();

			case 1:
				widthSegments = parameters[3]->operator int();

			default:
				OtError("Too many parameters [%ld] for [BoxGeometry] constructor (max 3)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtBoxGeometryClass::setWidthSegments
//

OtObject OtBoxGeometryClass::setWidthSegments(int ws) {
	widthSegments = ws;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtBoxGeometryClass::setHeightSegments
//

OtObject OtBoxGeometryClass::setHeightSegments(int hs) {
	heightSegments = hs;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtBoxGeometryClass::setSegments
//

OtObject OtBoxGeometryClass::setDepthSegments(int ds) {
	depthSegments = ds;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtBoxGeometryClass::renderGUI
//

bool OtBoxGeometryClass::renderGUI() {
	bool changed = false;
	changed |= ImGui::SliderInt("X Segments", &widthSegments, 1, 20);
	changed |= ImGui::SliderInt("Y Segments", &heightSegments, 1, 20);
	changed |= ImGui::SliderInt("Z Segments", &depthSegments, 1, 20);
	changed |= OtGeometryClass::renderGUI();

	if (changed) {
		refreshGeometry = true;
	}

	return changed;
}


//
//	OtBoxGeometryClass::serialize
//

nlohmann::json OtBoxGeometryClass::serialize(std::filesystem::path* basedir) {
	auto data = OtGeometryClass::serialize(basedir);
	data["type"] = name;
	data["xsegments"] = widthSegments;
	data["ysegments"] = heightSegments;
	data["zsegments"] = depthSegments;
	return data;
}


//
//	OtBoxGeometryClass::deserialize
//

void OtBoxGeometryClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	OtGeometryClass::deserialize(data, basedir);
	widthSegments = data.value("xsegments", 1);
	heightSegments = data.value("ysegments", 1);
	depthSegments = data.value("zsegments", 1);
	refreshGeometry = true;
}


//
//	OtBoxGeometryClass::fillGeometry
//

void OtBoxGeometryClass::fillGeometry() {
	// create all six planes
	buildPlane(-1, -1, 1.0f, 1.0f,  1.0f, depthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(w, v, u); });
	buildPlane( 1, -1, 1.0f, 1.0f, -1.0f, depthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(w, v, u); });
	buildPlane( 1,	1, 1.0f, 1.0f,  1.0f, widthSegments,  depthSegments, [](float u, float v, float w) { return glm::vec3(u, w, v); });
	buildPlane( 1, -1, 1.0f, 1.0f, -1.0f, widthSegments,  depthSegments, [](float u, float v, float w) { return glm::vec3(u, w, v); });
	buildPlane( 1, -1, 1.0f, 1.0f,  1.0f, widthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(u, v, w); });
	buildPlane(-1, -1, 1.0f, 1.0f, -1.0f, widthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(u, v, w); });
}


//
//	OtBoxGeometryClass::buildPlane
//

void OtBoxGeometryClass::buildPlane(int udir, int vdir, float w, float h, float d, int gridX, int gridY, glm::vec3 (*cb)(float, float, float)) {
	// add vertices
	auto segmentWidth = w / gridX;
	auto segmentHeight = h / gridY;

	auto widthHalf = w / 2.0f;
	auto heightHalf = h / 2.0f;
	auto depthHalf = d / 2.0f;

	auto gridX1 = gridX + 1;
	auto gridY1 = gridY + 1;

	auto offset = (uint32_t) vertices.size();

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
	static OtType type;

	if (!type) {
		type = OtType::create<OtBoxGeometryClass>("BoxGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunction::create(&OtBoxGeometryClass::init));
		type->set("setWidthSegments", OtFunction::create(&OtBoxGeometryClass::setWidthSegments));
		type->set("setHeightSegments", OtFunction::create(&OtBoxGeometryClass::setHeightSegments));
		type->set("setDepthSegments", OtFunction::create(&OtBoxGeometryClass::setDepthSegments));
	}

	return type;
}
