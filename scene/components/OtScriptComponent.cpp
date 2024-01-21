//	ObjectTalk Scripting Language
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
#include "OtClass.h"
#include "OtVM.h"

#include "OtMessageBus.h"
#include "OtUi.h"

#include "OtPathTools.h"
#include "OtEntityObject.h"
#include "OtScriptComponent.h"


//
//	New script template
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
}\n\
";


//
//	OtScriptComponent::renderUI
//

bool OtScriptComponent::renderUI() {
	return OtUiFileSelector(
		"Path##ScriptPath",
		path,
		[](std::string& path) {
			// create a new script file (and give it the right extension; can't trust the user :-)
			auto realPath = OtPathReplaceExtension(path, ".ot");
			std::ofstream stream(realPath);

			std::string script{scriptTemplate};
			script.replace(script.find("$"), 1, OtPathGetStem(realPath));

			stream << script;
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
//	OtScriptComponent::load
//

void OtScriptComponent::load() {
	// load the script and compile it
	module = OtModule::create();
	module->load(path);

	// see if the module has the right class definition
	auto className = OtPathGetStem(path);

	if (module->hasByName(className)) {
		auto classObject = module->getByName(className);

		// ensure it is a class object
		if (classObject.isKindOf<OtClassClass>()) {
			// create instance of class
			instance = OtClass(classObject)->instantiate(0, nullptr);

			// ensure the class is derived from Entity
			if (instance.isKindOf<OtEntityObjectClass>()) {
				createSelector = OtSelector::create("create");
				updateSelector = OtSelector::create("update");
				hasCreateMethod = instance->has(createSelector);
				hasUpdateMethod = instance->has(updateSelector);

			} else {
				OtError("Class [%s] in script [%s] is not dereive from [Entity]", className.c_str(), path.c_str());
			}

		} else {
			OtError("Object [%s] in script [%s] is not a class", className.c_str(), path.c_str());
		}

	} else {
		OtError("Script [%s] does not contain class [%s]", path.c_str(), className.c_str());
	}
}


//
//	OtScriptComponent::create
//

void OtScriptComponent::create() {
	if (hasCreateMethod) {
		OtVM::instance()->callMemberFunction(instance, createSelector);
	}
}


//
//	OtScriptComponent::update
//

void OtScriptComponent::update() {
	if (hasUpdateMethod) {
		OtVM::instance()->callMemberFunction(instance, updateSelector);
	}
}


//
//	OtScriptComponent::serialize
//

nlohmann::json OtScriptComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtPathRelative(path, basedir);
	return data;
}


//
//	OtScriptComponent::deserialize
//

void OtScriptComponent::deserialize(nlohmann::json data, std::string* basedir) {
	path = OtPathGetAbsolute(data, "path", basedir);
}
