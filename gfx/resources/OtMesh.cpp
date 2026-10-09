//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <format>

#include "assimp/Exporter.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "meshoptimizer.h"

#include "OtLog.h"
#include "OtPath.h"
#include "OtText.h"

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

	// configure importer
	Assimp::Importer importer;

	importer.SetPropertyInteger(
		AI_CONFIG_PP_RVC_FLAGS,
		aiComponent_COLORS |
		aiComponent_BONEWEIGHTS |
		aiComponent_ANIMATIONS |
		aiComponent_TEXTURES |
		aiComponent_LIGHTS |
		aiComponent_CAMERAS |
		aiComponent_MATERIALS);

	importer.SetPropertyInteger(
		AI_CONFIG_PP_SBP_REMOVE,
		aiPrimitiveType_POINT |
		aiPrimitiveType_LINE);

	// determine the import flags
	unsigned int flags =
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_RemoveComponent |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		aiProcess_FlipUVs;

	// read the file
	const aiScene* scene = importer.ReadFile(path, flags);

	// ensure mesh was loaded correctly
	if (scene == nullptr) {
		OtLogError("Unable to mesh model at [{}], error: {}", path, importer.GetErrorString());
	}

	// ensure scene is complete
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		OtLogError("Incomplete mesh at [{}]", path);
	}

	// import all the meshes
	int offset = 0;
	bool needsTangents = false;

	for (auto im = 0u; im < scene->mNumMeshes; im++) {
		auto aimesh = scene->mMeshes[im];

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
				aimesh->mBitangents ? ToVec3(aimesh->mBitangents[iv]) : glm::vec3()));
		}

		// process all indices
		for (auto it = 0u; it < aimesh->mNumFaces; it++) {
			auto i0 = aimesh->mFaces[it].mIndices[0] + offset;
			auto i1 = aimesh->mFaces[it].mIndices[1] + offset;
			auto i2 = aimesh->mFaces[it].mIndices[2] + offset;
			addTriangle(i0, i1, i2);
		}

		// update the index offset
		offset += aimesh->mNumVertices;
	}

	if (needsTangents) {
		generateTangents();
	}

	generateAABB();
}


//
//	OtMesh::save
//

void OtMesh::save(const std::string& path, bool tangent, bool bitangent, bool uv) {
	// create an assimp scene
	aiScene* scene =  new aiScene();

	scene->mNumMaterials = 1;
	scene->mMaterials = new aiMaterial*[1];
	scene->mMaterials[0] = new aiMaterial();

	aiMaterial* material = scene->mMaterials[0];
	float roughness = 0.2f;
	float metalness = 1.0f;
	aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
	material->AddProperty(&roughness, 1, AI_MATKEY_ROUGHNESS_FACTOR);
	material->AddProperty(&metalness, 1, AI_MATKEY_METALLIC_FACTOR);
	material->AddProperty(&baseColor, 1, AI_MATKEY_COLOR_DIFFUSE);

	scene->mNumMeshes = 1;
	scene->mMeshes = new aiMesh*[1];
	scene->mMeshes[0] = new aiMesh();
	scene->mMeshes[0]->mMaterialIndex = 0;
	scene->mMeshes[0]->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;

	scene->mRootNode = new aiNode();
	scene->mRootNode->mNumMeshes = 1;
	scene->mRootNode->mMeshes = new unsigned int[1];
	scene->mRootNode->mMeshes[0] = 0;

	// create an assimp mesh
	aiMesh* aMash = scene->mMeshes[0];
	aMash->mNumVertices = static_cast<unsigned int>(vertices.size());
	aMash->mVertices = new aiVector3D[aMash->mNumVertices];
	aMash->mNormals = new aiVector3D[aMash->mNumVertices];

	if (tangent) { aMash->mTangents = new aiVector3D[aMash->mNumVertices]; }
	if (bitangent) { aMash->mBitangents = new aiVector3D[aMash->mNumVertices]; }

	if (uv) {
		aMash->mTextureCoords[0] = new aiVector3D[aMash->mNumVertices];
		aMash->mNumUVComponents[0] = 2;
	}

	size_t i = 0;

	for (auto& vertex : vertices) {
		aMash->mVertices[i] = aiVector3D(vertex.position.x, vertex.position.y, vertex.position.z);
		aMash->mNormals[i] = aiVector3D(vertex.normal.x, vertex.normal.y, vertex.normal.z);
		if (tangent) { aMash->mTangents[i] = aiVector3D(vertex.tangent.x, vertex.tangent.y, vertex.tangent.z); }
		if (bitangent) { aMash->mBitangents[i] = aiVector3D(vertex.bitangent.x, vertex.bitangent.y, vertex.bitangent.z); }
		if (uv) { aMash->mTextureCoords[0][i] = aiVector3D(vertex.uv.x, vertex.uv.y, 0.0f); }
		i++;
	}

	aMash->mNumFaces = static_cast<unsigned int>(indices.size() / 3);
	aMash->mFaces = new aiFace[aMash->mNumFaces];

	for (size_t i = 0; i < static_cast<size_t>(aMash->mNumFaces); i++) {
		aiFace& face = aMash->mFaces[i];
		face.mNumIndices = 3;
		face.mIndices = new unsigned int[face.mNumIndices];
		face.mIndices[0] = indices[3 * i + 0];
		face.mIndices[1] = indices[3 * i + 1];
		face.mIndices[2] = indices[3 * i + 2];
	}

	// export the mesh
	Assimp::Exporter exporter;
	auto ext = OtPath::getExtension(path);
	auto format = OtText::mid(ext, 1, ext.size() - 1);

	if (format == "gltf" || format == "glb") {
		format += "2";
	}

	auto result = exporter.Export(scene, format, path);
	delete scene;

	// sanity check
	if (result != AI_SUCCESS) {
		OtLogError("Can't save mesh to [{}], error: {}", path, exporter.GetErrorString());
	}
}


