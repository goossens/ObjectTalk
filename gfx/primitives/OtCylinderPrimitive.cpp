//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>
#include <cmath>

#include "nlohmann/json.hpp"

#include "OtCylinderPrimitive.h"
#include "OtUi.h"


//
//	OtCylinderPrimitive::createMesh
//

void OtCylinderPrimitive::createMesh(OtMesh* mesh) {
	// generate a torso
	generateTorso(mesh);

	// generate end caps if required
	if (!openEnded) {
		if (topRadius > 0) {
			generateCap(mesh, true);
		}

		if (bottomRadius > 0) {
			generateCap(mesh, false);
		}
	}
}


//
//	OtCylinderPrimitive::generateTorso
//

void OtCylinderPrimitive::generateTorso(OtMesh* mesh) {
	auto slope = ( bottomRadius - topRadius);

	// generate each height segment
	for (auto y = 0; y <= heightSegments; y++) {
		auto v = (float) y / heightSegments;
		auto radius = v * (bottomRadius - topRadius) + topRadius;

		// generate each radial segment
		for (auto x = 0; x <= radialSegments; x++) {
			auto u = (float) x / radialSegments;
			auto theta = glm::radians(u * thetaLength + thetaStart);

			auto sinTheta = std::sin(theta);
			auto cosTheta = std::cos(theta);

			// add a new vertice
			mesh->addVertex(OtVertex(
				glm::vec3(radius * sinTheta, -v + 0.5f, radius * cosTheta),
				glm::normalize(glm::vec3(sinTheta, slope, cosTheta)),
				glm::vec2(u, v)));
		}
	}

	// add triangles
	for (auto y = 0; y < heightSegments; y++) {
		for (auto x = 0; x < radialSegments; x++) {
			auto a = y * (radialSegments + 1) + x;
			auto d = a + 1;
			auto b = a + (radialSegments + 1);
			auto c = b + 1;

			mesh->addTriangle(a, b, d);
			mesh->addTriangle(b, c, d);
		}
	}
}


//
//	OtCylinderPrimitive::generateCap
//

void OtCylinderPrimitive::generateCap(OtMesh* mesh, bool top) {
	auto radius = top ? topRadius : bottomRadius;
	auto sign = top ? 1 : - 1;

	// add center
	auto center = (uint32_t) mesh->getVertexCount();

	mesh->addVertex(OtVertex(
		glm::vec3(0.0f, 0.5f * sign, 0.0f),
		glm::vec3(0.0f, sign, 0.0f),
		glm::vec2(0.5f, 0.5f)));

	// add outside vertices
	auto offset = (uint32_t) mesh->getVertexCount();

	for (auto x = 0; x <= radialSegments; x++) {
		auto u = (float) x / radialSegments;
		auto theta = glm::radians(u * thetaLength + thetaStart);
		auto cosTheta = std::cos(theta);
		auto sinTheta = std::sin(theta);

		// add vertex
		mesh->addVertex(OtVertex(
			glm::vec3(radius * sinTheta, 0.5f * sign, radius * cosTheta),
			glm::vec3(0, sign, 0),
			glm::vec2((cosTheta * 0.5) + 0.5, (sinTheta * 0.5 * sign) + 0.5)));
	}

	// add triangles
	for (auto c = 0; c < radialSegments; c++) {
		if (top) {
			mesh->addTriangle(offset + c, offset + c + 1, center);

		} else {
			mesh->addTriangle(offset + c + 1, offset + c, center);
		}
	}
}


//
//	OtCylinderPrimitive::renderUI
//

bool OtCylinderPrimitive::renderUI() {
	bool changed = false;
	changed |= OtUi::dragFloat("Top Radius", &topRadius, 0.0f, 1.0f);
	changed |= OtUi::dragFloat("Bottom Radius", &bottomRadius, 0.0f, 1.0f);
	changed |= OtUi::dragInt("Radial Segments", &radialSegments, 1, 64);
	changed |= OtUi::dragInt("Height Segments", &heightSegments, 1, 32);
	changed |= OtUi::toggleButton("Open Ended", &openEnded);
	changed |= OtUi::dragFloat("Theta Start", &thetaStart, 0.0f, 360.0f);
	changed |= OtUi::dragFloat("Theta Length", &thetaLength, 0.0f, 360.0f);
	return changed;
}


//
//	OtCylinderPrimitive::serialize
//

nlohmann::json OtCylinderPrimitive::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data["topRadius"] = topRadius;
	data["bottomRadius"] = bottomRadius;
	data["radialSegments"] = radialSegments;
	data["heightSegments"] = heightSegments;
	data["openEnded"] = openEnded;
	data["thetaStart"] = thetaStart;
	data["thetaLength"] = thetaLength;
	return data;
}


//
//	OtCylinderPrimitive::deserialize
//

void OtCylinderPrimitive::deserialize(nlohmann::json data, std::string* basedir) {
	topRadius = data.value("topRadius", 1.0f);
	bottomRadius = data.value("bottomRadius", 1.0f);
	radialSegments = data.value("radialSegments", 32);
	heightSegments = data.value("heightSegments", 1);
	openEnded = data.value("openEnded", false);
	thetaStart = data.value("thetaStart", 0.0f);
	thetaLength = data.value("thetaLength", 360.0f);
}
