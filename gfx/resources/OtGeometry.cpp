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

#include "OtGeometry.h"


//
//	OtGeometry::OtGeometry
//

OtGeometry::OtGeometry() {
	mesh = std::make_shared<OtMesh>();
	mesh->generateCube();
	incrementVersion();
}

OtGeometry::OtGeometry(const std::string& path) {
	mesh = std::make_shared<OtMesh>();
	mesh->load(path);
	incrementVersion();
}


//
//	OtGeometry::clear
//

void OtGeometry::clear() {
	mesh = std::make_shared<OtMesh>();
	incrementVersion();
}