//
//	OtMesh::add
//

void OtMesh::add(const OtMesh& mesh) {
	// determine offset
	auto offset = static_cast<uint32_t>(vertices.size());

	// make room for new vertices and indices
	vertices.reserve(vertices.size() + mesh.vertices.size());
	indices.reserve(indices.size() + mesh.indices.size());

	// copy vertices
	for (auto& vertex : mesh.vertices) {
		vertices.emplace_back(vertex);
	}

	// copy indices and adjust by offset
	for (auto index : mesh.indices) {
		indices.emplace_back(index + offset);
	}

	// regenerate bounding box
	generateAABB();

	// set flag to refresh GPU buffers
	refreshBuffers = true;
}


//
//	OtMesh::optimize
//

void OtMesh::optimize() {
	// create index buffer for optimizer
	std::vector<unsigned int> tmpBuffer;
	tmpBuffer.reserve(indices.size());

	for (auto index : indices) {
		tmpBuffer.emplace_back(static_cast<unsigned int>(index));
	}

	// allocate space for the remap table
	std::vector<unsigned int> remap(indices.size());

	// generate the remap table and buffers
	size_t uniqueVertexCount = meshopt_generateVertexRemap(
		remap.data(),
		tmpBuffer.data(),
		indices.size(),
		vertices.data(),
		vertices.size(),
		sizeof(OtVertex));

	std::vector<unsigned int> optimizedIndices(indices.size());
	std::vector<OtVertex> optimizedVertices(uniqueVertexCount);

	meshopt_remapIndexBuffer(optimizedIndices.data(), indices.data(), indices.size(), remap.data());
	meshopt_remapVertexBuffer(optimizedVertices.data(), vertices.data(), vertices.size(), sizeof(OtVertex), remap.data());

	// vertex cache optimization
	 meshopt_optimizeVertexCache(
		optimizedIndices.data(),
		optimizedIndices.data(),
		indices.size(),
		uniqueVertexCount
	);

	// overdraw optimization
	meshopt_optimizeOverdraw(
		optimizedIndices.data(),
		optimizedIndices.data(),
		indices.size(),
		&optimizedVertices[0].position.x,
		uniqueVertexCount,
		sizeof(OtVertex),
		1.05f
	);

	//	vertex fetch optimization
	meshopt_optimizeVertexFetch(
		optimizedVertices.data(),
		optimizedIndices.data(),
		indices.size(),
		optimizedVertices.data(),
		uniqueVertexCount,
		sizeof(OtVertex)
	);

	// store new vertices and indices
	vertices = optimizedVertices;
	indices.clear();
	indices.reserve(optimizedIndices.size());

	for (auto index : optimizedIndices) {
		indices.emplace_back(static_cast<uint32_t>(index));
	}

	// set flag to refresh GPU buffers
	refreshBuffers = true;
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
	// clear normals
	for (auto& vertex : vertices) {
		vertex.normal = glm::vec3(0.0f);
	}

	// generate new normals
	for (size_t c = 0; c < indices.size(); c += 3) {
		OtVertex& v0 = vertices[indices[c]];
		OtVertex& v1 = vertices[indices[c + 1]];
		OtVertex& v2 = vertices[indices[c + 2]];

		auto normal = glm::cross(v1.position - v0.position, v2.position - v0.position);
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
	// inspired by https://learnopengl.com/Advanced-Lighting/Normal-Mapping

	// clear (bi)tangents
	for (auto& vertex : vertices) {
		vertex.tangent = glm::vec3(0.0f);
		vertex.bitangent = glm::vec3(0.0f);
	}

	// generate new (bi)tangents
	for (size_t i = 0; i < indices.size(); i += 3) {
		OtVertex& v0 = vertices[indices[i]];
		OtVertex& v1 = vertices[indices[i + 1]];
		OtVertex& v2 = vertices[indices[i + 2]];

		glm::vec3 edge1 = v1.position - v0.position;
		glm::vec3 edge2 = v2.position - v0.position;

		glm::vec2 deltaUV1 = v1.uv - v0.uv;
		glm::vec2 deltaUV2 = v2.uv - v0.uv;
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent;
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;

		glm::vec3 bitangent;
		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		v0.bitangent += bitangent;
		v1.bitangent += bitangent;
		v2.bitangent += bitangent;
	}

	// normalize the (bi)tangents
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
