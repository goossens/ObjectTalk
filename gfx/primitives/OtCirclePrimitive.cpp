//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "nlohmann/json.hpp"

#include "OtCirclePrimitive.h"
#include "OtUi.h"


//
//	OtCirclePrimitive::createMesh
//

void OtCirclePrimitive::createMesh(OtMesh* mesh) {
	// add center
	mesh->addVertex(OtVertex(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::vec2(0.5f, 0.5f)));

	// add points on circle
	float delta = thetaLength / segments;

	for (auto c = 0; c <= segments; c++) {
		auto angle = glm::radians(thetaStart + c * delta);
		auto x = std::cos(angle);
		auto y = std::sin(angle);

		mesh->addVertex(OtVertex(
			glm::vec3(x, y, 0),
			glm::normalize(glm::vec3(0, 0, 1)),
			glm::vec2((x + 1.0f) / 2.0f, 1.0f - ((y + 1.0f) / 2.0f))));
	}

	// add triangles
	for (auto c = 1; c <= segments; c++) {
		mesh->addTriangle(c, c + 1, 0);
	}
}


//
//	OtCirclePrimitive::renderUI
//

bool OtCirclePrimitive::renderUI() {
	bool changed = false;
	changed |= OtUiDragInt("Segments", &segments, 1, 50);
	changed |= OtUiDragFloat("Theta Start", &thetaStart, 0.0f, 360.0f);
	changed |= OtUiDragFloat("Theta Length", &thetaLength, 0.0f, 360.0f);
	return changed;
}


//
//	OtCirclePrimitive::serialize
//

nlohmann::json OtCirclePrimitive::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data["segments"] = segments;
	data["thetaStart"] = thetaStart;
	data["thetaLength"] = thetaLength;
	return data;
}


//
//	OtCirclePrimitive::deserialize
//

void OtCirclePrimitive::deserialize(nlohmann::json data, std::string* basedir) {
	segments = data.value("segments", 32);
	thetaStart = data.value("thetaStart", 0.0f);
	thetaLength = data.value("thetaLength", 360.0f);
}
