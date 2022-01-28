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

	// does geometry have holes (or is it one-sided)?
	bool wantsCulling() { return culling; }

	// access vertices
	OtVertex& getVertex(size_t offset) {
		if (refreshGeometry) {
			fillGeometry();
		}

		return vertices[offset];
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
		minBB.x = std::min(minBB.x, vertex.position.x);
		minBB.y = std::min(minBB.y, vertex.position.y);
		minBB.z = std::min(minBB.z, vertex.position.z);
		maxBB.x = std::max(minBB.x, vertex.position.x);
		maxBB.y = std::max(minBB.y, vertex.position.y);
		maxBB.z = std::max(minBB.z, vertex.position.z);
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

	// geometry data
	bool culling = false;
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
