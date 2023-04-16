//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"
#include "OtModule.h"

#include "OtImageObject.h"

#include "OtGraphicsModule.h"


//
//	OtGraphicsModuleRegister
//

void OtGraphicsModuleRegister() {
	OtModuleClass::registerInternal("graphics", [](OtModule module) {
		module->set("Image", OtClass::create(OtImageObjectClass::getMeta()));
	});
}
