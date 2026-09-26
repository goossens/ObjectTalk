//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtEntityObject.h"
#include "OtSceneObject.h"
#include "OtScriptComponentObject.h"


//
//	OtScriptComponentObjectClass::getMeta
//

OtType OtScriptComponentObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtScriptComponentObjectClass>("ScriptComponent", OtComponentObjectClass::getMeta());
	}

	return type;
}
