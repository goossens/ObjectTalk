//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <limits>

#include "OtLog.h"

#include "OtModelMesh.h"
#include "OtModelUtils.h"


//
//	OtModelMesh::load
//

void OtModelMesh::load(const aiMesh* mesh, OtModelNodes& nodes) {
	// sanity check
	if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
		OtLogError("Mesh has incompatible primitive type");
	}

	// save the name
	name = mesh->mName.C_Str();

	// clear and reserve vertex space
	vertices.clear();
	vertices.reserve(mesh->mNumVertices);

	// see what components we have
	bool hasUV = mesh->mNumUVComponents[0] == 2 && mesh->mTextureCoords[0] != nullptr;
	bool hasNormals = mesh->mNormals != nullptr;
	bool hasTangents = mesh->mTangents != nullptr;
	bool hasBitangents = mesh->mBitangents != nullptr;

	// process all vertices
	for (auto i = 0u; i < mesh->mNumVertices; i++) {
		// get position
		glm::vec3 pos = toVec3(mesh->mVertices[i]);

		// add a new vertex
		vertices.emplace_back(
			pos,
			hasNormals ? toVec3(mesh->mNormals[i]) : glm::vec3(),
			hasUV ? toVec2(mesh->mTextureCoords[0][i]) : glm::vec2(),
			hasTangents ? toVec3(mesh->mTangents[i]) : glm::vec3(),
			hasBitangents ? toVec3(mesh->mBitangents[i]) : glm::vec3());
	}

	// clear and reserve index space
	indices.clear();
	indices.reserve(mesh->mNumFaces * 3);

	// process all triangles
    for(auto i = 0u; i < mesh->mNumFaces; i++) {
		indices.emplace_back(mesh->mFaces[i].mIndices[0]);
		indices.emplace_back(mesh->mFaces[i].mIndices[1]);
		indices.emplace_back(mesh->mFaces[i].mIndices[2]);
	}

	// remember material reference
	material = mesh->mMaterialIndex;

	// set bounding box
	aabb.clear();
	aabb.addPoint(toVec3(mesh->mAABB.mMin));
	aabb.addPoint(toVec3(mesh->mAABB.mMax));

	// process bones data (if required)
	bones.clear();
	boneIndex.clear();
	vertexBones.clear();

	if (mesh->HasBones()) {
		bones.reserve(mesh->mNumBones);
		vertexBones.resize(mesh->mNumVertices);
		std::vector<size_t> boneCounts;
		boneCounts.resize(mesh->mNumVertices, 0);

		// process all bones
		for(auto i = 0u; i < mesh->mNumBones; i++) {
			aiBone* bone = mesh->mBones[i];
			std::string name = bone->mName.C_Str();

			if (!nodes.hasNode(name)) {
				OtLogError("Bone [{}] is not part of model's hierarchy", name);
			}

			bones.emplace_back(name, toMat4(bone->mOffsetMatrix));
			boneIndex[name] = i;

			// loop through each vertex that is affected by that bone
			for (auto j = 0u; j < bone->mNumWeights; j++) {
				auto id = bone->mWeights[j].mVertexId;
				auto weight = bone->mWeights[j].mWeight;
				boneCounts[id]++;

				switch (boneCounts[id]) {
					case 1:
						vertexBones[id].indices.x = i;
						vertexBones[id].weights.x = weight;
						break;

					case 2:
						vertexBones[id].indices.y = i;
						vertexBones[id].weights.y = weight;
						break;

					case 3:
						vertexBones[id].indices.z = i;
						vertexBones[id].weights.z = weight;
						break;

					case 4:
						vertexBones[id].indices.w = i;
						vertexBones[id].weights.w = weight;
						break;

					default:
						OtLogError("Mesh has too many bones connected to vertex");
						break;
				}
			}
		}

		// normalize weights to make all weights sum 1
		for (auto i = 0u; i < mesh->mNumVertices; i++) {
			glm::vec4& boneWeights = vertexBones[i].weights;
			float totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;

			if (totalWeight > 0.0f) {
				vertexBones[i].weights = boneWeights / totalWeight;
			}
		}
	}

	// update buffers during next submission
	update = true;
}


//
//	OtModelMesh::submitTriangles
//

void OtModelMesh::submitTriangles() {
	if (update) {
		vertexBuffer.set(vertices.data(), vertices.size(), OtVertex::getLayout());
		indexBuffer.set(indices.data(), indices.size());
		update = false;
	}

	vertexBuffer.submit();
	indexBuffer.submit();
}
