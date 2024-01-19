//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	glm::vec3(1.0f, 1, 1),
	glm::vec3(-1.0f, -1.0f, 1.0f),
	glm::vec3(-1.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f)
};

static uint32_t bi[] = {
	2, 1, 0,
	0, 3, 2,
	1, 3, 0,
	2, 3, 1
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
//	OtTetrahedronGeometryClass::serialize
//

nlohmann::json OtTetrahedronGeometryClass::serialize(std::string* basedir) {
	auto data = OtPolyhedronGeometryClass::serialize(basedir);
	data["type"] = name;
	return data;
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
