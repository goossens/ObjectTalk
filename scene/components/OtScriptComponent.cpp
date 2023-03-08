//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>

#include "glm/ext.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"
#include "OtMessageBus.h"
#include "OtUi.h"

#include "OtComponentTools.h"
#include "OtScriptComponent.h"


//
//	new script template
//

static const char* scriptTemplate = "\
var scene = import(\"scene\");\n\
\n\
class $ : scene.Entity {\n\
	function create(this) {\n\
	}\n\
\n\
	function update(this) {\n\
	}\n\
};\n\
";


//
//	OtScriptComponent::renderGUI
//

bool OtScriptComponent::renderGUI() {
	return OtUiFileSelector(
		"Path", path,
		[](std::filesystem::path& path) {
			// create a new script file (and give it the right extension; can't trust the user :-)
			path.replace_extension(".ot");
			std::ofstream stream(path);

			std::string script{scriptTemplate};
			script.replace(script.find("$"), 1, path.stem().string());

			stream << script;
			stream.close();

			// open it in the editor
			OtMessageBus::instance()->send("open " + path.string());

		},
		[](std::filesystem::path& path) {
			std::cout << path << std::endl;
			// open in editor
			OtMessageBus::instance()->send("open " + path.string());
		});
}


//
//	OtScriptComponent::serialize
//

nlohmann::json OtScriptComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtComponentGetRelativePath(path, basedir);
	return data;
}


//
//	OtScriptComponent::deserialize
//

void OtScriptComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	path = OtComponentGetAbsolutePath(data, "path", basedir);
}
