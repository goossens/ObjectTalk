//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtLog.h"

#include "OtEntityObject.h"
#include "OtSceneObject.h"
#include "OtScriptComponentObject.h"


//
//	OtScriptComponentObjectClass::getScene
//

OtObject OtScriptComponentObjectClass::getScene() {
	return OtSceneObject::create(scene);
}


//
//	OtScriptComponentObjectClass::getEntity
//

OtObject OtScriptComponentObjectClass::getEntity() {
	return OtEntityObject::create(scene, entity);
}


//
//	OtScriptComponentObjectClass::getMeta
//

OtType OtScriptComponentObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtScriptComponentObjectClass>("ScriptComponent", OtObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtScriptComponentObjectClass::invalidConstructor));

		type->set("getScene", OtFunction::create(&OtScriptComponentObjectClass::getScene));
		type->set("getEntity", OtFunction::create(&OtScriptComponentObjectClass::getEntity));
	}

	return type;
}


//
//	OtScriptComponentObjectClass::invalidConstructor
//

void OtScriptComponentObjectClass::invalidConstructor(size_t, OtObject*) {
	OtLogError("Class [ScriptComponent] can't be directly instantiated, create component via [Entity] class instead");
}
