//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtPlanePrimitive.h"


//
//	OtPlanePrimitive::createMesh
//

void OtPlanePrimitive::createMesh(OtMesh* mesh) {
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

			mesh->addVertex(OtVertex(
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

			mesh->addTriangle(a, b, d);
			mesh->addTriangle(b, c, d);

			if (iy == 0) {
				mesh->addLine(a, d);
			}

			mesh->addLine(a, b);
			mesh->addLine(b, c);

			if (ix == widthSegments - 1) {
				mesh->addLine(c, d);
			}
		}
	}
}


//
//	OtPlanePrimitive::renderUI
//

bool OtPlanePrimitive::renderUI() {
	bool changed = false;
	changed |= ImGui::SliderInt("Width Segments", &widthSegments, 1, 256);
	changed |= ImGui::SliderInt("Height Segments", &heightSegments, 1, 256);
	return changed;
}


//
//	OtPlanePrimitive::serialize
//

nlohmann::json OtPlanePrimitive::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data["widthSegments"] = widthSegments;
	data["heightSegments"] = heightSegments;
	return data;
}


//
//	OtPlanePrimitive::deserialize
//

void OtPlanePrimitive::deserialize(nlohmann::json data, std::string* basedir) {
	widthSegments = data.value("widthSegments", 1);
	heightSegments = data.value("heightSegments", 1);
}
