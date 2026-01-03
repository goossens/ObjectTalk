//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtModule.h"

#include "OtDynamicBody.h"
#include "OtFixture.h"
#include "OtKinematicBody.h"
#include "OtStaticBody.h"
#include "OtWorld.h"


//
//	Module registration
//

static OtModuleRegistration registration{"physics2d", [](OtModule module) {
	module->set("DynamicBody", OtClass::create(OtDynamicBodyClass::getMeta()));
	module->set("Fixture", OtClass::create(OtFixtureClass::getMeta()));
	module->set("KinematicBody", OtClass::create(OtKinematicBodyClass::getMeta()));
	module->set("StaticBody", OtClass::create(OtStaticBodyClass::getMeta()));
	module->set("World", OtClass::create(OtWorldClass::getMeta()));
}};
