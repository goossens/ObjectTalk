//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <limits>

#include "ot/function.h"

#include "geometry.h"


//
//	OtGeometryClass::~OtGeometryClass
//

OtGeometryClass::~OtGeometryClass() {
	clearBuffers();
}


//
//	OtGeometryClass::clear
//

void OtGeometryClass::clearGeometry() {
	vertices.clear();
	triangles.clear();
	lines.clear();

	refreshGeometry = true;
	refreshBuffers = true;

	float min = std::numeric_limits<float>::min();
	float max = std::numeric_limits<float>::max();

	minBB = glm::vec3(max, max, max);
	maxBB = glm::vec3(min, min, min);
}


//
//	OtGeometryClass::updateGeometry
//

void OtGeometryClass::updateGeometry() {
	clearGeometry();
	fillGeometry();
	refreshGeometry = false;
	refreshBuffers = true;
}


//
//	OtGeometryClass::clearBuffers
//

void OtGeometryClass::clearBuffers() {
	if (bgfx::isValid(vertexBuffer)) {
		bgfx::destroy(vertexBuffer);
		vertexBuffer = BGFX_INVALID_HANDLE;
	}

	if (bgfx::isValid(triagleIndexBuffer)) {
		bgfx::destroy(triagleIndexBuffer);
		triagleIndexBuffer = BGFX_INVALID_HANDLE;
	}

	if (bgfx::isValid(lineIndexBuffer)) {
		bgfx::destroy(lineIndexBuffer);
		lineIndexBuffer = BGFX_INVALID_HANDLE;
	}

	refreshBuffers = true;
}


//
//	OtGeometryClass::updateBuffers
//

void OtGeometryClass::updateBuffers() {
	clearBuffers();

	bgfx::VertexLayout layout = OtVertex::getVertexLayout();
	vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), sizeof(OtVertex) * vertices.size()), layout);
	triagleIndexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(triangles.data(), sizeof(uint32_t) * triangles.size()), BGFX_BUFFER_INDEX32);
	lineIndexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(lines.data(), sizeof(uint32_t) * lines.size()), BGFX_BUFFER_INDEX32);
	refreshBuffers = false;
}


//
//	OtGeometryClass::getVertexBuffer
//

bgfx::VertexBufferHandle OtGeometryClass::getVertexBuffer() {
	if (refreshGeometry) {
		updateGeometry();
	}

	if (refreshBuffers) {
		updateBuffers();
	}

	return vertexBuffer;
}


//
//	OtGeometryClass::getTriangleIndexBuffer
//

bgfx::IndexBufferHandle OtGeometryClass::getTriangleIndexBuffer() {
	if (refreshGeometry) {
		updateGeometry();
	}

	if (refreshBuffers) {
		updateBuffers();
	}

	return triagleIndexBuffer;
}


//
//	OtGeometryClass::getLineIndexBuffer
//

bgfx::IndexBufferHandle OtGeometryClass::getLineIndexBuffer() {
	if (refreshGeometry) {
		updateGeometry();
	}

	if (refreshBuffers) {
		updateBuffers();
	}

	return lineIndexBuffer;
}


//
//	OtGeometryClass::getMeta
//

OtType OtGeometryClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtGeometryClass>("Geometry", OtGuiClass::getMeta());
	}

	return type;
}


//
//	OtGeometryClass::create
//

OtGeometry OtGeometryClass::create() {
	OtGeometry geometry = std::make_shared<OtGeometryClass>();
	geometry->setType(getMeta());
	return geometry;
}
