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

#include "OtUi.h"
#include "OtVertex.h"

#include "OtTerrain.h"


//
//	Geo clipmap structure
//
//	C = center tile
//	R = ring tile
//	R1..N = subsequent rings with lower levels of detail (LOD)
//
//	    |               |               |
//	    |               |               |
//	----+-------+-------+-------+-------+----
//	    |       |       |       |       |
//	    | R1..N | R1..N | R1..N | R1..N |
//	    |       |       |       |       |
//	    +-------+---+---+---+---+-------+
//	    |       | R | R | R | R |       |
//	    | R1..N +---+---+---+---+ R1..N +
//	    |       | R | C | C | R |       |
//	----+-------+---+---+---+---+-------+----
//	    |       | R | C | C | R |       |
//	    | R1..N +---+---+---+---+ R1..N +
//	    |       | R | R | R | R |       |
//	    +-------+---+---+---+---+-------+
//	    |       |       |       |       |
//	    | R1..N | R1..N | R1..N | R1..N |
//	    |       |       |       |       |
//	----+-------+-------+-------+-------+----
//	    |               |               |
//	    |               |               |


//
//	OtTerrain::renderUI
//

bool OtTerrain::renderUI() {
	bool changed = false;
	changed |= OtUi::selectorPowerOfTwo("Tile Size", &tileSize, 4, 64);
	changed |= OtUi::dragInt("Levels of Detail", &lods, 1, 10);

	if (changed) {
		clear();
	}

	changed |= OtUi::dragFloat("Horizontal Scale", &hScale, 0.01f, 10.0f);
	changed |= OtUi::dragFloat("Vertical Scale", &vScale, 1.0f, 1000.0f);
	changed |= OtUi::dragFloat("Vertical Offset", &vOffset, -1000.0f, 1000.0f);
	changed |= heights.renderUI();
	changed |= material.renderUI();
	changed |= OtUi::toggleButton("Wireframe", &wireframe);

#ifdef OT_DEBUG
	std::string buffer = std::to_string(meshes.size());
	OtUi::readonlyText("Visible Meshes", &buffer);
#endif

	return changed;
}


//
//	OtTerrain::serialize
//

nlohmann::json OtTerrain::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["tileSize"] = tileSize;
	data["lods"] = lods;
	data["hScale"] = hScale;
	data["vScale"] = vScale;
	data["vOffset"] = vOffset;
	data["heightmap"] = heights.serialize(basedir);
	data["material"] = material.serialize(basedir);
	data["wireframe"] = wireframe;
	return data;
}


//
//	OtTerrain::deserialize
//

void OtTerrain::deserialize(nlohmann::json data, std::string* basedir) {
	tileSize = data.value("tileSize", 32);
	lods = data.value("lods", 4);
	hScale = data.value("hScale", 1.0f);
	vScale = data.value("vScale", 1.0f);
	vOffset = data.value("vOffset", 0.5f);

	if (data.contains("heights")) {
		heights.deserialize(data["heights"], basedir);
	}

	if (data.contains("material")) {
		material.deserialize(data["material"], basedir);
	}

	wireframe = data.value("wireframe", false);
	clear();
}


//
//	OtTerrain::getMeshes
//

std::vector<OtTerrainMesh>& OtTerrain::getMeshes(OtFrustum& frustum, const glm::vec3& camera) {
	// update heights if required
	if (heights.dirty) {
		heights.update(tileableFbm, normalMapper);
	}

	// initialize if required
	if (!vertices.isValid()) {
		initialize();
	}

	// clear list of visible meshes
	meshes.clear();

	// determine center of terrain's geoclipmap
	float factor = 16.0f * hScale;
	centerX = std::floor(camera.x / factor) * factor;
	centerZ = std::floor(camera.z / factor) * factor;

	// lambda function to process a single tile
	auto processTile = [&](OtTerrainTile& tile, OtFrustum& frustum, int lod) {
		OtTerrainMesh mesh(tile, float(tileSize), centerX, vOffset, centerZ, hScale * (1 << lod), vScale);

		if (frustum.isVisibleAABB(mesh.aabb)) {
			meshes.push_back(mesh);
		}
	};

	// process the four center tiles
	for (auto& tile: centerTiles) {
		processTile(tile, frustum, 0);
	}

	// process the rings for LODs 0..N
	for (auto i = 0; i < lods; i++) {
		for (auto& tile: ringTiles) {
			processTile(tile, frustum, i);
		}
	}

	// return the list of visible meshes
	return meshes;
}


//
//	OtTerrain::createVertices
//

void OtTerrain::createVertices()
{
	// create temporary buffer
	std::vector<glm::vec3> buffer;
	int tileVertices = tileSize + 1;
	buffer.reserve(tileVertices * tileVertices);

	// generate vertices
	for (auto z = 0; z < tileVertices; z++) {
		for (auto x = 0; x < tileVertices; x++) {
			buffer.emplace_back(float(x), 0.0f, float(z));
		}
	}

	// store vertices in GPU buffer
	vertices.set(buffer.data(), buffer.size(), OtVertexPos::getLayout());
}


//
//	OtTerrain::createIndices
//

