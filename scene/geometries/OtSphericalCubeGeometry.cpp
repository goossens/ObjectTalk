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

#include "OtSphericalCubeGeometry.h"


//
//	See https://en.wikipedia.org/wiki/Cube#Spherical_tiling
//


//
//	Globals
//

static glm::vec3 bv[] = {
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, 1.0f, -1.0f),
	glm::vec3(-1.0f, 1.0f,-1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
	glm::vec3(1.0f, -1.0f,  1.0f),
	glm::vec3(1.0f, 1.0f,  1.0f),
	glm::vec3(-1.0f, 1.0f, 1.0f)
};

static uint32_t bi[] = {
	2, 1, 0,
	0, 3, 2,
    0, 4, 7,
	7, 3, 0,
    0, 1, 5,
	5, 4, 0,
    1, 2, 6,
	6, 5, 1,
    2, 3, 7,
	7, 6, 2,
    4, 5, 6,
	6, 7, 4
};


//
//	OtSphericalCubeGeometryClass::OtSphericalCubeGeometryClass
//

OtSphericalCubeGeometryClass::OtSphericalCubeGeometryClass() {
	baseVertices = bv;
	baseIndices = bi;
	baseVertexCount = sizeof(bv) / sizeof(bv[0]);
	baseIndexCount = sizeof(bi) / sizeof(bi[0]);
}


//
//	OtSphericalCubeGeometryClass::serialize
//

nlohmann::json OtSphericalCubeGeometryClass::serialize(std::string* basedir) {
	auto data = OtPolyhedronGeometryClass::serialize(basedir);
	data["type"] = name;
	return data;
}


//
//	OtSphericalCubeGeometryClass::getMeta
//

OtType OtSphericalCubeGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtSphericalCubeGeometryClass>("SphericalCubeGeometry", OtPolyhedronGeometryClass::getMeta());
	}

	return type;
}
