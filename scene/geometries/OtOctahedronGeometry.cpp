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

#include "OtOctahedronGeometry.h"


//
//	See https://en.wikipedia.org/wiki/Octahedron
//


//
//	Globals
//

static float t = (1.0 + std::sqrt(5.0)) / 2.0;

static glm::vec3 bv[] = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f)
};

static uint32_t bi[] = {
	0, 2, 4,
	0, 4, 3,
	0, 3, 5,
	0, 5, 2,
	1, 2, 5,
	1, 5, 3,
	1, 3, 4,
	1, 4, 2
};


//
//	OtOctahedronGeometryClass::OtOctahedronGeometryClass
//

OtOctahedronGeometryClass::OtOctahedronGeometryClass() {
	baseVertices = bv;
	baseIndices = bi;
	baseVertexCount = sizeof(bv) / sizeof(bv[0]);
	baseIndexCount = sizeof(bi) / sizeof(bi[0]);
}


//
//	OtOctahedronGeometryClass::serialize
//

nlohmann::json OtOctahedronGeometryClass::serialize(std::filesystem::path* basedir) {
	auto data = OtPolyhedronGeometryClass::serialize(basedir);
	data["type"] = name;
	return data;
}


//
//	OtOctahedronGeometryClass::getMeta
//

OtType OtOctahedronGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtOctahedronGeometryClass>("OctahedronGeometry", OtPolyhedronGeometryClass::getMeta());
	}

	return type;
}
