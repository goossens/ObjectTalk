//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtModule.h"

#include "OtScene2.h"
#include "OtStage.h"


//
//	OtStageClass::registerModule
//

void OtStageClass::registerModule() {
	OtModuleClass::registerInternal("Stage", [](OtModule module) {
		module->set("Stage", OtClassClass::create(OtStageClass::getMeta()));

		module->set("Scene2", OtClassClass::create(OtScene2Class::getMeta()));
	});
}


//
//	OtStageClass::getMeta
//

OtType OtStageClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtStageClass>("Stage", OtObjectClass::getMeta());
	}

	return type;
}
