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
#include "OtMaterialComponentObject.h"
#include "OtMessageComponentObject.h"
#include "OtModelComponentObject.h"
#include "OtScriptComponentObject.h"
#include "OtTransformComponentObject.h"
#include "OtSceneObject.h"


//
//	OtEntityObjectClass::getScene
//

OtObject OtEntityObjectClass::getScene() {
	return OtSceneObject::create(scene);
}


//
//	OtEntityObjectClass::hasModelComponent
//

bool OtEntityObjectClass::hasModelComponent() {
	return scene->hasComponent<OtModelComponent>(entity);
}


//
//	OtEntityObjectClass::getModelComponent
//

OtObject OtEntityObjectClass::getModelComponent() {
	return OtModelComponentObject::create(scene, entity);
}


//
//	OtEntityObjectClass::addModelComponent
//

OtObject OtEntityObjectClass::addModelComponent() {
	if (hasModelComponent()) {
		OtLogError("Entity already has a ModelComponent]");
	}

	scene->addComponent<OtModelComponent>(entity);
	return OtModelComponentObject::create(scene, entity);
}


//
//	OtEntityObjectClass::hasTransformComponent
//

bool OtEntityObjectClass::hasTransformComponent() {
	return scene->hasComponent<OtTransformComponent>(entity);
}


//
//	OtEntityObjectClass::getTransformComponent
//

OtObject OtEntityObjectClass::getTransformComponent() {
	return OtTransformComponentObject::create(scene, entity);
}


//
//	OtEntityObjectClass::addTransformComponent
//

OtObject OtEntityObjectClass::addTransformComponent() {
	if (hasTransformComponent()) {
		OtLogError("Entity already has a TransformComponent]");
	}

	scene->addComponent<OtTransformComponent>(entity);
	return OtTransformComponentObject::create(scene, entity);
}


//
//	OtEntityObjectClass::hasMaterialComponent
//

bool OtEntityObjectClass::hasMaterialComponent() {
	return scene->hasComponent<OtMaterialComponent>(entity);
}


//
//	OtEntityObjectClass::getMaterialComponent
//

OtObject OtEntityObjectClass::getMaterialComponent() {
	return OtMaterialComponentObject::create(scene, entity);
}


//
//	 OtEntityObjectClass::addMaterialComponent
//

OtObject OtEntityObjectClass::addMaterialComponent() {
	if (hasMaterialComponent()) {
		OtLogError("Entity already has a MaterialComponent]");
	}

	scene->addComponent<OtMaterialComponent>(entity);
	return OtMaterialComponentObject::create(scene, entity);
}


//
//	OtEntityObjectClass::hasScriptComponent
//

bool OtEntityObjectClass::hasScriptComponent() {
	return scene->hasComponent<OtScriptComponent>(entity);
}


//
//	OtEntityObjectClass::getScriptComponent
//

OtObject OtEntityObjectClass::getScriptComponent() {
	return OtScriptComponentObject::create(scene, entity);
}


//
//	OtEntityObjectClass::addScriptComponent
//

OtObject OtEntityObjectClass::addScriptComponent() {
	if (hasScriptComponent()) {
		OtLogError("Entity already has a ScriptComponent]");
	}

	scene->addComponent<OtScriptComponent>(entity);
	return OtScriptComponentObject::create(scene, entity);
}


//
//	OtEntityObjectClass::hasMessageComponent
//

bool OtEntityObjectClass::hasMessageComponent() {
	return scene->hasComponent<OtMessageComponent>(entity);
}


//
//	OtEntityObjectClass::getMessageComponent
//

OtObject OtEntityObjectClass::getMessageComponent() {
	return OtMessageComponentObject::create(scene, entity);
}


//
//	OtEntityObjectClass::addMessageComponent
//

OtObject OtEntityObjectClass::addMessageComponent() {
	if (hasMessageComponent()) {
		OtLogError("Entity already has a MessageComponent]");
	}

	scene->addComponent<OtMessageComponent>(entity);
	return OtMessageComponentObject::create(scene, entity);
}


//
//	OtEntityObjectClass::getMeta
//

OtType OtEntityObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtEntityObjectClass>("Entity", OtObjectClass::getMeta());
		type->set("getScene", OtFunction::create(&OtEntityObjectClass::getScene));

		type->set("hasModelComponent", OtFunction::create(&OtEntityObjectClass::hasModelComponent));
		type->set("getModelComponent", OtFunction::create(&OtEntityObjectClass::getModelComponent));
		type->set("addModelComponent", OtFunction::create(&OtEntityObjectClass::addModelComponent));

		type->set("hasTransformComponent", OtFunction::create(&OtEntityObjectClass::hasTransformComponent));
		type->set("getTransformComponent", OtFunction::create(&OtEntityObjectClass::getTransformComponent));
		type->set("addTransformComponent", OtFunction::create(&OtEntityObjectClass::addTransformComponent));

		type->set("hasMaterialComponent", OtFunction::create(&OtEntityObjectClass::hasMaterialComponent));
		type->set("getMaterialComponent", OtFunction::create(&OtEntityObjectClass::getMaterialComponent));
		type->set("addMaterialComponent", OtFunction::create(&OtEntityObjectClass::addMaterialComponent));

		type->set("hasScriptComponent", OtFunction::create(&OtEntityObjectClass::hasScriptComponent));
		type->set("getScriptComponent", OtFunction::create(&OtEntityObjectClass::getScriptComponent));
		type->set("addScriptComponent", OtFunction::create(&OtEntityObjectClass::addScriptComponent));

		type->set("hasMessageComponent", OtFunction::create(&OtEntityObjectClass::hasMessageComponent));
		type->set("getMessageComponent", OtFunction::create(&OtEntityObjectClass::getMessageComponent));
		type->set("addMessageComponent", OtFunction::create(&OtEntityObjectClass::addMessageComponent));
	}

	return type;
}
