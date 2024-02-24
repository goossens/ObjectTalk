//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
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
	inline bool isValid() { return vertices.size() && triangles.size(); }

	// generate simple primitives
	void generateCube();

	// load save a mesh
	void load(const std::string& path);
	void save(const std::string& path);

	// add vertices/triangles/lines to the geometry
	inline void addVertex(const OtVertex& vertex) {
		vertices.emplace_back(vertex);
		aabb.addPoint(vertex.position);
		refreshBuffers = true;
	}


	inline void addTriangle(uint32_t p1, uint32_t p2, uint32_t p3) {
		triangles.emplace_back(p1);
		triangles.emplace_back(p2);
		triangles.emplace_back(p3);
		refreshBuffers = true;
	}

	inline void addLine(uint32_t p1, uint32_t p2) {
		lines.emplace_back(p1);
		lines.emplace_back(p2);
	}

	inline void addIndex(uint32_t p1, uint32_t p2, uint32_t p3) {
		addTriangle(p1, p2, p3);
		addLine(p1, p2);
		addLine(p2, p3);
		addLine(p3, p1);
	}

	// quick way to add complete faces
	void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		auto n = glm::normalize(glm::cross(c - b, a - b));
		auto offset = (uint32_t) vertices.size();

		addVertex(OtVertex(a, n));
		addVertex(OtVertex(b, n));
		addVertex(OtVertex(c, n));

		addTriangle(offset, offset + 1, offset + 2);

		addLine(offset, offset + 1);
		addLine(offset + 1, offset + 2);
		addLine(offset + 2, offset);
	}

	void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
		auto n = glm::normalize(glm::cross(c - b, a - b));
		auto offset = (uint32_t) vertices.size();

		addVertex(OtVertex(a, n));
		addVertex(OtVertex(b, n));
		addVertex(OtVertex(c, n));
		addVertex(OtVertex(d, n));

		addTriangle(offset, offset + 1, offset + 2);
		addTriangle(offset, offset + 2, offset + 3);

		addLine(offset, offset + 1);
		addLine(offset + 1, offset + 2);
		addLine(offset + 2, offset + 3);
		addLine(offset + 3, offset);
	}

	// get counts
	size_t getVertexCount() { return vertices.size(); }
	size_t getIndexCount() { return triangles.size(); }

	// generate normals and tangents
	void generateNormals();
	void generateTangents();
	void generateLines();

	// post process a mesh before it is submitted to the GPU
	void postProcess(std::function<void(std::vector<OtVertex>& vertices, std::vector<uint32_t>& triangles)>);

	// submit to GPU
	void submitTriangles();
	void submitLines();

	// get the AABB
	OtAABB& getAABB() { return aabb; }

private:
	// geometry data
	std::vector<OtVertex> vertices;
	std::vector<uint32_t> triangles;
	std::vector<uint32_t> lines;

	// Axis-aligned Bounding Box (AABB)
	OtAABB aabb;

	// GPU resources
	OtVertexBuffer vertexBuffer;
	OtIndexBuffer triangleIndexBuffer;
	OtIndexBuffer lineIndexBuffer;
	bool refreshBuffers = true;
	void updateBuffers();
};
