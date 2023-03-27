//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtException.h"

#include "OtModelMesh.h"


//
//	Helper functions
//

static inline glm::vec2 ToVec2(const aiVector3D& v) {
	return glm::vec2(v.x, v.y);
}

static inline glm::vec3 ToVec3(const aiVector3D& v) {
	return glm::vec3(v.x, v.y, v.z);
}


//
//	OtModelMesh::load
//

void OtModelMesh::load(const aiMesh* mesh) {
	// sanity check
	if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
		OtError("Internal error: mesh has incompatible primitive type");
	}

	// clear and reserve vertex space
	vertices.clear();
	vertices.reserve(mesh->mNumVertices);

	// see if we have 2-dimensional UV coordinates
	bool hasUV = mesh->mNumUVComponents[0] == 2 && mesh->mTextureCoords[0] != nullptr;

	// process all vertices
	for (auto i = 0; i < mesh->mNumVertices; i++) {
		// get position
		glm::vec3 pos = ToVec3(mesh->mVertices[i]);

		// add a new vertex
		vertices.emplace_back(
			pos,
			ToVec3(mesh->mNormals[i]),
			hasUV ? ToVec2(mesh->mTextureCoords[0][i]) : glm::vec2(),
			ToVec3(mesh->mTangents[i]),
			ToVec3(mesh->mBitangents[i]));
	}

	// clear and reserve index space
	indices.clear();
	indices.reserve(mesh->mNumFaces * 3);

	// process all triangles
    for(auto i = 0; i < mesh->mNumFaces; i++) {
		indices.emplace_back(mesh->mFaces[i].mIndices[0]);
		indices.emplace_back(mesh->mFaces[i].mIndices[1]);
		indices.emplace_back(mesh->mFaces[i].mIndices[2]);
	}

	// remember material reference
	material = mesh->mMaterialIndex;

	// set bounding box
	aabb.clear();
	aabb.addPoint(ToVec3(mesh->mAABB.mMin));
	aabb.addPoint(ToVec3(mesh->mAABB.mMax));

	// update buffers
	vertexBuffer.set(vertices.data(), vertices.size(), OtVertex::getLayout());
	indexBuffer.set(indices.data(), indices.size());
}


//
//	OtModelMesh::submitTriangles
//

void OtModelMesh::submitTriangles() {
	vertexBuffer.submit();
	indexBuffer.submit();
}
