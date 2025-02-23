//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtGridPrimitive.h"
#include "OtUi.h"


//
//	OtGridPrimitive::createMesh
//

void OtGridPrimitive::createMesh(OtMesh* mesh) {
	// add vertices
	for (auto z = 0; z <= depthSegments; z++) {
		auto v = static_cast<float>(z) / depthSegments;

		for (auto x = 0; x <= widthSegments; x++) {
			auto u = static_cast<float>(x) / widthSegments;

			mesh->addVertex(OtVertex(
				glm::vec3(u - 0.5f, 0.0f, v - 0.5f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec2(u, v)));
		}
	}

	// add triangles
	for (auto z = 0; z < depthSegments; z++) {
		for (auto x = 0; x < widthSegments; x++) {
			auto a = z * (widthSegments + 1) + x;
			auto b = a + 1;
			auto c = a + widthSegments + 1;
			auto d = c + 1;

			mesh->addTriangle(a, c, d);
			mesh->addTriangle(a, d, b);
		}
	}
}


//
//	OtGridPrimitive::renderUI
//

bool OtGridPrimitive::renderUI() {
	bool changed = false;
	changed |= OtUi::dragInt("Width Segments", &widthSegments, 1, 256);
	changed |= OtUi::dragInt("Depth Segments", &depthSegments, 1, 256);
	return changed;
}


//
//	OtGridPrimitive::serialize
//

nlohmann::json OtGridPrimitive::serialize(std::string* /* basedir */) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data["widthSegments"] = widthSegments;
	data["depthSegments"] = depthSegments;
	return data;
}


//
//	OtGridPrimitive::deserialize
//

void OtGridPrimitive::deserialize(nlohmann::json data, std::string* /* basedir */) {
	widthSegments = data.value("widthSegments", 1);
	depthSegments = data.value("depthSegments", 1);
}
