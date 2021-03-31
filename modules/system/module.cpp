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

#include "system.h"
#include "path.h"
#include "os.h"
#include "fs.h"


//
//	System module
//

extern "C" void init(OtModule module) {
	module->set("System", OtClassClass::create(OtSystemClass::getMeta()));
	module->set("Path", OtClassClass::create(OtPathClass::getMeta()));
	module->set("OS", OtClassClass::create(OtOSClass::getMeta()));
	module->set("FS", OtClassClass::create(OtFSClass::getMeta()));
}
