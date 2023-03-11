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

#include "OtException.h"
#include "OtClass.h"
#include "OtVM.h"

#include "OtGlm.h"
#include "OtMessageBus.h"
#include "OtUi.h"

#include "OtComponentTools.h"
#include "OtEntityObject.h"
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
			// open in editor
			OtMessageBus::instance()->send("open " + path.string());
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
	auto className = path.stem().string();

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
				OtExcept("Class [%s] in script [%s] is not dereive from [Entity]", className.c_str(), path.string().c_str());
			}

		} else {
			OtExcept("Object [%s] in script [%s] is not a class", className.c_str(), path.string().c_str());
		}

	} else {
		OtExcept("Script [%s] does not contain class [%s]", path.string().c_str(), className.c_str());
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
