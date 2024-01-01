//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "OtAABB.h"
#include "OtGlm.h"
#include "OtTerrainTile.h"


//
//	OtTerrainMesh
//

class OtTerrainMesh {
public:
	// constructor
	OtTerrainMesh(OtTerrainTile& t, float size, float xoffset, float yoffset, float zoffset, float hscale, float vscale) : tile(t) {
		// determine wold space transform
		transform = glm::mat4(1.0f);
		transform = glm::translate(transform, glm::vec3(xoffset, yoffset, zoffset));
		transform = glm::scale(transform, glm::vec3(hscale, vscale, hscale));
		transform *= tile.transform;

		// determine world space bounding box
		aabb.addPoint(OtGlmMul(transform, glm::vec3(0.0f, 0.0f, 0.0f)));
		aabb.addPoint(OtGlmMul(transform, glm::vec3(size, 1.0f, size)));
	}

	// properties
	OtTerrainTile& tile;
	glm::mat4 transform;
	OtAABB aabb;
};
