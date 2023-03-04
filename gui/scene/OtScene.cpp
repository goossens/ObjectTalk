//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtFramework.h"
#include "OtScene.h"
#include "OtSceneObject.h"


//
//	OtSceneClass::validateChild
//

void OtSceneClass::validateChild(OtNode child) {
	if (!child->isKindOf("SceneObject")) {
		OtExcept("A [Scene] can only have [SceneObjects] as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtSceneClass::preRender
//

void OtSceneClass::preRender(OtRenderer& renderer) {
	for (auto& child : children) {
		if (child->isEnabled()) {
			OtSceneObject(child)->preRender(renderer);
		}
	}
}


//
//	OtSceneClass::render
//

void OtSceneClass::render(OtRenderer& renderer) {
	for (auto& child : children) {
		if (child->isEnabled()) {
			OtSceneObject(child)->render(renderer);
		}
	}
}


//
//	OtSceneClass::getMeta
//

OtType OtSceneClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtSceneClass>("Scene", OtNodeClass::getMeta());
	}

	return type;
}
