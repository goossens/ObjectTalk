//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "nlohmann/json.hpp"

#include "OtSpherePrimitive.h"
#include "OtUi.h"


//
//	OtSpherePrimitive::createMesh
//

void OtSpherePrimitive::createMesh(OtMesh* mesh) {
	// get increments
	float ringDelta = thetaLength / phiSegments;
	float segDelta = phiLength / thetaSegments;

	// address each ring
	for (auto ring = 0; ring <= phiSegments; ring++) {
		auto theta = thetaStart + ring * ringDelta;

		// address each segment
		for (auto seg = 0; seg <= thetaSegments; seg++) {
			auto phi = phiStart + seg * segDelta;
			auto r0 = radius * std::sin(glm::radians(theta));
			auto y0 = radius * std::cos(glm::radians(theta));
			auto x0 = r0 * -std::sin(glm::radians(phi));
			auto z0 = r0 * -std::cos(glm::radians(phi));

			glm::vec2 uv = glm::vec2((float) seg / (float) thetaSegments, (float) ring / (float) phiSegments);

			// add vertex
			mesh->addVertex(OtVertex(
				glm::vec3(x0, y0, z0),
				glm::normalize(glm::vec3(x0, y0, z0)),
				uv));
		}
	}

	// add triangles
	for (auto ring = 0; ring < phiSegments; ring++) {
		for (auto seg = 0; seg < thetaSegments; seg++) {
			auto a = ring * (thetaSegments + 1) + seg;
			auto b = a + (thetaSegments + 1);
			auto c = b + 1;
			auto d = a + 1;

			mesh->addTriangle(a, b, d);
			mesh->addTriangle(b, c, d);
		}
	}
}


//
//	OtSpherePrimitive::renderUI
//

bool OtSpherePrimitive::renderUI() {
	bool changed = false;
	changed |= OtUi::dragInt("Phi Segments", &phiSegments, 1, 200);
	changed |= OtUi::dragFloat("Phi Start", &phiStart, 0.0f, 360.0f);
	changed |= OtUi::dragFloat("Phi Length", &phiLength, 0.0f, 360.0f);
	changed |= OtUi::dragInt("Theta Segments", &thetaSegments, 1, 100);
	changed |= OtUi::dragFloat("Theta Start", &thetaStart, 0.0f, 180.0f);
	changed |= OtUi::dragFloat("Theta Length", &thetaLength, 0.0f, 180.0f);
	return changed;
}


//
//	OtSpherePrimitive::serialize
//

nlohmann::json OtSpherePrimitive::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data["phiSegments"] = phiSegments;
	data["phiStart"] = phiStart;
	data["phiLength"] = phiLength;
	data["thetaSegments"] = thetaSegments;
	data["thetaStart"] = thetaStart;
	data["thetaLength"] = thetaLength;
	return data;
}


//
//	OtSpherePrimitive::deserialize
//

void OtSpherePrimitive::deserialize(nlohmann::json data, std::string* basedir) {
	phiSegments = data.value("phiSegments", 32);
	phiStart = data.value("phiStart", 0.0f);
	phiLength = data.value("phiLength", 360.0f);
	thetaSegments = data.value("thetaSegments", 64);
	thetaStart = data.value("thetaStart", 0.0f);
	thetaLength = data.value("thetaLength", 180.0f);
}
