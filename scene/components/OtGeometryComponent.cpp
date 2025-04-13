//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtGeometryComponent.h"
#include "OtUi.h"


//
//	OtGeometryComponent::renderUI
//

bool OtGeometryComponent::renderUI() {
	bool changed = asset.renderUI("Path##GeometryPath");
	changed |= OtUi::toggleButton("Transparent", &transparent);
	changed |= OtUi::toggleButton("Cull Back Faces", &cullback);
	changed |= OtUi::toggleButton("Cast shadow", &castShadow);
	changed |= OtUi::toggleButton("Wireframe", &wireframe);
	return changed;
}


//
//	OtGeometryComponent::serialize
//

nlohmann::json OtGeometryComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtAssetSerialize(asset.getPath(), basedir);
	data["transparent"] = transparent;
	data["cullback"] = cullback;
	data["castShadow"] = castShadow;
	data["wireframe"] = wireframe;
	return data;
}


//
//	OtGeometryComponent::deserialize
//

void OtGeometryComponent::deserialize(nlohmann::json data, std::string* basedir) {
	asset = OtAssetDeserialize(&data, "path", basedir);
	transparent = data.value("transparent", false);
	cullback = data.value("cullback", true);
	castShadow = data.value("castShadow", true);
	wireframe = data.value("wireframe", false);
}
