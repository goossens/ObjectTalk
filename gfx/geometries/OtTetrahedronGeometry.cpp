//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtFunction.h"

#include "OtTetrahedronGeometry.h"


//
//	See https://en.wikipedia.org/wiki/Tetrahedron
//


//
//	Globals
//

static glm::vec3 bv[] = {
	glm::vec3(1, 1, 1), glm::vec3(-1, -1, 1), glm::vec3(-1, 1, -1), glm::vec3(1, -1, -1)
};

static uint32_t bi[] = {
	2, 1, 0, 	0, 3, 2,	1, 3, 0,	2, 3, 1
};


//
//	OtTetrahedronGeometryClass::OtTetrahedronGeometryClass
//

OtTetrahedronGeometryClass::OtTetrahedronGeometryClass() {
	baseVertices = bv;
	baseIndices = bi;
	baseVertexCount = sizeof(bv) / sizeof(bv[0]);
	baseIndexCount = sizeof(bi) / sizeof(bi[0]);
}


//
//	OtTetrahedronGeometryClass::renderGUI
//

bool OtTetrahedronGeometryClass::renderGUI() {
	return OtPolyhedronGeometryClass::renderGUI();
}


//
//	OtTetrahedronGeometryClass::serialize
//

nlohmann::json OtTetrahedronGeometryClass::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = name;
	OtPolyhedronGeometryClass::serialize(data);
	return data;
}


//
//	OtTetrahedronGeometryClass::deserialize
//

void OtTetrahedronGeometryClass::deserialize(nlohmann::json data) {
	OtPolyhedronGeometryClass::deserialize(data);
}


//
//	OtTetrahedronGeometryClass::getMeta
//

OtType OtTetrahedronGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtTetrahedronGeometryClass>("TetrahedronGeometry", OtPolyhedronGeometryClass::getMeta());
	}

	return type;
}
