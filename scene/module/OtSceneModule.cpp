//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtModule.h"

#include "OtEntityObject.h"


//
//	OtSceneModuleRegister
//

void OtSceneModuleRegister() {
	OtModuleClass::registerInternal("scene", [](OtModule module) {
		module->set("Entity", OtClass::create(OtEntityObjectClass::getMeta()));
	});
}
