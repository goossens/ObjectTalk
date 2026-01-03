//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "imgui.h"
#include "nlohmann/json.hpp"


#include "OtTetrahedronPrimitive.h"


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
//	OtTetrahedronPrimitive::OtTetrahedronPrimitive
//

OtTetrahedronPrimitive::OtTetrahedronPrimitive() {
	baseVertices = bv;
	baseIndices = bi;
	baseVertexCount = sizeof(bv) / sizeof(bv[0]);
	baseIndexCount = sizeof(bi) / sizeof(bi[0]);
}


//
//	OtTetrahedronPrimitive::serialize
//

nlohmann::json OtTetrahedronPrimitive::serialize(std::string* basedir) {
	auto data = OtPolyhedronPrimitive::serialize(basedir);
	data["type"] = name;
	return data;
}
