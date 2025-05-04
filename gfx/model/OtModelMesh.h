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
#include <limits>
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
	inline size_t getMaterialIndex() { return material; }
	inline OtAABB& getAABB() { return aabb; }
	inline size_t getBoneCount() { return bones.size(); }

	struct Bone {
		Bone(const std::string& nm, const glm::mat4& ot, size_t nd) : name(nm), offsetTransform(ot), node(nd) {}
		std::string name;
		glm::mat4 offsetTransform;
		size_t node;
	};

	inline bool hasBone(const std::string& nm) { return boneIndex.find(nm) != boneIndex.end(); }
	inline Bone& getNode(const std::string& nm) { return bones[boneIndex[nm]]; }
	inline Bone& getBone(size_t boneID) { return bones[boneID]; }

	inline bool hasRootNode() { return rootNode != std::numeric_limits<size_t>::max(); }
	inline size_t getRootNode() { return rootNode; }

private:
	friend class OtModel;

	// properties
	std::string name;
	std::vector<OtVertex> vertices;
	std::vector<uint32_t> indices;

	std::vector<Bone> bones;
	std::unordered_map<std::string, size_t> boneIndex;
	std::vector<OtVertexBones> vertexBones;
	size_t rootNode = std::numeric_limits<size_t>::max();

	// material reference (index in model)
	size_t material;

	// bounding box
	OtAABB aabb;

	// buffers for rendering
	OtVertexBuffer vertexBuffer;
	OtVertexBuffer vertexBonesBuffer;
	OtIndexBuffer indexBuffer;
	bool update = false;
};
