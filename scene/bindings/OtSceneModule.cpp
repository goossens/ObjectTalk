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
#include "OtInteger.h"
#include "OtModule.h"

#include "OtSceneObject.h"
#include "OtEntityObject.h"
#include "OtComponentObject.h"

#include "OtBackgroundComponentObject.h"
#include "OtCameraComponentObject.h"
#include "OtDirectionalLightComponentObject.h"
#include "OtGeometryComponentObject.h"
#include "OtMaterialComponentObject.h"
#include "OtMessageComponentObject.h"
#include "OtScriptComponentObject.h"
#include "OtTransformComponentObject.h"

#include "OtPostProcessingComponentObject.h"


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
	module->set("GeometryComponent", OtClass::create(OtDirectionalLightComponentObjectClass::getMeta()));
	module->set("MaterialComponent", OtClass::create(OtMaterialComponentObjectClass::getMeta()));
	module->set("MessageComponent", OtClass::create(OtMessageComponentObjectClass::getMeta()));
	module->set("PostProcessing", OtClass::create(OtPostProcessingComponentObjectClass::getMeta()));
	module->set("ScriptComponent", OtClass::create(OtScriptComponentObjectClass::getMeta()));
	module->set("TransformComponent", OtClass::create(OtTransformComponentObjectClass::getMeta()));

	module->set("reinhardSimpleTonemap", OtInteger::create(static_cast<int>(OtPostProcessing::Tonemap::reinhardSimple)));
	module->set("reinhardExtendedTonemap", OtInteger::create(static_cast<int>(OtPostProcessing::Tonemap::reinhardExtended)));
	module->set("filmicTonemap", OtInteger::create(static_cast<int>(OtPostProcessing::Tonemap::filmic)));
	module->set("acesFilmicTonemap", OtInteger::create(static_cast<int>(OtPostProcessing::Tonemap::acesFilmic)));
	module->set("uncharted2Tonemap", OtInteger::create(static_cast<int>(OtPostProcessing::Tonemap::uncharted2)));
	module->set("lottesTonemap", OtInteger::create(static_cast<int>(OtPostProcessing::Tonemap::lottes)));
	module->set("uchimuraTonemap", OtInteger::create(static_cast<int>(OtPostProcessing::Tonemap::uchimura)));
}};
