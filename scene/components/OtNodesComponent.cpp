//	ObjectTalk Flowing Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtMessageBus.h"
#include "OtUi.h"

#include "OtNodesComponent.h"
#include "OtPathTools.h"


//
//	New flow template
//

static const char* flowTemplate = "\
{\n\
	\"metadata\": {},\n\
	\"nodes\": [],\n\
	\"links\": []\n\
}\n";


//
//	OtNodesComponent::renderUI
//

bool OtNodesComponent::renderUI() {
	return asset.renderUI("Path##FlowPath", [](const std::string& path) {
		// create a new nodes file
		std::ofstream stream(path);
		stream << flowTemplate;
		stream.close();
	});
}


//
//	OtNodesComponent::serialize
//

nlohmann::json OtNodesComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtPathRelative(asset.getPath(), basedir);
	return data;
}


//
//	OtNodesComponent::deserialize
//

void OtNodesComponent::deserialize(nlohmann::json data, std::string* basedir) {
	asset = OtPathGetAbsolute(data, "path", basedir);
}
