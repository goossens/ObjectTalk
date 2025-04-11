//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

	static float v[] = {
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
	auto flags =
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices;

	if (!OtGpuHasOriginBottomLeft()) {
		flags |= aiProcess_FlipUVs;
	}

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

	// load all the meshes
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
	// clear tangents
	for (auto& vertex : vertices) {
		vertex.tangent = glm::vec3(0.0f);
		vertex.bitangent = glm::vec3(0.0f);
	}

	// generate new tangents
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

	// normalize the tangents
	for (auto& vertex : vertices) {
		vertex.tangent = glm::normalize(vertex.tangent);
		vertex.bitangent = glm::normalize(vertex.bitangent);
	}

	refreshBuffers = true;
}


//
//	OtMesh::clip
//

void OtMesh::clip(OtPlane plane, OtMesh& result) {
	// static constexpr uint8_t X = UINT8_MAX;

	// static constexpr std::array<std::array<uint8_t, 6>, 8> triIndices {{
	//   {X, X, X, X, X, X},
	//   {0, 3, 5, X, X, X},
	//   {3, 1, 4, X, X, X},
	//   {0, 1, 5, 1, 4, 5},
	//   {4, 2, 5, X, X, X},
	//   {0, 3, 4, 0, 4, 2},
	//   {1, 5, 3, 1, 2, 5},
	//   {0, 1, 2, X, X, X},
	// }};

	// static constexpr std::array<uint8_t, 8> numVerts {0, 3, 3, 6, 3, 6, 6, 3};
	// static constexpr std::array<uint8_t, 8> numHedges {0, 1, 1, 1, 2, 2, 2, 0};

	// calculate vertex distances to plane
	std::vector<float> distances(vertices.size());

	for (auto& vertex : vertices) {
		distances.emplace_back(plane.distance(vertex.position));
	}

	// compute edge-plain intersections and face-vertex masks
	std::vector<glm::vec3> edges(indices.size());
	std::vector<uint8_t> masks(vertices.size());

	for (size_t i = 0; i < indices.size(); i +=3) {
		auto v1 = indices[i];
		auto v2 = indices[i + 1];
		auto v3 = indices[i + 2];

 		auto d1 = distances[v1];
		auto d2 = distances[v2];
		auto d3 = distances[v3];

		if (d1 * d2 < 0.0f) {
			edges[i] = glm::mix(vertices[v1].position, vertices[v2].position, d1 / (d1 - d2));
		}

		if (d2 * d3 < 0.0f) {
			edges[i] = glm::mix(vertices[v2].position, vertices[v3].position, d2 / (d2 - d3));
		}

		if (d3 * d1 < 0.0f) {
			edges[i] = glm::mix(vertices[v3].position, vertices[v1].position, d3 / (d3 - d1));
		}

		uint8_t mask = 0;
		if (d1 < 0.0f) {mask |= 1; }
		if (d2 < 0.0f) {mask |= 2; }
		if (d3 < 0.0f) {mask |= 4; }
		masks[i / 3] = mask;
	}

	result.clear();

// 	std::vector<VertH> edgepts(n_edges());

// 	for (auto f : faces()) {
// 	  uint8_t              fvi = 0;
// 	  uint8_t              fe  = 0;
// 	  std::array<EdgeH, 3> fedges;
// 	  std::array<VertH, 3> fverts;
// 	  for (auto it = cfh_begin(f); it != cfh_end(f) && fvi < 3; it++, fvi++) {
// 		HalfH h     = *it;
// 		VertH fv    = from_vertex_handle(h);
// 		fedges[fvi] = edge_handle(h);
// 		fverts[fvi] = fv;
// 		fe |= (1 << fvi) * uint8_t(vdata[fv.idx()].inside);
// 	  }
// 	  std::array<VertH, 6> tempV;
// 	  const uint8_t* const row = triIndices[fe].data();
// 	  std::transform(row, row + numVerts[fe], tempV.begin(), [&](const uint8_t vi) {
// 		VertH v;
// 		if (vi > 2) {
// 		  EdgeH  e  = fedges[vi - 3];
// 		  VertH& ev = edgepts[e.idx()];
// 		  if (!ev.is_valid()) {
// 			VertH a  = to_vertex_handle(halfedge_handle(e, 0));
// 			VertH b  = to_vertex_handle(halfedge_handle(e, 1));
// 			auto& ad = vdata[a.idx()];
// 			auto& bd = vdata[b.idx()];
// 			assert(ad.inside != bd.inside);
// 			float r = bd.distance / (bd.distance - ad.distance);
// 			ev =
// 			  gal::handle<VertH>(clipped.add_vertex(point(a) * r + point(b) * (1.f - r)));
// 		  }
// 		  return ev;
// 		}
// 		else {
// 		  VertH  oldv = fverts[vi];
// 		  VertH& newv = vdata[oldv.idx()].mappedV;
// 		  if (!newv.is_valid()) {
// 			newv = gal::handle<VertH>(clipped.add_vertex(point(oldv)));
// 		  }
// 		  return newv;
// 		}
// 	  });
// 	  for (size_t vi = 0; vi < numVerts[fe]; vi += 3) {
// 		clipped.add_face(tempV[vi], tempV[vi + 1], tempV[vi + 2]);
// 	  }
// 	}
// 	return clipped;
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

		for (size_t i = 0; i < indices.size(); i += 3) {
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
