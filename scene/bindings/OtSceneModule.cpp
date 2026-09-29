//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"
#include "OtModule.h"

#include "OtMathModule.h" // IWYU pragma: keep

#include "OtSceneObject.h"
#include "OtEntityObject.h"
#include "OtComponentObject.h"

#include "OtBackgroundComponentObject.h"
#include "OtDirectionalLightComponentObject.h"
#include "OtCameraComponentObject.h"
#include "OtMaterialComponentObject.h"
#include "OtMessageComponentObject.h"
#include "OtScriptComponentObject.h"
#include "OtTransformComponentObject.h"


//
//	Module registration
//

static OtModuleRegistration registration{"scene", [](OtModule module) {
	module->set("Scene", OtClass::create(OtSceneObjectClass::getMeta()));
	module->set("Entity", OtClass::create(OtEntityObjectClass::getMeta()));
	module->set("Component", OtClass::create(OtComponentObjectClass::getMeta()));

	module->set("BackgroundComponent", OtClass::create(OtBackgroundComponentObjectClass::getMeta()));
	module->set("CameraComponent", OtClass::create(OtCameraComponentObjectClass::getMeta()));
	module->set("DirectionalLightComponent", OtClass::create(OtDirectionalLightComponentObjectClass::getMeta()));
	module->set("MaterialComponent", OtClass::create(OtMaterialComponentObjectClass::getMeta()));
	module->set("MessageComponent", OtClass::create(OtMessageComponentObjectClass::getMeta()));
	module->set("ScriptComponent", OtClass::create(OtScriptComponentObjectClass::getMeta()));
	module->set("TransformComponent", OtClass::create(OtTransformComponentObjectClass::getMeta()));
}};
