//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/module.h"
#include "ot/class.h"

#include "http.h"
#include "url.h"
#include "router.h"
#include "server.h"


//
//	Http module
//

extern "C" void init(OtModule module) {
	module->set("Http", OtClassClass::create(OtHttpClass::getMeta()));
	module->set("URL", OtClassClass::create(OtURLClass::getMeta()));
	module->set("HttpRouter", OtClassClass::create(OtHttpRouterClass::getMeta()));
	module->set("HttpServer", OtClassClass::create(OtHttpServerClass::getMeta()));
}
