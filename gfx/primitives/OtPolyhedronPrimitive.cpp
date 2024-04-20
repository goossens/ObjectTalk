//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>
#include <vector>

#include "nlohmann/json.hpp"

#include "OtNumbers.h"

#include "OtPolyhedronPrimitive.h"
#include "OtUi.h"


//
//	See https://en.wikipedia.org/wiki/Polyhedron
//


//
//	correctUV
//

static float correctUV(const glm::vec3& normal, float u, float azimuth) {
	if ((azimuth < 0.0f) && (u == 1.0f)) {
		return u - 1.0f;

	} else if ((normal.x == 0.0f) && (normal.z == 0.0f)) {
		return azimuth / 2.0f / std::numbers::pi + 0.5f;

	} else {
		return u;
	}
}


//
//	OtPolyhedronPrimitive::createMesh
//

void OtPolyhedronPrimitive::createMesh(OtMesh* mesh) {
	// setup
	auto cols = detail + 1;
	std::vector<std::vector<glm::vec3>> v(cols + 1, std::vector<glm::vec3>(cols + 1));

	// process all base faces
	for (auto f = 0; f < baseIndexCount; f += 3) {
		auto a = baseVertices[baseIndices[f]];
		auto b = baseVertices[baseIndices[f + 1]];
		auto c = baseVertices[baseIndices[f + 2]];

		// construct all vertices for this subdivision
		for (auto i = 0; i <= cols; i++) {
			auto r1 = (float) i / (float) cols;
			auto aj = glm::mix(a, c, r1);
			auto bj = glm::mix(b, c, r1);
			auto rows = cols - i;

			for (auto j = 0; j <= rows; j++) {
				auto r2 = (float) j / (float) rows;
				v[i][j] = glm::normalize((j == 0 && i == cols) ? aj : glm::mix(aj, bj, r2));
			}
		}

		// construct all subfaces
		for (auto i = 0; i < cols; i++) {
			for (auto j = 0; j < 2 * (cols - i) - 1; j++) {
				int k = j / 2;

				if (j % 2 == 0) {
					addFace(mesh, v[i][k + 1], v[i + 1][k], v[i][k]);

				} else {
					addFace(mesh, v[i][k + 1], v[i + 1][k + 1], v[i + 1][k]);
				}
			}
		}
	}

	// add all triangles and lines
	for (auto i = 0; i < mesh->getVertexCount(); i += 3) {
		mesh->addTriangle(i, i + 1, i + 2);
	}

	// correct the UVs
	auto& vertices = mesh->getVertices();

	for (auto i = vertices.begin(); i < vertices.end();) {
		auto a = i++;
		auto b = i++;
		auto c = i++;

		auto centroid = (a->position + b->position + c->position) / 3.0f;
		auto azi = azimuth(centroid);

		a->uv.x = correctUV(a->normal, a->uv.x, azi);
		b->uv.x = correctUV(b->normal, b->uv.x, azi);
		c->uv.x = correctUV(c->normal, c->uv.x, azi);

		auto minU = std::min(a->uv.x, std::min(b->uv.x, c->uv.x));
		auto maxU = std::max(a->uv.x, std::max(b->uv.x, c->uv.x));

		if (maxU > 0.9 && minU < 0.1) {
			if (a->uv.x < 0.2) a->uv.x += 1.0;
			if (b->uv.x < 0.2) b->uv.x += 1.0;
			if (c->uv.x < 0.2) c->uv.x += 1.0;
		}
	}
}


//
//	OtPolyhedronPrimitive::addFace
//

void OtPolyhedronPrimitive::addFace(OtMesh* mesh, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
	if (detail == 0) {
		auto n = glm::normalize(glm::cross(c - b, a - b));
		mesh->addVertex(OtVertex(a, n, generateUV(a)));
		mesh->addVertex(OtVertex(b, n, generateUV(b)));
		mesh->addVertex(OtVertex(c, n, generateUV(c)));

	} else {
		mesh->addVertex(OtVertex(a, a, generateUV(a)));
		mesh->addVertex(OtVertex(b, b, generateUV(b)));
		mesh->addVertex(OtVertex(c, c, generateUV(c)));
	}
}


//
//	OtPolyhedronPrimitive::generateUV
//

glm::vec2 OtPolyhedronPrimitive::generateUV(const glm::vec3& normal) {
	// calculate the UV texture coordinates [0.0, 0.1]
	auto uv = glm::vec2(
		azimuth(normal) / 2.0 / std::numbers::pi + 0.5f,
		inclination(normal) / std::numbers::pi + 0.5f);

	return uv;
}


//
//	OtPolyhedronPrimitive::renderUI
//

bool OtPolyhedronPrimitive::renderUI() {
	return OtUiDragInt("Detail", &detail, 0, 20);
}


//
//	OtPolyhedronPrimitive::serialize
//

nlohmann::json OtPolyhedronPrimitive::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data["detail"] = detail;
	return data;
}

//
//	OtPolyhedronPrimitive::deserialize
//

void OtPolyhedronPrimitive::deserialize(nlohmann::json data, std::string* basedir) {
	detail = data.value("detail", 1);
}
