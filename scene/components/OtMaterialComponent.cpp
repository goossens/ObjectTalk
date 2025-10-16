//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtMaterialComponent.h"


//
//	OtMaterialComponent::OtMaterialComponent
//

OtMaterialComponent::OtMaterialComponent() {
	material = std::make_shared<OtMaterial>();
}


//
//	OtMaterialComponent::renderUI
//

bool OtMaterialComponent::renderUI() {
	return material->renderUI();
}


//
//	OtMaterialComponent::serialize
//

nlohmann::json OtMaterialComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["material"] = material->serialize(basedir);
	return data;
}


//
//	OtMaterialComponent::deserialize
//

void OtMaterialComponent::deserialize(nlohmann::json data, std::string* basedir) {
	material->deserialize(data["material"], basedir);
}
