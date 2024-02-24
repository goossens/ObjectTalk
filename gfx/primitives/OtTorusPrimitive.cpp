//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtTorusPrimitive.h"


//
//	OtTorusPrimitive::createMesh
//

void OtTorusPrimitive::createMesh(OtMesh* mesh) {
	// generate vertices
	for (auto j = 0; j <= radialSegments; j++) {
		for (auto i = 0; i <= tubularSegments; i++) {
			auto u = glm::radians(radialStart + (float) j / radialSegments * radialLength);
			auto v = glm::radians(tubularStart + (float) i / tubularSegments * tubularLength);

			auto x = (radius + tubeRadius * std::cos(v)) * std::cos(u);
			auto y = (radius + tubeRadius * std::cos(v)) * std::sin(u);
			auto z = tubeRadius * std::sin(v);

			// add vertex
			mesh->addVertex(OtVertex(
				glm::vec3(x, y, z),
				glm::normalize(glm::vec3(x, y, z) - glm::vec3(radius * std::cos(u), radius * std::sin(u), 0.0)),
				glm::vec2((float) i / (float) tubularSegments, (float) j / (float) radialSegments)));
		}
	}

	// add triangles and lines
	for (auto j = 1; j <= radialSegments; j++) {
		for (auto i = 1; i <= tubularSegments; i++) {
			auto a = (tubularSegments + 1) * j + i - 1;
			auto b = (tubularSegments + 1) * (j - 1) + i - 1;
			auto c = (tubularSegments + 1) * (j - 1) + i;
			auto d = (tubularSegments + 1) * j + i;

			mesh->addTriangle(a, b, d);
			mesh->addTriangle(b, c, d);

			if (i == 1) {
				mesh->addLine(a, b);
			}

			mesh->addLine(b, c);
			mesh->addLine(c, d);
			mesh->addLine(d, a);
		}
	}
}


//
//	OtTorusPrimitive::renderUI
//

bool OtTorusPrimitive::renderUI() {
	bool changed = false;
	changed |= ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f);
	changed |= ImGui::SliderFloat("Tube Radius", &tubeRadius, 0.0f, 1.0f);
	changed |= ImGui::SliderInt("Radial Segments", &radialSegments, 1, 64);
	changed |= ImGui::SliderInt("Tubular Segments", &tubularSegments, 1, 32);
	changed |= ImGui::SliderFloat("Radial Start", &radialStart, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat("Radial Length", &radialLength, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat("Tubular Start", &tubularStart, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat("Tubular Length", &tubularLength, 0.0f, 360.0f);
	return changed;
}


//
//	OtTorusPrimitive::serialize
//

nlohmann::json OtTorusPrimitive::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data["radius"] = radius;
	data["tubeRadius"] = tubeRadius;
	data["radialSegments"] = radialSegments;
	data["tubularSegments"] = tubularSegments;
	data["radialStart"] = radialStart;
	data["radialLength"] = radialLength;
	data["tubularStart"] = tubularStart;
	data["tubularLength"] = tubularLength;
	return data;
}


//
//	OtTorusPrimitive::deserialize
//

void OtTorusPrimitive::deserialize(nlohmann::json data, std::string* basedir) {
	radius = data.value("radius", 1.0f);
	tubeRadius = data.value("tubeRadius", 0.4f);
	radialSegments = data.value("radialSegments", 64);
	tubularSegments = data.value("tubularSegments", 32);
	radialStart = data.value("radialStart", 0.0f);
	radialLength = data.value("radialLength", 360.0f);
	tubularStart = data.value("tubularStart", 0.0f);
	tubularLength = data.value("tubularLength", 360.0f);
}
