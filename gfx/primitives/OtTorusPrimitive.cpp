//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "nlohmann/json.hpp"

#include "OtTorusPrimitive.h"
#include "OtUi.h"


//
//	OtTorusPrimitive::createMesh
//

void OtTorusPrimitive::createMesh(OtMesh* mesh) {
	// generate vertices
	for (auto j = 0; j <= radialSegments; j++) {
		for (auto i = 0; i <= tubularSegments; i++) {
			auto u = glm::radians(radialStart + static_cast<float>(j) / radialSegments * radialLength);
			auto v = glm::radians(tubularStart + static_cast<float>(i) / tubularSegments * tubularLength);

			auto x = (radius + tubeRadius * std::cos(v)) * std::cos(u);
			auto y = (radius + tubeRadius * std::cos(v)) * std::sin(u);
			auto z = tubeRadius * std::sin(v);

			// add vertex
			mesh->addVertex(OtVertex(
				glm::vec3(x, y, z),
				glm::normalize(glm::vec3(x, y, z) - glm::vec3(radius * std::cos(u), radius * std::sin(u), 0.0)),
				glm::vec2(
					static_cast<float>(i) / static_cast<float>(tubularSegments),
					static_cast<float>(j) / static_cast<float>(radialSegments))));
		}
	}

	// add triangles
	for (auto j = 1; j <= radialSegments; j++) {
		for (auto i = 1; i <= tubularSegments; i++) {
			auto a = (tubularSegments + 1) * j + i - 1;
			auto b = (tubularSegments + 1) * (j - 1) + i - 1;
			auto c = (tubularSegments + 1) * (j - 1) + i;
			auto d = (tubularSegments + 1) * j + i;

			mesh->addTriangle(a, b, d);
			mesh->addTriangle(b, c, d);
		}
	}
}


//
//	OtTorusPrimitive::renderUI
//

bool OtTorusPrimitive::renderUI() {
	bool changed = false;
	changed |= OtUi::dragFloat("Radius", &radius, 0.0f, 1.0f);
	changed |= OtUi::dragFloat("Tube Radius", &tubeRadius, 0.0f, 1.0f);
	changed |= OtUi::dragInt("Radial Segments", &radialSegments, 1, 64);
	changed |= OtUi::dragInt("Tubular Segments", &tubularSegments, 1, 32);
	changed |= OtUi::dragFloat("Radial Start", &radialStart, 0.0f, 360.0f);
	changed |= OtUi::dragFloat("Radial Length", &radialLength, 0.0f, 360.0f);
	changed |= OtUi::dragFloat("Tubular Start", &tubularStart, 0.0f, 360.0f);
	changed |= OtUi::dragFloat("Tubular Length", &tubularLength, 0.0f, 360.0f);
	return changed;
}


//
//	OtTorusPrimitive::serialize
//

nlohmann::json OtTorusPrimitive::serialize([[maybe_unused]] std::string* basedir) {
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

void OtTorusPrimitive::deserialize(nlohmann::json data, [[maybe_unused]] std::string* basedir) {
	radius = data.value("radius", 1.0f);
	tubeRadius = data.value("tubeRadius", 0.4f);
	radialSegments = data.value("radialSegments", 64);
	tubularSegments = data.value("tubularSegments", 32);
	radialStart = data.value("radialStart", 0.0f);
	radialLength = data.value("radialLength", 360.0f);
	tubularStart = data.value("tubularStart", 0.0f);
	tubularLength = data.value("tubularLength", 360.0f);
}
