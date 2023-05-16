//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAABB.h"

#include "OtIndexBuffer.h"
#include "OtVertexBuffer.h"


//
//	OtTerrainTile
//

class OtTerrainTile {
public:
	// constructors
	OtTerrainTile() = default;
	OtTerrainTile(int startx, int startz, int endx, int endz);

	// get bounding box
	OtAABB& getAABB() { return aabb; }

	// submit the triangles/lines
	void submitTriangles();
	void submitLines();

private:
	// GPU properties
	OtVertexBuffer vertexBuffer;
	OtIndexBuffer triangleBuffer;
	OtIndexBuffer lineBuffer;
	OtAABB aabb;
};