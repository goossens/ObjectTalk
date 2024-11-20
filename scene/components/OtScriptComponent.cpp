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
#include "OtIdentifier.h"
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
	return script.renderUI("Path##ScriptPath", [](const std::string& path) {
		// create a new script file
		std::ofstream stream(path);

		std::string script{scriptTemplate};
		script.replace(script.find("$"), 1, OtPathGetStem(path));

		stream << script;
		stream.close();
	});
}


//
//	OtScriptComponent::process
//

void OtScriptComponent::process() {
	// ensure asset is ready
	if (script.isReady()) {
		// load the script and compile it
		script->compile();

		// see if the module has the right class definition
		auto path = script.getPath();
		auto className = OtPathGetStem(path);
		auto module = script->getModule();

		if (module->hasByName(className)) {
			auto classObject = module->getByName(className);

			// ensure it is a class object
			if (classObject.isKindOf<OtClassClass>()) {
				// create instance of class
				instance = OtClass(classObject)->instantiate();

				// ensure the class is derived from Entity
				if (instance.isKindOf<OtEntityObjectClass>()) {
					hasCreateMethod = instance->has(createSymbol);
					hasUpdateMethod = instance->has(updateSymbol);

				} else {
					OtError("Class [{}] in script [{}] is not derived from [Entity]", className, path);
				}

			} else {
				OtError("Object [{}] in script [{}] is not a class", className, path);
			}

		} else {
			OtError("Script [{}] does not contain class [{}]", path, className);
		}
	}
}


//
//	OtScriptComponent::create
//

void OtScriptComponent::create() {
	if (hasCreateMethod) {
		OtVM::callMemberFunction(instance, createSymbol);
	}
}


//
//	OtScriptComponent::update
//

void OtScriptComponent::update() {
	if (hasUpdateMethod) {
		OtVM::callMemberFunction(instance, updateSymbol);
	}
}


//
//	OtScriptComponent::serialize
//

nlohmann::json OtScriptComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtPathRelative(script.getPath(), basedir);
	return data;
}


//
//	OtScriptComponent::deserialize
//

void OtScriptComponent::deserialize(nlohmann::json data, std::string* basedir) {
	script = OtPathGetAbsolute(data, "path", basedir);
}
