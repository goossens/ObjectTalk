//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <uv.h>

#include "Ot.h"

#include "OtSystem.h"
#include "OtPath.h"
#include "OtOS.h"
#include "OtFS.h"


//
//	System module
//

extern "C" void init(OtObject context) {
	context->set("System", OtClassClass::create(OtSystemClass::getMeta()));
	context->set("Path", OtClassClass::create(OtPathClass::getMeta()));
	context->set("OS", OtClassClass::create(OtOSClass::getMeta()));
	context->set("FS", OtClassClass::create(OtFSClass::getMeta()));
}
