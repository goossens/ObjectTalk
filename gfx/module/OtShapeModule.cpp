//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtClass.h"
#include "OtModule.h"

#include "OtShapeObject.h"


//
//	Module registration
//

static OtModuleRegistration registration{"shape", [](OtModule module) {
	// register classes
	module->set("Shape", OtClass::create(OtShapeClass::getMeta()));
}};
