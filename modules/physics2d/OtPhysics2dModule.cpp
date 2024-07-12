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

#include "OtBody.h"
#include "OtFixture.h"
#include "OtWorld.h"


//
//	Module registration
//

static OtModuleRegistration registration{"physics2d", [](OtModule module) {
	module->set("Body", OtClass::create(OtBodyClass::getMeta()));
	module->set("Fixture", OtClass::create(OtFixtureClass::getMeta()));
	module->set("World", OtClass::create(OtWorldClass::getMeta()));
}};
