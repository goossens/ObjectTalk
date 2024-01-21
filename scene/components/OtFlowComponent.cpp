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
	return OtUiFileSelector(
		"Path##FlowPath",
		path,
		[](std::string& path) {
			// create a new graph file (and give it the right extension; can't trust the user :-)
			auto realPath = OtPathReplaceExtension(path, ".otg");
			std::ofstream stream(realPath);
			stream << flowTemplate;
			stream.close();

			// open it in the editor
			OtMessageBus::instance()->send("openinwindow " + realPath);

		},
		[](std::string& path) {
			// open it in the editor
			OtMessageBus::instance()->send("openinwindow " + path);
		});
}


//
//	OtFlowComponent::serialize
//

nlohmann::json OtFlowComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtPathRelative(path, basedir);
	return data;
}


//
//	OtFlowComponent::deserialize
//

void OtFlowComponent::deserialize(nlohmann::json data, std::string* basedir) {
	path = OtPathGetAbsolute(data, "path", basedir);
}
