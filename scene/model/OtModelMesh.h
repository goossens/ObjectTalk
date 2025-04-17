//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "glm/glm.hpp"

#include "OtAABB.h"
#include "OtIndexBuffer.h"
#include "OtVertex.h"
#include "OtVertexBuffer.h"

#include "OtModelNodes.h"


//
//	OtModelMesh
//

class OtModelMesh {
public:
	// load the mesh
	void load(const aiMesh* mesh, OtModelNodes& nodes);

	// submit the triangles
	void submitTriangles();

	// get properties
	inline int getMaterialIndex() { return material; }
	inline OtAABB& getAABB() { return aabb; }
	inline size_t getBoneCount() { return bones.size(); }

	struct Bone {
		Bone(const std::string& n, const glm::mat4& o) : name(n), offsetTransform(o) {}
		std::string name;
		glm::mat4 offsetTransform;
	};

	inline Bone& getBone(size_t bone) { return bones[bone]; }

	private:
	// properties
	std::string name;
	std::vector<OtVertex> vertices;
	std::vector<uint32_t> indices;

	std::vector<Bone> bones;
	std::unordered_map<std::string, size_t> boneIndex;
	std::vector<OtVertexBones> vertexBones;

	// material reference (index in model)
	unsigned int material;

	// bounding box
	OtAABB aabb;

	// buffers for rendering
	OtVertexBuffer vertexBuffer;
	OtVertexBuffer vertexBonesBuffer;
	OtIndexBuffer indexBuffer;
	bool update = false;
};
