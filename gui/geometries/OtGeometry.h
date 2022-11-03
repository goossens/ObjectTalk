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

#include "OtAABB.h"
#include "OtGui.h"
#include "OtVertex.h"


//
//	OtGeometryClass
//

class OtGeometryClass;
typedef std::shared_ptr<OtGeometryClass> OtGeometry;

class OtGeometryClass : public OtGuiClass {
public:
	// constructor
	OtGeometryClass() {
		aabb = OtAABBClass::create();
	}

	// destructor
	~OtGeometryClass();

	// force updates
	void forceGeometryRefresh() { refreshGeometry = true; }
	void forceBufferRefresh() { refreshBuffers = true; }

	// access vertices
	OtVertex& getVertex(size_t offset) {
		if (refreshGeometry) {
			updateGeometry();
		}

		return vertices[offset];
	}

	// access bounding box
	OtAABB getAABB() {
		if (refreshGeometry) {
			updateGeometry();
		}

		return aabb;
	}

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
		aabb->addPoint(vertex.position);
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

	void addIndex(uint32_t p1, uint32_t p2, uint32_t p3) {
		addTriangle(p1, p2, p3);
		addLine(p1, p2);
		addLine(p2, p3);
		addLine(p3, p1);
	}

	// quick way to add complete faces
	virtual void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
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

	virtual void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
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
		addLine(offset + 2, offset);

		addLine(offset, offset + 2);
		addLine(offset + 2, offset + 3);
		addLine(offset + 3, offset);
	}

	// geometry data
	std::vector<OtVertex> vertices;
	std::vector<uint32_t> triangles;
	std::vector<uint32_t> lines;

	// Axis-aligned Bounding Box (AABB)
	OtAABB aabb;

	// handle buffers
	void clearBuffers();
	void updateBuffers();
	bool refreshBuffers = true;

	// BGFX buffers
	bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle triagleIndexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle lineIndexBuffer = BGFX_INVALID_HANDLE;
};
