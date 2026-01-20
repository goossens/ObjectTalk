//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "nlohmann/json.hpp"

#include "OtCubePrimitive.h"
#include "OtUi.h"


//
//	OtCubePrimitive::renderUI
//

bool OtCubePrimitive::renderUI() {
	auto changed = false;
	changed |= OtUi::dragInt("X Segments", &widthSegments, 1, 20);
	changed |= OtUi::dragInt("Y Segments", &heightSegments, 1, 20);
	changed |= OtUi::dragInt("Z Segments", &depthSegments, 1, 20);
	return changed;
}


//
//	OtCubePrimitive::serialize
//

nlohmann::json OtCubePrimitive::serialize([[maybe_unused]] std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data["xsegments"] = widthSegments;
	data["ysegments"] = heightSegments;
	data["zsegments"] = depthSegments;
	return data;
}


//
//	OtCubePrimitive::deserialize
//

void OtCubePrimitive::deserialize(nlohmann::json data, [[maybe_unused]] std::string* basedir) {
	widthSegments = data.value("xsegments", 1);
	heightSegments = data.value("ysegments", 1);
	depthSegments = data.value("zsegments", 1);
}


//
//	OtCubePrimitive::createMesh
//

void OtCubePrimitive::createMesh(OtMesh* mesh) {
	// create all six planes
	buildPlane(mesh, -1, -1, 1.0f, 1.0f,  1.0f, depthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(w, v, u); });
	buildPlane(mesh,  1, -1, 1.0f, 1.0f, -1.0f, depthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(w, v, u); });
	buildPlane(mesh,  1,  1, 1.0f, 1.0f,  1.0f, widthSegments,	depthSegments, [](float u, float v, float w) { return glm::vec3(u, w, v); });
	buildPlane(mesh,  1, -1, 1.0f, 1.0f, -1.0f, widthSegments,	depthSegments, [](float u, float v, float w) { return glm::vec3(u, w, v); });
	buildPlane(mesh,  1, -1, 1.0f, 1.0f,  1.0f, widthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(u, v, w); });
	buildPlane(mesh, -1, -1, 1.0f, 1.0f, -1.0f, widthSegments, heightSegments, [](float u, float v, float w) { return glm::vec3(u, v, w); });
}


//
//	OtCubePrimitive::buildPlane
//

void OtCubePrimitive::buildPlane(OtMesh* mesh, int udir, int vdir, float w, float h, float depth, int gridX, int gridY, glm::vec3 (*cb)(float, float, float)) {
	// add vertices
	auto segmentWidth = w / gridX;
	auto segmentHeight = h / gridY;

	auto widthHalf = w / 2.0f;
	auto heightHalf = h / 2.0f;
	auto depthHalf = depth / 2.0f;

	auto gridX1 = gridX + 1;
	auto gridY1 = gridY + 1;

	auto offset = static_cast<uint32_t>(mesh->getVertexCount());

	for (auto iy = 0; iy < gridY1; iy++) {
		auto y = iy * segmentHeight - heightHalf;

		for (auto ix = 0; ix < gridX1; ix++) {
			auto x = ix * segmentWidth - widthHalf;

			mesh->addVertex(OtVertex(
				cb(x * udir, y * vdir, depthHalf),
				cb(0.0f, 0.0f, (depth > 0.0f) ? 1.0f : -1.0f),
				glm::vec2(static_cast<float>(ix) / gridX, static_cast<float>(iy) / gridY)));
		}
	}

	// add triangles
	for (auto iy = 0; iy < gridY; iy++) {
		for (auto ix = 0; ix < gridX; ix++) {
			auto a = offset + ix + gridX1 * iy;
			auto b = offset + ix + gridX1 * (iy + 1);
			auto c = offset + (ix + 1) + gridX1 * (iy + 1);
			auto d = offset + (ix + 1) + gridX1 * iy;

			mesh->addTriangle(a, b, d);
			mesh->addTriangle(b, c, d);
		}
	}
}
