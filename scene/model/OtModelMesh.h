//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <vector>

#include "assimp/mesh.h"

#include "OtAABB.h"
#include "OtIndexBuffer.h"
#include "OtVertex.h"
#include "OtVertexBuffer.h"


//
//	OtModelMesh
//

class OtModelMesh {
public:
	// load the mesh
	void load(const aiMesh* mesh);

	// submit the triangles
	void submitTriangles();

private:
	// the renderer accesses our properties
	friend class OtSceneRenderer;

	// our vertices and triangle indices
	std::vector<OtVertex> vertices;
	std::vector<uint32_t> indices;

	// material reference (index in model)
	unsigned int material;

	// bounding box
	OtAABB aabb;

	// buffers for rendering
	OtVertexBuffer vertexBuffer;
	OtIndexBuffer indexBuffer;
};

