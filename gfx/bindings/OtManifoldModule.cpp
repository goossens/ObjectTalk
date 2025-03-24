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
//	OtManifoldClass::getMeta
//

OtType OtManifoldClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtManifoldClass>("Manifold", OtObjectClass::getMeta());
		type->set("clear", OtFunction::create(&OtManifoldClass::clear));

		type->set("union", OtFunction::create(&OtManifoldClass::unionManifolds));
		type->set("difference", OtFunction::create(&OtManifoldClass::differenceManifolds));
		type->set("intersect", OtFunction::create(&OtManifoldClass::intersectManifolds));

		type->set("translate", OtFunction::create(&OtManifoldClass::translate));
		type->set("rotate", OtFunction::create(&OtManifoldClass::rotate));
		type->set("scale", OtFunction::create(&OtManifoldClass::scale));
		type->set("mirror", OtFunction::create(&OtManifoldClass::mirror));
	}

	return type;
}


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
