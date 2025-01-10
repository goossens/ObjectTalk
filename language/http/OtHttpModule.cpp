//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtModule.h"

#include "OtURL.h"
#include "OtHttpRouter.h"
#include "OtHttpServer.h"


//
//	Module registration
//

static OtModuleRegistration registration{"http", [](OtModule module) {
	module->set("URL", OtClass::create(OtURLClass::getMeta()));
	module->set("Router", OtClass::create(OtHttpRouterClass::getMeta()));
	module->set("Server", OtClass::create(OtHttpServerClass::getMeta()));
}};
