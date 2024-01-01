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

#include "OtDodecahedronGeometry.h"


//
//	See https://en.wikipedia.org/wiki/Dodecahedron
//


//
//	Globals
//

static float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
static float r = 1.0f / t;

static glm::vec3 bv[] = {
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, 1.0f),
	glm::vec3(-1.0f, 1.0f, -1.0f),
	glm::vec3(-1.0f, 1.0f, 1.0f),

	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),

	glm::vec3(0.0f, -r, -t),
	glm::vec3(0.0f, -r, t),
	glm::vec3(0.0f, r, -t),
	glm::vec3(0.0f, r, t),

	glm::vec3(-r, -t, 0.0f),
	glm::vec3(-r, t, 0.0f),
	glm::vec3(r, -t, 0.0f),
	glm::vec3(r, t, 0.0f),

	glm::vec3(-t, 0.0f, -r),
	glm::vec3(t, 0.0f, -r),
	glm::vec3(-t, 0.0f, r),
	glm::vec3(t, 0.0f, r)
};

static uint32_t bi[] = {
	3, 11, 7,
	3, 7, 15,
	3, 15, 13,
	7, 19, 17,
	7, 17, 6,
	7, 6, 15,
	17, 4, 8,
	17, 8, 10,
	17, 10, 6,
	8, 0, 16,
	8, 16, 2,
	8, 2, 10,
	0, 12, 1,
	0, 1, 18,
	0, 18, 16,
	6, 10, 2,
	6, 2, 13,
	6, 13, 15,
	2, 16, 18,
	2, 18, 3,
	2, 3, 13,
	18, 1, 9,
	18, 9, 11,
	18, 11, 3,
	4, 14, 12,
	4, 12, 0,
	4, 0, 8,
	11, 9, 5,
	11, 5, 19,
	11, 19, 7,
	19, 5, 14,
	19, 14, 4,
	19, 4, 17,
	1, 12, 14,
	1, 14, 5,
	1, 5, 9
};


//
//	OtDodecahedronGeometryClass::OtDodecahedronGeometryClass
//

OtDodecahedronGeometryClass::OtDodecahedronGeometryClass() {
	baseVertices = bv;
	baseIndices = bi;
	baseVertexCount = sizeof(bv) / sizeof(bv[0]);
	baseIndexCount = sizeof(bi) / sizeof(bi[0]);
}


//
//	OtDodecahedronGeometryClass::renderGUI
//

bool OtDodecahedronGeometryClass::renderGUI() {
	return OtPolyhedronGeometryClass::renderGUI();
}


//
//	OtDodecahedronGeometryClass::serialize
//

nlohmann::json OtDodecahedronGeometryClass::serialize(std::filesystem::path* basedir) {
	auto data = OtPolyhedronGeometryClass::serialize(basedir);
	data["type"] = name;
	return data;
}


//
//	OtDodecahedronGeometryClass::getMeta
//

OtType OtDodecahedronGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtDodecahedronGeometryClass>("DodecahedronGeometry", OtPolyhedronGeometryClass::getMeta());
	}

	return type;
}
