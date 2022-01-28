//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtModule.h"

#include "OtHttp.h"
#include "OtURL.h"
#include "OtHttpRouter.h"
#include "OtHttpServer.h"


//
//	OtHttpClass::registerModule
//

void OtHttpClass::registerModule() {
	OtModuleClass::registerInternal("http", [](OtModule module) {
		module->set("Http", OtClassClass::create(OtHttpClass::getMeta()));
		module->set("URL", OtClassClass::create(OtURLClass::getMeta()));
		module->set("Router", OtClassClass::create(OtHttpRouterClass::getMeta()));
		module->set("Server", OtClassClass::create(OtHttpServerClass::getMeta()));
	});
}


//
//	OtHttpClass::getMeta
//

OtType OtHttpClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtHttpClass>("Http", OtObjectClass::getMeta());
	}

	return type;
}
