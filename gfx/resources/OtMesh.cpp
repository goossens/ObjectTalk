//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

#include "OtException.h"
#include "OtLog.h"

#include "OtGpu.h"
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
	lineBuffer.clear();
}


//
//	OtMesh::generateCube
//

void OtMesh::generateCube() {
	// clear the mesh
	clear();

	float v[] = {
		// back face
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
		// bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
	};

	// add vertices
	for (auto i = 0; i < 36; i++) {
		addVertex(OtVertex(
			glm::vec3(v[(i * 8) + 0], v[(i * 8) + 1], v[(i * 8) + 2]),
			glm::vec3(v[(i * 8) + 3], v[(i * 8) + 4], v[(i * 8) + 5]),
			glm::vec2(v[(i * 8) + 6], v[(i * 8) + 7])));
	}

	// add indices
	for (auto i = 0; i < 12; i++) {
		addTriangle(i * 3, (i * 3) + 1, (i * 3) + 2);
	}
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
	uint32_t flags =
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		(OtGpuHasOriginBottomLeft() ? 0 : aiProcess_FlipUVs);

	// read the file
	const aiScene* scene = importer.ReadFile(path, flags);

	// ensure mesh was loaded correctly
	if (scene == nullptr) {
		OtError("Unable to mesh model [{}], error: {}", path, importer.GetErrorString());
	}

	// ensure scene is complete
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		OtError("Incomplete mesh [{}]", path);
	}

	// load all the meshes
	int offset = 0;
	bool needsTangents = false;

	for (auto i = 0; i < scene->mNumMeshes; i++) {
		auto aimesh = scene->mMeshes[i];

		// see if we have 2-dimensional UV coordinates
		bool hasUV = aimesh->mNumUVComponents[0] >= 2 && aimesh->mTextureCoords[0] != nullptr;
		needsTangents |= aimesh->mTangents == nullptr;

		// process all vertices
		for (auto i = 0; i < aimesh->mNumVertices; i++) {
			// get position
			glm::vec3 pos = ToVec3(aimesh->mVertices[i]);

			// add a new vertex
			addVertex(OtVertex(
				pos,
				ToVec3(aimesh->mNormals[i]),
				hasUV ? ToVec2(aimesh->mTextureCoords[0][i]) : glm::vec2(),
				aimesh->mTangents ? ToVec3(aimesh->mTangents[i]) : glm::vec3(),
				aimesh->mTangents ? ToVec3(aimesh->mBitangents[i]) : glm::vec3()));
		}

		// process all indices
		for(auto i = 0; i < aimesh->mNumFaces; i++) {
			auto i0 = aimesh->mFaces[i].mIndices[0] + offset;
			auto i1 = aimesh->mFaces[i].mIndices[1] + offset;
			auto i2 = aimesh->mFaces[i].mIndices[2] + offset;
			addTriangle(i0, i1, i2);
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
		stream << fmt::format("v {} {} {}\n", vertex.position.x, vertex.position.y, vertex.position.z);
	}

	// write all normals
	for (auto& vertex : vertices) {
		stream << fmt::format("vn {} {} {}\n", vertex.normal.x, vertex.normal.y, vertex.normal.z);
	}

	// write all texture coordinates
	for (auto& vertex : vertices) {
		stream << fmt::format("vt {} {}\n", vertex.uv.x, vertex.uv.y);
	}

	// write all indices
	for (auto i = indices.begin(); i < indices.end(); i += 3) {
		stream << fmt::format("f {} {} {}\n", *i + 1, *(i + 1) + 1,  *(i + 2) + 1);
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
		vertex.normal = glm::vec3(0.0);
	}

	// generate new normals
	for (auto c = 0; c < indices.size(); c += 3) {
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
	// clear tangents
	for (auto& vertex : vertices) {
		vertex.tangent = glm::vec3(0.0);
		vertex.bitangent = glm::vec3(0.0);
	}

	// generate new tangents
	for (auto i = 0; i < indices.size(); i += 3) {
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

	// normalize the tangents
	for (auto& vertex : vertices) {
		vertex.tangent = glm::normalize(vertex.tangent);
		vertex.bitangent = glm::normalize(vertex.bitangent);
	}

	refreshBuffers = true;
}


//
//	OtMesh::getVertices
//

std::vector<OtVertex>& OtMesh::getVertices(bool update) {
	if (update) {
		refreshBuffers = true;
		refreshLinesBuffer = true;
	}

	return vertices;
}


//
//	OtMesh::submitTriangles
//

void OtMesh::submitTriangles() {
	updateBuffers();
	vertexBuffer.submit();
	indexBuffer.submit();
}


//
//	OtMesh::submitLines
//

void OtMesh::submitLines() {
	updateBuffers(true);
	vertexBuffer.submit();
	lineBuffer.submit();
}


//
//	OtMesh::updateBuffers
//

void OtMesh::updateBuffers(bool updateLines) {
	// update the buffers (if required)
	if (refreshBuffers) {
		if (!isValid()) {
			OtLogFatal("You can't submit a mesh without vertices and/or indices");
		}

		vertexBuffer.set(vertices.data(), vertices.size(), OtVertex::getLayout());
		indexBuffer.set(indices.data(), indices.size());
		refreshBuffers = false;
	}

	// update lines buffer (if required)
	if (updateLines && refreshLinesBuffer) {
		std::vector<uint32_t> lines;

		for (auto i = 0; i < indices.size(); i += 3) {
			auto p0 = indices[i];
			auto p1 = indices[i + 1];
			auto p2 = indices[i + 2];

			lines.emplace_back(p0); lines.emplace_back(p1);
			lines.emplace_back(p1); lines.emplace_back(p2);
			lines.emplace_back(p2); lines.emplace_back(p0);
		}

		lineBuffer.set(lines.data(), lines.size());
		refreshLinesBuffer = false;
	}
}
