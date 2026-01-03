//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "OtAABB.h"
#include "OtIndexBuffer.h"
#include "OtVertex.h"
#include "OtVertexBuffer.h"


//
//	OtMesh
//

class OtMesh {
public:
	// clear the mesh
	void clear();

	// see if mesh is valid
	inline bool isValid() { return vertices.size() && indices.size(); }

	// load/save a mesh
	void load(const std::string& path);
	void save(const std::string& path);

	// add vertices/indices to the mesh
	inline void addVertex(const OtVertex& vertex) {
		vertices.emplace_back(vertex);
		aabb.addPoint(vertex.position);
		refreshBuffers = true;
	}

	inline void addTriangle(uint32_t p1, uint32_t p2, uint32_t p3) {
		indices.emplace_back(p1);
		indices.emplace_back(p2);
		indices.emplace_back(p3);
		refreshBuffers = true;
	}

	// quick way to add complete faces
	void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		auto n = glm::normalize(glm::cross(c - b, a - b));
		auto offset = static_cast<uint32_t>(vertices.size());

		addVertex(OtVertex(a, n));
		addVertex(OtVertex(b, n));
		addVertex(OtVertex(c, n));

		addTriangle(offset, offset + 1, offset + 2);
	}

	void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
		auto n = glm::normalize(glm::cross(c - b, a - b));
		auto offset = static_cast<uint32_t>(vertices.size());

		addVertex(OtVertex(a, n));
		addVertex(OtVertex(b, n));
		addVertex(OtVertex(c, n));
		addVertex(OtVertex(d, n));

		addTriangle(offset, offset + 1, offset + 2);
		addTriangle(offset, offset + 2, offset + 3);
	}

	// get counts
	size_t getVertexCount() { return vertices.size(); }
	size_t getTriangleCount() { return indices.size() / 3; }
	size_t getIndexCount() { return indices.size(); }

	// generate AABB, normals and tangents
	void generateAABB();
	void generateNormals();
	void generateTangents();

	// get access to vertices and indices (to allow transformations and rendering)
	inline std::vector<OtVertex>& getVertices(bool update=false) {
		refreshBuffers |= update;
		return vertices;
	}

	inline std::vector<uint32_t>& getIndices() { return indices; }

	// get the AABB
	OtAABB& getAABB() { return aabb; }

private:
	// geometry data
	std::vector<OtVertex> vertices;
	std::vector<uint32_t> indices;

	// Axis-aligned Bounding Box (AABB)
	OtAABB aabb;

	// GPU resources
	OtVertexBuffer vertexBuffer;
	OtIndexBuffer indexBuffer;
	bool refreshBuffers = true;

	// access buffers
	friend class OtRenderPass;

	inline OtVertexBuffer& getVertexBuffer() {
		if (refreshBuffers) {
			updateBuffers();
		}

		return vertexBuffer;
	}

	inline OtIndexBuffer& getIndexBuffer() {
		if (refreshBuffers) {
			updateBuffers();
		}

		return indexBuffer;
	}

	// house keeping function
	void updateBuffers();
};
