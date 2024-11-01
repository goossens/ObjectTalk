//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtGeometryComponent.h"
#include "OtUi.h"


//
//	OtGeometryComponent::renderUI
//

bool OtGeometryComponent::renderUI() {
	bool changed = asset.renderUI("Path##GeometryPath");
	changed |= OtUi::toggleButton("Wireframe", &wireframe);
	changed |= OtUi::toggleButton("Transparent", &transparent);
	changed |= OtUi::toggleButton("Cull Back Faces", &cullback);
	return changed;
}


//
//	OtGeometryComponent::serialize
//

nlohmann::json OtGeometryComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtPathRelative(asset.getPath(), basedir);
	data["wireframe"] = wireframe;
	data["transparent"] = transparent;
	data["cullback"] = cullback;
	return data;
}


//
//	OtGeometryComponent::deserialize
//

void OtGeometryComponent::deserialize(nlohmann::json data, std::string* basedir) {
	asset = OtPathGetAbsolute(data, "path", basedir);
	wireframe = data.value("wireframe", false);
	transparent = data.value("transparent", false);
	cullback = data.value("cullback", true);
}
