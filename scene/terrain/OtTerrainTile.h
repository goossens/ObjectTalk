//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtIndexBuffer.h"
#include "OtVertexBuffer.h"


//
//	OtTerrainTile
//

class OtTerrainTile {
public:
	// constructor
	inline OtTerrainTile(OtVertexBuffer& v, OtIndexBuffer& t, OtIndexBuffer& l, float xoffset, float zoffset, float rotate) : vertices(v), triangles(t), lines(l) {
		// calculate local space transform
		transform = glm::mat4(1.0f);
		transform = glm::rotate(transform, glm::radians(rotate), glm::vec3(0.0f, 1.0f, 0.0f));
		transform = glm::translate(transform, glm::vec3(xoffset, 0.0f, zoffset));
	}

	// properties
	OtVertexBuffer& vertices;
	OtIndexBuffer& triangles;
	OtIndexBuffer& lines;
	glm::mat4 transform;
};
