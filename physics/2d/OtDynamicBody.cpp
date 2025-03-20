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

#include "OtDynamicBody.h"
#include "OtWorld.h"


//
//	OtDynamicBodyClass::init
//

void OtDynamicBodyClass::init(OtObject w) {
	// sanity check
	if (!w.isKindOf<OtWorldClass>()) {
		OtLogError("Expected a [World], not a [{}]", w.getTypeName());
	}

	OtWorld world = OtWorld(w);

	b2BodyDef def;
	def.type = b2_dynamicBody;
	body = world->world->CreateBody(&def);
	body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}



//
//	OtDynamicBodyClass::getMeta
//

OtType OtDynamicBodyClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtDynamicBodyClass>("DynamicBody", OtBodyClass::getMeta());
		type->set("__init__", OtFunction::create(&OtDynamicBodyClass::init));
	}

	return type;
}
