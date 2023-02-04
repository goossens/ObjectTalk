//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"
#include "nlohmann/json.hpp"
#include "tesselator.h"

#include "OtException.h"
#include "OtFunction.h"

#include "OtExtrudedGeometry.h"
#include "OtPolygon.h"


//
//	OtExtrudedGeometryClass::init
//

void OtExtrudedGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 3:
				segments = parameters[2]->operator int();

			case 2:
				depth = parameters[1]->operator float();

			case 1:
				setShape(parameters[0]);
				break;

			default:
				OtExcept("Too many parameters [%ld] for [ExtrudedGeometry] constructor (max 3)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtExtrudedGeometryClass::setWidth
//

OtObject OtExtrudedGeometryClass::setShape(OtObject object) {
	object->expectKindOf("Shape");
	shape = object->cast<OtShapeClass>();
	refreshGeometry = true;
	return shared();
}


//
//	OtExtrudedGeometryClass::setDepth
//

OtObject OtExtrudedGeometryClass::setDepth(float d) {
	depth = d;
	refreshGeometry = true;
	return shared();
}


//
//	OtExtrudedGeometryClass::setSegments
//

OtObject OtExtrudedGeometryClass::setSegments(int s) {
	segments = s;
	refreshGeometry = true;
	return shared();
}


//
//	OtExtrudedGeometryClass::fillGeometry
//

void OtExtrudedGeometryClass::fillGeometry() {
	// sanity check
	if (!shape) {
		OtExcept("No [Shape] specified for [ExtrudedGeometry]");
	}

	auto polygonCount = shape->getPolygonCount();

	if (!polygonCount) {
		OtExcept("[ExtrudedGeometry] can't extrude an empty [Shape]");
	}

	// create a tesselator
	TESStesselator *tess = tessNewTess(NULL);
    tessSetOption(tess, TESS_CONSTRAINED_DELAUNAY_TRIANGULATION, 1);

	// process all polygons
	std::vector<glm::vec2> polygon;
	bool cw;

	for (auto i = 0; i < polygonCount; i++) {
		// get the points on the polygon
		shape->getPolygon(polygon, i, segments);

		// determine winding order of first polygon
		if (i == 0) {
			cw = OtPolygonIsClockwise(polygon);
		}

		// reverse polygon order if shape uses clockwise order for external contours
		if (cw) {
			std::reverse(polygon.begin(), polygon.end());
		}

		// add polygon to tesselator
		tessAddContour(tess, 2, polygon.data(), sizeof(glm::vec2), (int) polygon.size());

		// create faces to connect front and back
		for (auto j = 0; j < polygon.size() - 1; j++) {
			addFace(
				glm::vec3(polygon[j].x, polygon[j].y, 0.0),
				glm::vec3(polygon[j].x, polygon[j].y, -depth),
				glm::vec3(polygon[j + 1].x, polygon[j + 1].y, -depth),
				glm::vec3(polygon[j + 1].x, polygon[j + 1].y, 0.0));
		}
	}

	// perform the tesselation
	if (!tessTesselate(tess, TESS_WINDING_ODD, TESS_POLYGONS, 3, 2, nullptr)) {
		OtExcept("[ExtrudedGeometry] can't tesselate geometry");
	}

	int vertexCount = tessGetVertexCount(tess);
	int indexCount = tessGetElementCount(tess);
	int offset = (uint32_t) vertices.size();

	// create the front and back-facing vertices (interwoven)
	const TESSreal* verts = tessGetVertices(tess);

	for (auto i = 0; i < vertexCount; i++) {
		auto x = *verts++;
		auto y = *verts++;

		// front-facing
		addVertex(OtVertex(
			glm::vec3(x, y, 0.0),
			glm::vec3(0.0, 0.0, 1.0),
			glm::vec2(0.0)));

		// back-facing
		addVertex(OtVertex(
			glm::vec3(x, y, -depth),
			glm::vec3(0.0, 0.0, -1.0),
			glm::vec2(0.0)));
	}

	// determine winding order of triangles
	const TESSindex* indices = tessGetElements(tess);
	glm::vec3 p1 = vertices[offset + indices[0] * 2].position;
	glm::vec3 p2 = vertices[offset + indices[1] * 2].position;
	glm::vec3 p3 = vertices[offset + indices[2] * 2].position;

	// create the front and back faces
	for (auto i = 0; i < indexCount; i++) {
		auto i1 = offset + *indices++ * 2;
		auto i2 = offset + *indices++ * 2;
		auto i3 = offset + *indices++ * 2;

		addIndex(i1, i2, i3);
		addIndex(i1 + 1, i3 + 1, i2 + 1);
	}

	// cleanup
	tessDeleteTess(tess);
}


//
//	OtExtrudedGeometryClass::renderGUI
//

bool OtExtrudedGeometryClass::renderGUI() {
	return false;
}


//
//	OtExtrudedGeometryClass::serialize
//

nlohmann::json OtExtrudedGeometryClass::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = name;
	return data;
}


//
//	OtExtrudedGeometryClass::deserialize
//

void OtExtrudedGeometryClass::deserialize(nlohmann::json data) {
}


//
//	OtExtrudedGeometryClass::getMeta
//

OtType OtExtrudedGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtExtrudedGeometryClass>("ExtrudedGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtExtrudedGeometryClass::init));
		type->set("setShape", OtFunctionClass::create(&OtExtrudedGeometryClass::setShape));
		type->set("setDepth", OtFunctionClass::create(&OtExtrudedGeometryClass::setDepth));
		type->set("setSegments", OtFunctionClass::create(&OtExtrudedGeometryClass::setSegments));
	}

	return type;
}


//
//	OtExtrudedGeometryClass::create
//

OtExtrudedGeometry OtExtrudedGeometryClass::create() {
	OtExtrudedGeometry geometry = std::make_shared<OtExtrudedGeometryClass>();
	geometry->setType(getMeta());
	return geometry;
}
