//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtOctahedronPrimitive.h"


//
//	See https://en.wikipedia.org/wiki/Octahedron
//


//
//	Globals
//

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
//	OtOctahedronPrimitive::OtOctahedronPrimitive
//

OtOctahedronPrimitive::OtOctahedronPrimitive() {
	baseVertices = bv;
	baseIndices = bi;
	baseVertexCount = sizeof(bv) / sizeof(bv[0]);
	baseIndexCount = sizeof(bi) / sizeof(bi[0]);
}


//
//	OtOctahedronPrimitive::serialize
//

nlohmann::json OtOctahedronPrimitive::serialize(std::string* basedir) {
	auto data = OtPolyhedronPrimitive::serialize(basedir);
	data["type"] = name;
	return data;
}
