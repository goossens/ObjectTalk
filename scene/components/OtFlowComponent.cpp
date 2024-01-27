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

#include "OtFlowComponent.h"
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
//	OtFlowComponent::renderUI
//

bool OtFlowComponent::renderUI() {
	return graph.renderUI("Path##FlowPath", [](const std::string& path) {
		// create a new graph file
		std::ofstream stream(path);
		stream << flowTemplate;
		stream.close();
	});
}


//
//	OtFlowComponent::serialize
//

nlohmann::json OtFlowComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtPathRelative(graph.getPath(), basedir);
	return data;
}


//
//	OtFlowComponent::deserialize
//

void OtFlowComponent::deserialize(nlohmann::json data, std::string* basedir) {
	graph = OtPathGetAbsolute(data, "path", basedir);
}
