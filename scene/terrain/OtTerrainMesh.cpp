//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"

#include "OtTerrainMesh.h"


//
//	OtTerrainMesh::update
//

void OtTerrainMesh::update(float hscale, float vscale, float xoffset, float zoffset) {
	// determine level scale
	float lscale = 1 << level;

	// determine model->world transform
	transform = glm::scale(glm::mat4(1.0), glm::vec3(hscale * lscale, hscale * lscale, vscale));
	transform = glm::translate(transform, glm::vec3(xoffset, 0.0, zoffset));

	// determine world AABB
	worldAABB = tile->getAABB().transform(transform);
}

