//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"
#include "OtModule.h"
#include "OtObject.h"
#include "OtValue.h"

#include "OtManifold.h"
#include "OtManifoldModule.h"


//
//	Module registration
//

static OtModuleRegistration registration{"manifold", [](OtModule module) {
	// register classes
	module->set("Manifold", OtClass::create(OtManifoldClass::getMeta()));

	// register functions
	module->set("cube", OtFunction::create(OtManifoldClass::cube));
	module->set("cylinder", OtFunction::create(OtManifoldClass::cylinder));
	module->set("sphere", OtFunction::create(OtManifoldClass::sphere));
}};
