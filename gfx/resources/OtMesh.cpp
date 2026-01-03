//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "fmt/format.h"

#include "OtLog.h"

#include "OtMesh.h"


//
//	OtMesh::clear
//

void OtMesh::clear() {
	vertices.clear();
	indices.clear();

	aabb.clear();

	vertexBuffer.clear();
	indexBuffer.clear();
}


//
//	OtMesh::load
//

static inline glm::vec2 ToVec2(const aiVector3D& v) {
	return glm::vec2(v.x, v.y);
}

static inline glm::vec3 ToVec3(const aiVector3D& v) {
	return glm::vec3(v.x, v.y, v.z);
}

void OtMesh::load(const std::string& path) {
	// clear the mesh
	clear();

	// create an asset importer
	Assimp::Importer importer;

	// determine the import flags
	auto flags =
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		aiProcess_FlipUVs;

	// read the file
	const aiScene* scene = importer.ReadFile(path, static_cast<unsigned int>(flags));

	// ensure mesh was loaded correctly
	if (scene == nullptr) {
		OtLogError("Unable to mesh model [{}], error: {}", path, importer.GetErrorString());
	}

	// ensure scene is complete
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		OtLogError("Incomplete mesh [{}]", path);
	}

	// import all the meshes
	int offset = 0;
	bool needsTangents = false;

	for (auto im = 0u; im < scene->mNumMeshes; im++) {
		auto aimesh = scene->mMeshes[im];

		// ensure this is a mesh with triangles
		if (aimesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
			// see if we have 2-dimensional UV coordinates
			bool hasUV = aimesh->mNumUVComponents[0] >= 2 && aimesh->mTextureCoords[0] != nullptr;
			needsTangents |= aimesh->mTangents == nullptr;

			// process all vertices
			for (auto iv = 0u; iv < aimesh->mNumVertices; iv++) {
				// add a new vertex
				addVertex(OtVertex(
					ToVec3(aimesh->mVertices[iv]),
					ToVec3(aimesh->mNormals[iv]),
					hasUV ? ToVec2(aimesh->mTextureCoords[0][iv]) : glm::vec2(),
					aimesh->mTangents ? ToVec3(aimesh->mTangents[iv]) : glm::vec3(),
					aimesh->mTangents ? ToVec3(aimesh->mBitangents[iv]) : glm::vec3()));
			}

			// process all indices
			for (auto it = 0u; it < aimesh->mNumFaces; it++) {
				auto i0 = aimesh->mFaces[it].mIndices[0] + offset;
				auto i1 = aimesh->mFaces[it].mIndices[1] + offset;
				auto i2 = aimesh->mFaces[it].mIndices[2] + offset;
				addTriangle(i0, i1, i2);
			}
		}

		// update the index offset
		offset += aimesh->mNumVertices;
	}

	if (needsTangents) {
		generateTangents();
	}
}


//
//	OtMesh::save
//

void OtMesh::save(const std::string& path) {
	// open the file
	std::ofstream stream(path.c_str());

	// write all vertices
	for (auto& vertex : vertices) {
		stream << fmt::format("v {:.4f} {:.4f} {:.4f}\n", vertex.position.x, vertex.position.y, vertex.position.z);
	}

	// write all normals
	for (auto& vertex : vertices) {
		stream << fmt::format("vn {:.4f} {:.4f} {:.4f}\n", vertex.normal.x, vertex.normal.y, vertex.normal.z);
	}

	// write all texture coordinates
	for (auto& vertex : vertices) {
		stream << fmt::format("vt {:.4f} {:.4f}\n", vertex.uv.x, vertex.uv.y);
	}

	// write all indices
	for (auto i = indices.begin(); i < indices.end(); i += 3) {
		stream << fmt::format(
			"f {}/{}/{} {}/{}/{} {}/{}/{}\n",
			*i + 1, *i + 1, *i + 1,
			*(i + 1) + 1, *(i + 1) + 1, *(i + 1) + 1,
			*(i + 2) + 1, *(i + 2) + 1, *(i + 2) + 1);
	}

	stream.close();
}


//
//	OtMesh::generateAABB
//

void OtMesh::generateAABB() {
	aabb.clear();

	for (auto& vertex : vertices) {
		aabb.addPoint(vertex.position);
	}
}


//
//	OtMesh::generateNormals
//

void OtMesh::generateNormals() {
	// clear tangents
	for (auto& vertex : vertices) {
		vertex.normal = glm::vec3(0.0f);
	}

	// generate new normals
	for (size_t c = 0; c < indices.size(); c += 3) {
		OtVertex& v0 = vertices[indices[c]];
		OtVertex& v1 = vertices[indices[c + 1]];
		OtVertex& v2 = vertices[indices[c + 2]];

		auto normal = glm::cross(v2.position - v1.position, v0.position - v1.position);
		v0.normal += normal;
		v1.normal += normal;
		v2.normal += normal;
	}

	// normalize the normals
	for (auto& vertex : vertices) {
		vertex.normal = glm::normalize(vertex.normal);
	}

	refreshBuffers = true;
}


//
//	OtMesh::generateTangents
//

void OtMesh::generateTangents() {
	// clear tangents and bitangent
	for (auto& vertex : vertices) {
		vertex.tangent = glm::vec3(0.0f);
		vertex.bitangent = glm::vec3(0.0f);
	}

	// generate new tangents and bitangent
	for (size_t i = 0; i < indices.size(); i += 3) {
		OtVertex& v0 = vertices[indices[i]];
		OtVertex& v1 = vertices[indices[i + 1]];
		OtVertex& v2 = vertices[indices[i + 2]];

		glm::vec3 edge1 = v1.position - v0.position;
		glm::vec3 edge2 = v2.position - v0.position;

		float deltaU1 = v1.uv.x - v0.uv.x;
		float deltaV1 = v1.uv.y - v0.uv.y;
		float deltaU2 = v2.uv.x - v0.uv.x;
		float deltaV2 = v2.uv.y - v0.uv.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

		glm::vec3 tangent;
		tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
		tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
		tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;

		glm::vec3 bitangent;
		bitangent.x = f * (-deltaU2 * edge1.x + deltaU1 * edge2.x);
		bitangent.y = f * (-deltaU2 * edge1.y + deltaU1 * edge2.y);
		bitangent.z = f * (-deltaU2 * edge1.z + deltaU1 * edge2.z);

		v0.bitangent += bitangent;
		v1.bitangent += bitangent;
		v2.bitangent += bitangent;
	}

	// normalize the tangents and bitangent
	for (auto& vertex : vertices) {
		vertex.tangent = glm::normalize(vertex.tangent);
		vertex.bitangent = glm::normalize(vertex.bitangent);
	}

	refreshBuffers = true;
}


//
//	OtMesh::updateBuffers
//

void OtMesh::updateBuffers() {
	// update the buffers (if required)
	if (refreshBuffers) {
		if (!isValid()) {
			OtLogFatal("You can't submit a mesh without vertices and/or indices");
		}

		vertexBuffer.set(vertices.data(), vertices.size(), OtVertex::getDescription());
		indexBuffer.set(indices.data(), indices.size());
		refreshBuffers = false;
	}
}
