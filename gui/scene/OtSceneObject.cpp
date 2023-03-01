//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
		type = OtTypeClass::create<OtSceneObjectClass>("SceneObject", OtNodeClass::getMeta());

		type->set("castShadow", OtFunction::create(&OtSceneObjectClass::castShadow));
		type->set("castsShadow", OtFunction::create(&OtSceneObjectClass::castsShadow));
		type->set("receiveShadow", OtFunction::create(&OtSceneObjectClass::receiveShadow));
		type->set("receivesShadow", OtFunction::create(&OtSceneObjectClass::receivesShadow));
	}

	return type;
}
