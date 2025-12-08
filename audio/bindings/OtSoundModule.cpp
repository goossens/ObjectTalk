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
#include "OtLog.h"
#include "OtModule.h"

#include "OtSoundModule.h"


//
//	OtSoundClass::init
//

void OtSoundClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {
		sound.load(parameters[0]->operator std::string());

	} else if (count != 0) {
		OtLogError("[Sound] constructor expects 0 or 1 arguments (not {})", count);
	}
}


//
//	OtSoundClass::getMeta
//

OtType OtSoundClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtSoundClass>("Sound", OtObjectClass::getMeta());

		type->set("__init__", OtFunction::create(&OtSoundClass::init));
		type->set("load", OtFunction::create(&OtSoundClass::load));

		type->set("play", OtFunction::create(&OtSoundClass::play));
		type->set("loop", OtFunction::create(&OtSoundClass::loop));
		type->set("pause", OtFunction::create(&OtSoundClass::pause));
		type->set("resume", OtFunction::create(&OtSoundClass::resume));
		type->set("stop", OtFunction::create(&OtSoundClass::stop));

		type->set("isPlaying", OtFunction::create(&OtSoundClass::isPlaying));
	}

	return type;
}


//
//	Module registration
//

static OtModuleRegistration registration{"sound", [](OtModule module) {
	// register classes
	module->set("Sound", OtClass::create(OtSoundClass::getMeta()));
}};
