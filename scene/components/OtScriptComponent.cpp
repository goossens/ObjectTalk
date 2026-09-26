//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtClass.h"
#include "OtException.h"
#include "OtLog.h"
#include "OtPath.h"
#include "OtText.h"
#include "OtVM.h"

#include "OtScriptComponent.h"
#include "OtScriptComponentObject.h"


//
//	New script template
//

static const char* scriptTemplate = "\
var scene = import(\"scene\");\n\
\n\
class $ : scene.ScriptComponent {\n\
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
		std::string script{scriptTemplate};
		script.replace(script.find("$"), 1, OtPath::getStem(path));
		OtText::save(path, script);
	});
}


//
//	OtScriptComponent::update
//

void OtScriptComponent::update(OtScene* scene, OtEntity entity) {
	// ensure script asset is ready and initialized
	if (!initialized && script.isReady()) {
		// see if the module has the right class definition
		auto path = script.getPath();
		auto className = OtPath::getStem(path);
		auto module = script->getModule();

		if (module->hasByName(className)) {
			auto classObject = module->getByName(className);

			// ensure it is a class object
			if (classObject.isKindOf<OtClassClass>()) {
				// create instance of class
				instance = OtClass(classObject)->instantiate();

				// ensure the class is derived from ScriptComponent
				if (instance.isKindOf<OtScriptComponentObjectClass>()) {
					hasCreateMethod = instance->has(createSymbol);
					hasUpdateMethod = instance->has(updateSymbol);

				} else {
					OtLogError("Class [{}] in script [{}] is not derived from [ScriptComponent]", className, path);
				}

			} else {
				OtLogError("Object [{}] in script [{}] is not a class", className, path);
			}

		} else {
			OtLogError("Script [{}] does not contain class [{}]", path, className);
		}

		// this is a hack because we need to connect the component to the ECS
		OtScriptComponentObject(instance)->setEcsReferences(scene, entity);

		if (hasCreateMethod) {
			OtVM::callMemberFunction(instance, createSymbol);
		}

		initialized = true;

	} else if (script.isInvalid() || script.isMissing()) {
		throw OtException(script.getErrorMessage());
	}

	if (hasUpdateMethod) {
		OtVM::callMemberFunction(instance, updateSymbol);
	}
}


//
//	OtScriptComponent::serialize
//

nlohmann::json OtScriptComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["path"] = OtAssetSerialize(script.getPath(), basedir);
	return data;
}


//
//	OtScriptComponent::deserialize
//

void OtScriptComponent::deserialize(nlohmann::json& data, std::string* basedir) {
	script = OtAssetDeserialize(&data, "path", basedir);
}
