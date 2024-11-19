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

#include "OtVec2.h"
#include "OtVec3.h"
#include "OtVec4.h"


//
//	Module registration
//

static OtModuleRegistration registration{"math", [](OtModule module) {
	module->set("Vec2", OtClass::create(OtVec2Class::getMeta()));
	module->set("Vec3", OtClass::create(OtVec3Class::getMeta()));
	module->set("Vec4", OtClass::create(OtVec4Class::getMeta()));
}};
