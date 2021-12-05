//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "scene.h"
#include "sceneobject.h"

#include "ot/exception.h"


//
//	OtSceneClass::validateChild
//

void OtSceneClass::validateChild(OtComponent child) {
	if (!child->isKindOf("SceneObject")) {
		OtExcept("A [Scene] can only have [SceneObjects] as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtSceneClass::preRender
//

void OtSceneClass::preRender(OtRenderingContext* context) {

	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtSceneObjectClass>()->preRender(context);
		}
	}
}


//
//	OtSceneClass::render
//

void OtSceneClass::render(OtRenderingContext* context) {
	// render all scene objects
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtSceneObjectClass>()->render(context);
		}
	}
}


//
//	OtSceneClass::getMeta
//

OtType OtSceneClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtSceneClass>("Scene", OtComponentClass::getMeta());
	}

	return type;
}


//
//	OtSceneClass::create
//

OtScene OtSceneClass::create() {
	OtScene scene = std::make_shared<OtSceneClass>();
	scene->setType(getMeta());
	return scene;
}
