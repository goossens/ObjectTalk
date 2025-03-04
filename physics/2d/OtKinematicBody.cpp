//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtLog.h"

#include "OtKinematicBody.h"
#include "OtWorld.h"


//
//	OtKinematicBodyClass::init
//

void OtKinematicBodyClass::init(OtObject w) {
	// sanity check
	if (!w.isKindOf<OtWorldClass>()) {
		OtLogError("Expected a [World], not a [{}]", w->getType()->getName());
	}

	OtWorld world = OtWorld(w);

	b2BodyDef def;
	def.type = b2_kinematicBody;
	body = world->world->CreateBody(&def);
	body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}



//
//	OtKinematicBodyClass::getMeta
//

OtType OtKinematicBodyClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtKinematicBodyClass>("KinematicBody", OtBodyClass::getMeta());
		type->set("__init__", OtFunction::create(&OtKinematicBodyClass::init));
	}

	return type;
}
