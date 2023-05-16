//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "OtVertex.h"

#include "OtTerrainConstants.h"
#include "OtTerrainTile.h"


//
//	Helper to quickly add triangles to index buffers
//

#define TRIANGLE(p1, p2, p3)														\
	triangles.push_back(p1); triangles.push_back(p2); triangles.push_back(p3);		\
	lines.push_back(p1); lines.push_back(p2);										\
	lines.push_back(p2); lines.push_back(p3);										\
	lines.push_back(p3); lines.push_back(p1)


//
//	OtTerrainTile::OtTerrainTile
//

OtTerrainTile::OtTerrainTile(int startX, int startZ, int endX, int endZ) {
	// create grid vertices
	std::vector<glm::vec3> grid;
	grid.reserve((OtTerrainTileSize + 1) * (OtTerrainTileSize + 1));

	for (auto z = startZ; z <= endZ; z++) {
		for (auto x = startX; x <= endX; x++) {
			grid.emplace_back(float(x), 0.0f, float(z));
		}
	}

	// fill vertex buffer
	vertexBuffer.set(grid.data(), grid.size(), OtVertexPos::getLayout());

	// create indices
	std::vector<uint32_t> triangles;
	std::vector<uint32_t> lines;

	// process all triangle quads
	auto stride = OtTerrainTileSize + 1;
	auto minValue = -OtTerrainTileSize / 2;
	auto maxValue = OtTerrainTileSize / 2 - 2;

	for (auto z = 0; z < OtTerrainTileSize; z += 2) {
		for (auto x = 0; x < OtTerrainTileSize; x += 2) {
			// A---B---C
			// | \ | / |
			// D---E---F
			// | / | \ |
			// G---H---I
			auto A = z * stride + x;
			auto B = A + 1;
			auto C = B + 1;
			auto D = A + stride;
			auto E = D + 1;
			auto F = E + 1;
			auto G = D + stride;
			auto H = G + 1;
			auto I = H + 1;

			// edge case transitions to lower resolution of next "ring"
			if (startZ + z == minValue) {
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

			if (startX + x == minValue) {
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

			if (startZ + z == maxValue) {
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

			if (startX + x == maxValue) {
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

	// set bounding box
	aabb = OtAABB(startX, 0.0f, startZ, endX, 1.0f, endZ);
}


//
//	OtTerrainTile::submitTriangles
//

void OtTerrainTile::submitTriangles() {
	vertexBuffer.submit();
	triangleBuffer.submit();
}


//
//	OtTerrainTile::submitLines
//

void OtTerrainTile::submitLines() {
	vertexBuffer.submit();
	lineBuffer.submit();
}
