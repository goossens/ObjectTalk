//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtGeometry.h"


//
//	OtGeometryClass::~OtGeometryClass
//

OtGeometryClass::~OtGeometryClass() {
	clearBuffers();
}


//
//	OtGeometryClass::computeTangents
//

OtObject OtGeometryClass::computeTangents() {
	if (!tangent) {
		tangent = true;
		refreshBuffers = true;
	}

	return shared();
}


//
//	OtGeometryClass::clear
//

void OtGeometryClass::clearGeometry() {
	aabb->clear();
	vertices.clear();
	triangles.clear();
	lines.clear();

	refreshGeometry = true;
	refreshBuffers = true;
}


//
//	OtGeometryClass::updateGeometry
//

void OtGeometryClass::updateGeometry() {
	clearGeometry();
	fillGeometry();
	refreshGeometry = false;
}


//
//	OtGeometryClass::validateGeometry
//

void OtGeometryClass::validateGeometry() {
	if (refreshGeometry) {
		updateGeometry();
	}
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

	// do we need to generate the tangents?
	if (tangent) {
		// clear tangents
		for (auto& vertex : vertices) {
			vertex.tangent = glm::vec3(0.0);
//			vertex.bitangent = glm::vec3(0.0);
		}

		// generate new tangents
		for (auto i = 0; i < triangles.size(); i += 3) {
			OtVertex& v0 = vertices[triangles[i]];
			OtVertex& v1 = vertices[triangles[i + 1]];
			OtVertex& v2 = vertices[triangles[i + 2]];

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

//			glm::vec3 bitangent;
//			bitangent.x = f * (-deltaU2 * edge1.x + deltaU1 * edge2.x);
//			bitangent.y = f * (-deltaU2 * edge1.y + deltaU1 * edge2.y);
//			bitangent.z = f * (-deltaU2 * edge1.z + deltaU1 * edge2.z);

//			v0.bitangent += bitangent;
//			v1.bitangent += bitangent;
//			v2.bitangent += bitangent;
		}

		// normalize the tangents
		for (auto& vertex : vertices) {
			vertex.tangent = glm::normalize(vertex.tangent);
//			vertex.bitangent = glm::normalize(vertex.bitangent);
		}
	}

	// update the buffers
	bgfx::VertexLayout layout = OtVertex::getVertexLayout();
	vertexBuffer = bgfx::createVertexBuffer(bgfx::copy(vertices.data(), sizeof(OtVertex) * (uint32_t) vertices.size()), layout);
	triagleIndexBuffer = bgfx::createIndexBuffer(bgfx::copy(triangles.data(), sizeof(uint32_t) * (uint32_t) triangles.size()), BGFX_BUFFER_INDEX32);
	lineIndexBuffer = bgfx::createIndexBuffer(bgfx::copy(lines.data(), sizeof(uint32_t) * (uint32_t) lines.size()), BGFX_BUFFER_INDEX32);
	refreshBuffers = false;
}


//
//	OtGeometryClass::validateBuffers
//

void OtGeometryClass::validateBuffers() {
	if (refreshBuffers) {
		updateBuffers();
	}
}


//
//	OtGeometryClass::getVertexBuffer
//

bgfx::VertexBufferHandle OtGeometryClass::getVertexBuffer() {
	validateGeometry();
	validateBuffers();
	return vertexBuffer;
}


//
//	OtGeometryClass::getTriangleIndexBuffer
//

bgfx::IndexBufferHandle OtGeometryClass::getTriangleIndexBuffer() {
	validateGeometry();
	validateBuffers();
	return triagleIndexBuffer;
}


//
//	OtGeometryClass::getLineIndexBuffer
//

bgfx::IndexBufferHandle OtGeometryClass::getLineIndexBuffer() {
	validateGeometry();
	validateBuffers();
	return lineIndexBuffer;
}


//
//	OtGeometryClass::getMeta
//

OtType OtGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtGeometryClass>("Geometry", OtGuiClass::getMeta());

		type->set("computeTangents", OtFunctionClass::create(&OtGeometryClass::computeTangents));
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
