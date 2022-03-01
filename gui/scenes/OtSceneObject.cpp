//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtSceneObject.h"


//
//	OtSceneObjectClass::getMeta
//

OtType OtSceneObjectClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSceneObjectClass>("SceneObject", OtComponentClass::getMeta());

		type->set("castShadow", OtFunctionClass::create(&OtSceneObjectClass::castShadow));
		type->set("castsShadow", OtFunctionClass::create(&OtSceneObjectClass::castsShadow));
		type->set("receiveShadow", OtFunctionClass::create(&OtSceneObjectClass::receiveShadow));
		type->set("receivesShadow", OtFunctionClass::create(&OtSceneObjectClass::receivesShadow));
	}

	return type;
}
