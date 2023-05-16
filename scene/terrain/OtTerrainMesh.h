//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtAABB.h"

#include "OtTerrainTile.h"


//
//	OtTerrainMesh
//

class OtTerrainMesh {
public:
	// constructors
	OtTerrainMesh() = default;
	OtTerrainMesh(int l, OtTerrainTile* t) : level(l), tile(t) {}

	// update the mesh parameters
	void update(float hscale, float vscale, float xoffset, float zoffset);

	// submit the triangles/lines
	void submitTriangles() { tile->submitTriangles(); }
	void submitLines() { tile->submitLines(); }

	// get the bounding box
	OtAABB& getAABB() { return worldAABB; }

	// get the model transform
	glm::mat4& getTransform() { return transform; }

private:
	// mesh properties
	int level;
	OtTerrainTile* tile;
	OtAABB worldAABB;
	glm::mat4 transform;
};
