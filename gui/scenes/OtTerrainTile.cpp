//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "OtTerrainTile.h"


//
//	Constants
//

static const int tileSize = 242;


//
//	OtTerrainTileClass::~OtTerrainTileClass
//

OtTerrainTileClass::~OtTerrainTileClass() {
	// release resources
	if (bgfx::isValid(vertexBuffer)) {
		bgfx::destroy(vertexBuffer);
	}

	if (bgfx::isValid(indexBuffer)) {
		bgfx::destroy(indexBuffer);
	}
}


//
//	OtTerrainTileClass::generate
//

void OtTerrainTileClass::generate(OtTerrainMap terrainmap) {
	// dimensions
	auto tileOffsetX = x * tileSize - tileSize / 2;
	auto tileOffsetY = y * tileSize - tileSize / 2;

	auto lodSize = tileSize - 2;
	auto lodIncrement = lod >= 6 ? 1 : (6 - lod) * 2;
	auto lodSegments = lodSize / lodIncrement;
	auto lodVertices = lodSegments + 1;
	auto lodOffsetX = tileOffsetX + 1;
	auto lodOffsetY = tileOffsetY + 1;

	auto borderSize = tileSize;
	auto borderVertices = borderSize + 1;
	auto borderOffsetX = tileOffsetX;
	auto borderOffsetY = tileOffsetY;

	// add LOD vertices
	for (auto iy = 0; iy < lodVertices; iy++) {
		auto wy = lodOffsetY + iy * lodIncrement;

		for (auto ix = 0; ix < lodVertices; ix++) {
			auto wx = lodOffsetX + ix * lodIncrement;

			vertices.emplace_back(
				glm::vec3(wx, terrainmap->getHeight(wx, wy), wy),
				terrainmap->getNormal(wx, wy));
		}
	}

	// add LOD triangles
	for (auto iy = 0; iy < lodSegments; iy++) {
		for (auto ix = 0; ix < lodSegments; ix ++) {
			auto a = ix + lodVertices * iy;
			auto b = ix + lodVertices * (iy + 1);
			auto c = (ix + 1) + lodVertices * (iy + 1);
			auto d = (ix + 1) + lodVertices * iy;

			triangles.push_back(a);
			triangles.push_back(b);
			triangles.push_back(d);

			triangles.push_back(b);
			triangles.push_back(c);
			triangles.push_back(d);
		}
	}

	//
	// LEFT BORDER
	//

	// add left border vertices
	auto index = vertices.size();
	auto wx = borderOffsetX;
	auto wy = borderOffsetY;

	for (auto iy = 0; iy < borderVertices; iy++) {
		wy = borderOffsetY + iy;

		vertices.emplace_back(
			glm::vec3(wx, terrainmap->getHeight(wx, wy), wy),
			terrainmap->getNormal(wx, wy));
	}

	// add left border triangles
	triangles.push_back(index);
	triangles.push_back(index + 1);
	triangles.push_back(0);

	for (auto s = 0; s < lodSegments; s++) {
		auto borderIndex = index + 1 + s * lodIncrement;
		auto lodIndex = s * lodVertices;

		triangles.push_back(lodIndex);
		lodIndex += lodVertices;
		triangles.push_back(borderIndex);
		triangles.push_back(lodIndex);

		for (auto i = 0; i < lodIncrement; i++) {
			triangles.push_back(borderIndex++);
			triangles.push_back(borderIndex);
			triangles.push_back(lodIndex);
		}
	}

	triangles.push_back(index + borderVertices - 2);
	triangles.push_back(index + borderVertices - 1);
	triangles.push_back(lodSegments * lodVertices);

	//
	// RIGHT BORDER
	//

	// add right border vertices
	index = vertices.size();
	wx = borderOffsetX + borderSize;

	for (auto iy = 0; iy < borderVertices; iy++) {
		wy = borderOffsetY + iy;

		vertices.emplace_back(
			glm::vec3(wx, terrainmap->getHeight(wx, wy), wy),
			terrainmap->getNormal(wx, wy));
	}

	// add right border triangles
	triangles.push_back(index);
	triangles.push_back(lodSegments);
	triangles.push_back(index + 1);

	for (auto s = 0; s < lodSegments; s++) {
		auto borderIndex = index + 1 + s * lodIncrement;
		auto lodIndex = lodSegments + s * lodVertices;

		triangles.push_back(lodIndex);
		lodIndex += lodVertices;
		triangles.push_back(lodIndex);
		triangles.push_back(borderIndex);

		for (auto i = 0; i < lodIncrement; i++) {
			triangles.push_back(borderIndex++);
			triangles.push_back(lodIndex);
			triangles.push_back(borderIndex);
		}
	}

	triangles.push_back(index + borderSize);
	triangles.push_back(index + borderSize - 1);
	triangles.push_back(lodVertices * lodVertices - 1);

	//
	// TOP BORDER
	//

	// add top border vertices
	index = vertices.size();
	wy = borderOffsetY;

	for (auto ix = 0; ix < borderVertices; ix++) {
		wx = borderOffsetX + ix;

		vertices.emplace_back(
			glm::vec3(wx, terrainmap->getHeight(wx, wy), wy),
			terrainmap->getNormal(wx, wy));
	}

	// add top border triangles
	triangles.push_back(index);
	triangles.push_back(0);
	triangles.push_back(index + 1);

	for (auto s = 0; s < lodSegments; s++) {
		auto borderIndex = index + 1 + s * lodIncrement;
		auto lodIndex = s;

		triangles.push_back(lodIndex++);
		triangles.push_back(lodIndex);
		triangles.push_back(borderIndex);

		for (auto i = 0; i < lodIncrement; i++) {
			triangles.push_back(borderIndex++);
			triangles.push_back(lodIndex);
			triangles.push_back(borderIndex);
		}
	}

	triangles.push_back(index + borderVertices - 2);
	triangles.push_back(lodSegments);
	triangles.push_back(index + borderVertices - 1);

	//
	// BOTTOM BORDER
	//

	// add bottom border vertices
	index = vertices.size();
	wy = borderOffsetY + borderSize;

	for (auto ix = 0; ix < borderVertices; ix++) {
		wx = borderOffsetX + ix;

		vertices.emplace_back(
			glm::vec3(wx, terrainmap->getHeight(wx, wy), wy),
			terrainmap->getNormal(wx, wy));
	}

	// add bottom border triangles
	triangles.push_back(index);
	triangles.push_back(index + 1);
	triangles.push_back(lodSegments * lodVertices);

	for (auto s = 0; s < lodSegments; s++) {
		auto borderIndex = index + 1 + s * lodIncrement;
		auto lodIndex = lodSegments * lodVertices + s;

		triangles.push_back(lodIndex++);
		triangles.push_back(borderIndex);
		triangles.push_back(lodIndex);

		for (auto i = 0; i < lodIncrement; i++) {
			triangles.push_back(borderIndex++);
			triangles.push_back(borderIndex);
			triangles.push_back(lodIndex);
		}
	}

	triangles.push_back(lodVertices * lodVertices - 1);
	triangles.push_back(index + borderVertices - 2);
	triangles.push_back(index + borderVertices - 1);
}


//
//	OtTerrainTileClass::submit
//

void OtTerrainTileClass::submit() {
	// create buffers (if required)
	if (!bgfx::isValid(vertexBuffer)) {
		bgfx::VertexLayout layout = OtTerrainVertex::getVertexLayout();
		vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), sizeof(OtTerrainVertex) * vertices.size()), layout);
		indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(triangles.data(), sizeof(uint32_t) * triangles.size()), BGFX_BUFFER_INDEX32);
	}

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, vertexBuffer);
	bgfx::setIndexBuffer(indexBuffer);
}
