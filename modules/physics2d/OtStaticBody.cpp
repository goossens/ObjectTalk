//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtStaticBody.h"
#include "OtWorld.h"


//
//	OtStaticBodyClass::init
//

void OtStaticBodyClass::init(OtObject w) {
	// sanity check
	if (!w.isKindOf<OtWorldClass>()) {
		OtError("Expected a [World], not a [{}]", w->getType()->getName());
	}

	OtWorld world = OtWorld(w);

	b2BodyDef def;
	def.type = b2_staticBody;
	body = world->world->CreateBody(&def);
}



//
//	OtStaticBodyClass::getMeta
//

OtType OtStaticBodyClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtStaticBodyClass>("StaticBody", OtBodyClass::getMeta());
		type->set("__init__", OtFunction::create(&OtStaticBodyClass::init));
	}

	return type;
}