#define TRIANGLE(p1, p2, p3)														\
	triangles.push_back(p1); triangles.push_back(p2); triangles.push_back(p3);		\
	lines.push_back(p1); lines.push_back(p2);										\
	lines.push_back(p2); lines.push_back(p3);										\
	lines.push_back(p3); lines.push_back(p1)

void OtTerrain::createIndices(OtIndexBuffer& triangleBuffer, OtIndexBuffer& lineBuffer, int degenerate) {
	// create indices
	std::vector<uint32_t> triangles;
	std::vector<uint32_t> lines;
	int tileVertices = tileSize + 1;

	// process all triangle quads
	for (auto z = 0; z < tileSize; z += 2) {
		for (auto x = 0; x < tileSize; x += 2) {
			// A---B---C
			// | \ | / |
			// D---E---F
			// | / | \ |
			// G---H---I
			auto A = z * tileVertices + x;
			auto B = A + 1;
			auto C = B + 1;
			auto D = A + tileVertices;
			auto E = D + 1;
			auto F = E + 1;
			auto G = D + tileVertices;
			auto H = G + 1;
			auto I = H + 1;

			// edge case transitions to lower resolution of next "ring"
			if (z == 0 && (degenerate & topDegenerate)) {
				// A---B---C
				// | \   / |
				// D---E---F
				// | / | \ |
				// G---H---I
				TRIANGLE(A, E, C);

			} else {
				TRIANGLE(A, E, B);
				TRIANGLE(B, E, C);
			}

			if (x == 0 && (degenerate & leftDegenerate)) {
				// A---B---C
				// | \ | / |
				// D   E---F
				// | / | \ |
				// G---H---I
				TRIANGLE(A, G, E);

			} else {
				TRIANGLE(A, D, E);
				TRIANGLE(D, G, E);
			}

			if (z == tileSize - 2 && (degenerate & bottomDegenerate)) {
				// A---B---C
				// | \ | / |
				// D---E---F
				// | /   \ |
				// G---H---I
				TRIANGLE(G, I, E);

			} else {
				TRIANGLE(G, H, E);
				TRIANGLE(H, I, E);
			}

			if (x == tileSize - 2 && (degenerate & rightDegenerate)) {
				// A---B---C
				// | \ | / |
				// D---E   F
				// | / | \ |
				// G---H---I
				TRIANGLE(I, C, E);

			} else {
				TRIANGLE(I, F, E);
				TRIANGLE(F, C, E);
			}
		}
	}

	// create buffers
	triangleBuffer.set(triangles.data(), triangles.size());
	lineBuffer.set(lines.data(), lines.size());
}


//
//	OtTerrain::createTiles
//

void OtTerrain::createTiles() {
	// create the 4 center tiles for LOD 0
	centerTiles.emplace_back(vertices, fullTriangles, fullLines, 0.0f, 0.0f, 0.0f);
	centerTiles.emplace_back(vertices, fullTriangles, fullLines, 0.0f, 0.0f, 90.0f);
	centerTiles.emplace_back(vertices, fullTriangles, fullLines, 0.0f, 0.0f, 180.0f);
	centerTiles.emplace_back(vertices, fullTriangles, fullLines, 0.0f, 0.0f, 270.0f);

	// create the 12 ring tiles for LODs 1..N
	float size = float(tileSize);

	// top
	ringTiles.emplace_back(vertices, cornerTriangles, cornerLines, size, size, 180.0f);
	ringTiles.emplace_back(vertices, sideTriangles, sideLines, size, -size, 90.0f);
	ringTiles.emplace_back(vertices, sideTriangles, sideLines, size, 0.0f, 90.0f);
	ringTiles.emplace_back(vertices, cornerTriangles, cornerLines, size, size, 90.0f);

	// left
	ringTiles.emplace_back(vertices, sideTriangles, sideLines, size, -size, 180.0f);
	ringTiles.emplace_back(vertices, sideTriangles, sideLines, size, 0.0f, 180.0f);

	// right
	ringTiles.emplace_back(vertices, sideTriangles, sideLines, size, -size, 0.0f);
	ringTiles.emplace_back(vertices, sideTriangles, sideLines, size, 0.0f, 0.0f);

	// bottom
	ringTiles.emplace_back(vertices, cornerTriangles, cornerLines, size, size, 270.0f);
	ringTiles.emplace_back(vertices, sideTriangles, sideLines, size, -size, 270.0f);
	ringTiles.emplace_back(vertices, sideTriangles, sideLines, size, 0.0f, 270.0f);
	ringTiles.emplace_back(vertices, cornerTriangles, cornerLines, size, size, 0.0f);
}


//
//	OtTerrain::initialize
//

void OtTerrain::initialize() {
	// create vertices
	createVertices();

	// create indices
	createIndices(fullTriangles, fullLines, noDegenerate);
	createIndices(sideTriangles, sideLines, rightDegenerate);
	createIndices(cornerTriangles, cornerLines, rightDegenerate | bottomDegenerate);

	// create tiles
	createTiles();
}


//
//	OtTerrain::clear
//

void OtTerrain::clear() {
	vertices.clear();
	fullTriangles.clear();
	sideTriangles.clear();
	cornerTriangles.clear();
	fullLines.clear();
	sideLines.clear();
	cornerLines.clear();
	centerTiles.clear();
	ringTiles.clear();
	meshes.clear();
}
