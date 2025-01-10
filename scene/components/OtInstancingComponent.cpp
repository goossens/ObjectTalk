//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"


#include "OtInstancingComponent.h"


//
//	OtInstancingComponent::renderUI
//

bool OtInstancingComponent::renderUI() {
	return asset.renderUI("Instances");
}


//
//	OtInstancingComponent::serialize
//

nlohmann::json OtInstancingComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtAssetSerialize(asset.getPath(), basedir);
	return data;
}


//
//	OtInstancingComponent::deserialize
//

void OtInstancingComponent::deserialize(nlohmann::json data, std::string* basedir) {
	asset = OtAssetDeserialize(&data, "path", basedir);
}

