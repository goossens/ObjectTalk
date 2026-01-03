//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtGrassComponent.h"


//
//	OtGrassComponent::OtGrassComponent
//

OtGrassComponent::OtGrassComponent() {
	grass = std::make_shared<OtGrass>();
}


//
//	OtGrassComponent::renderUI
//

bool OtGrassComponent::renderUI() {
	return grass->renderUI();
}


//
//	OtGrassComponent::serialize
//

nlohmann::json OtGrassComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["grass"] = grass->serialize(basedir);
	return data;
}


//
//	OtGrassComponent::deserialize
//

void OtGrassComponent::deserialize(nlohmann::json data, std::string* basedir) {
	grass->deserialize(data["grass"], basedir);
}
