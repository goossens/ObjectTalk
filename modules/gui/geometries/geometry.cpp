//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "geometry.h"


//
//	OtGeometryClass::clear
//

void OtGeometryClass::clear() {
	vertices.clear();
	triangles.clear();
}


//
//	OtGeometryClass::getVertexBuffer
//

bgfx::VertexBufferHandle OtGeometryClass::getVertexBuffer() {
	bgfx::VertexLayout layout = OtVertex::getVertexLayout();
	return bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), sizeof(OtVertex) * vertices.size()), layout);
}


//
//	OtGeometryClass::getTriangleIndexBuffer
//

bgfx::IndexBufferHandle OtGeometryClass::getTriangleIndexBuffer() {
	return bgfx::createIndexBuffer(bgfx::makeRef(triangles.data(), sizeof(uint16_t) * triangles.size()));
}


//
//	OtGeometryClass::getLineIndexBuffer
//

bgfx::IndexBufferHandle OtGeometryClass::getLineIndexBuffer() {
	// convert triangles to lines (if required)
	if (!lines.size()) {
		for (auto c = 0; c < triangles.size(); c+= 3) {
			lines.push_back(triangles[c]);
			lines.push_back(triangles[c + 1]);
			lines.push_back(triangles[c + 1]);
			lines.push_back(triangles[c + 2]);
			lines.push_back(triangles[c + 2]);
			lines.push_back(triangles[c]);
		}
	}

	return bgfx::createIndexBuffer(bgfx::makeRef(lines.data(), sizeof(uint16_t) * lines.size()));
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
