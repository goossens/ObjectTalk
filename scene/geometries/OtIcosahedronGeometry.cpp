//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtFunction.h"

#include "OtIcosahedronGeometry.h"


//
//	See https://en.wikipedia.org/wiki/Regular_icosahedron
//


//
//	Globals
//

static float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

static glm::vec3 bv[] = {
	glm::vec3(-1.0f, t, 0.0f),
	glm::vec3(1.0f, t, 0.0f),
	glm::vec3(-1.0f, -t, 0.0f),
	glm::vec3(1.0f, -t, 0.0f),
	glm::vec3(0.0f, -1.0f, t),
	glm::vec3(0.0f, 1.0f, t),
	glm::vec3(0.0f, -1.0f, -t),
	glm::vec3(0.0f, 1.0f, -t),
	glm::vec3(t, 0.0f, -1.0f),
	glm::vec3(t, 0.0f, 1.0f),
	glm::vec3(-t, 0.0f, -1.0f),
	glm::vec3(-t, 0.0f, 1.0f)
};

static uint32_t bi[] = {
	0, 11, 5,
	0, 5, 1,
	0, 1, 7,
	0, 7, 10,
	0, 10, 11,
	1, 5, 9,
	5, 11, 4,
	11, 10, 2,
	10, 7, 6,
	7, 1, 8,
	3, 9, 4,
	3, 4, 2,
	3, 2, 6,
	3, 6, 8,
	3, 8, 9,
	4, 9, 5,
	2, 4, 11,
	6, 2, 10,
	8, 6, 7,
	9, 8, 1
};


//
//	OtIcosahedronGeometryClass::OtIcosahedronGeometryClass
//

OtIcosahedronGeometryClass::OtIcosahedronGeometryClass() {
	baseVertices = bv;
	baseIndices = bi;
	baseVertexCount = sizeof(bv) / sizeof(bv[0]);
	baseIndexCount = sizeof(bi) / sizeof(bi[0]);
}


//
//	OtIcosahedronGeometryClass::serialize
//

nlohmann::json OtIcosahedronGeometryClass::serialize(std::string* basedir) {
	auto data = OtPolyhedronGeometryClass::serialize(basedir);
	data["type"] = name;
	return data;
}


//
//	OtIcosahedronGeometryClass::getMeta
//

OtType OtIcosahedronGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtIcosahedronGeometryClass>("IcosahedronGeometry", OtPolyhedronGeometryClass::getMeta());
	}

	return type;
}
