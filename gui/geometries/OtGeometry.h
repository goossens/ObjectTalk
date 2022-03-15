//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtGui.h"
#include "OtVertex.h"


//
//	OtGeometryClass
//

class OtGeometryClass;
typedef std::shared_ptr<OtGeometryClass> OtGeometry;

class OtGeometryClass : public OtGuiClass {
public:
	// destructor
	~OtGeometryClass();

	// force updates
	void forceGeometryRefresh() { refreshGeometry = true; }
	void forceBufferRefresh() { refreshBuffers = true; }

	// access vertices
	OtVertex& getVertex(size_t offset) {
		if (refreshGeometry) {
			fillGeometry();
		}

		return vertices[offset];
	}

	// access bounding box
	glm::vec3 getMinBB() { return minBB; }
	glm::vec3 getMaxBB() { return maxBB; }

	// access BGFX buffers
	bgfx::VertexBufferHandle getVertexBuffer();
	bgfx::IndexBufferHandle getTriangleIndexBuffer();
	bgfx::IndexBufferHandle getLineIndexBuffer();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtGeometry create();

protected:
	// handle geometry
	void clearGeometry();
	void updateGeometry();
	virtual void fillGeometry() {}
	bool refreshGeometry = true;

	// add vertices/triangles/lines to the geometry
	void addVertex(const OtVertex& vertex) {
		vertices.push_back(vertex);
		minBB.x = std::min(minBB.x, vertex.position.x);
		minBB.y = std::min(minBB.y, vertex.position.y);
		minBB.z = std::min(minBB.z, vertex.position.z);
		maxBB.x = std::max(maxBB.x, vertex.position.x);
		maxBB.y = std::max(maxBB.y, vertex.position.y);
		maxBB.z = std::max(maxBB.z, vertex.position.z);
	}

	void addTriangle(uint32_t p1, uint32_t p2, uint32_t p3) {
		triangles.push_back(p1);
		triangles.push_back(p2);
		triangles.push_back(p3);
	}

	void addLine(uint32_t p1, uint32_t p2) {
		lines.push_back(p1);
		lines.push_back(p2);
	}

	// quick way to add complete faces
	virtual void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		auto n = glm::normalize(glm::cross(c - b, a - b));
		auto offset = vertices.size();

		addVertex(OtVertex(a, n));
		addVertex(OtVertex(b, n));
		addVertex(OtVertex(c, n));

		addTriangle(offset, offset + 1, offset + 2);

		addLine(offset, offset + 1);
		addLine(offset + 1, offset + 2);
		addLine(offset + 2, offset);
	}

	virtual void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
		auto n = glm::normalize(glm::cross(c - b, a - b));
		auto offset = vertices.size();

		addVertex(OtVertex(a, n));
		addVertex(OtVertex(b, n));
		addVertex(OtVertex(c, n));
		addVertex(OtVertex(d, n));

		addTriangle(offset, offset + 1, offset + 2);
		addTriangle(offset, offset + 2, offset + 3);

		addLine(offset, offset + 1);
		addLine(offset + 1, offset + 2);
		addLine(offset + 2, offset);

		addLine(offset, offset + 2);
		addLine(offset + 2, offset + 3);
		addLine(offset + 3, offset);
	}

	// geometry data
	std::vector<OtVertex> vertices;
	std::vector<uint32_t> triangles;
	std::vector<uint32_t> lines;

	// Axis-aligned Bounding Box (BB)
	glm::vec3 minBB, maxBB;

	// handle buffers
	void clearBuffers();
	void updateBuffers();
	bool refreshBuffers = true;

	// BGFX buffers
	bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle triagleIndexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle lineIndexBuffer = BGFX_INVALID_HANDLE;
};
