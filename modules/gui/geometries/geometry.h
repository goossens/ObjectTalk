//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"
#include "bgfx/bgfx.h"

#include "gui.h"
#include "vertex.h"


//
//	OtGeometryClass
//

class OtGeometryClass;
typedef std::shared_ptr<OtGeometryClass> OtGeometry;

class OtGeometryClass : public OtGuiClass {
public:
	// destructor
	~OtGeometryClass();

	// add vertices/triangles/lines to the geometry
	void addVertex(const OtVertex& vertex) {
		vertices.push_back(vertex);
	}

	void addTriangle(uint16_t p1, uint16_t p2, uint16_t p3) {
		triangles.push_back(p1);
		triangles.push_back(p2);
		triangles.push_back(p3);
	}

	void addLine(uint16_t p1, uint16_t p2) {
		lines.push_back(p1);
		lines.push_back(p2);
	}

	// reset geometry
	void clear();

	// does geometry have holes (or is one-sided)?
	bool wantsCulling() { return culling; }

	// access BGFX buffers
	bgfx::VertexBufferHandle getVertexBuffer();
	bgfx::IndexBufferHandle getTriangleIndexBuffer();
	bgfx::IndexBufferHandle getLineIndexBuffer();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtGeometry create();

protected:
	// handle buffers
	void clearBuffers();
	void updateBuffers();
	virtual void fillBuffers() {}
	bool refreshBuffers = true;

	// geometry data
	bool culling = false;

	std::vector<OtVertex> vertices;
	std::vector<uint16_t> triangles;
	std::vector<uint16_t> lines;

	// BGFX buffers
	bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle triagleIndexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle lineIndexBuffer = BGFX_INVALID_HANDLE;
};
