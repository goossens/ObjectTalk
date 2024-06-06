//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtModule.h"

#include "OtAnimation.h"
#include "OtEntityObject.h"


//
//	Module registration
//

static OtModuleRegistration registration{"scene", [](OtModule module) {
	module->set("Animation", OtClass::create(OtAnimationClass::getMeta()));
	module->set("Entity", OtClass::create(OtEntityObjectClass::getMeta()));
}};
