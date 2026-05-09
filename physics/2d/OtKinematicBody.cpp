//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtKinematicBody.h"
#include "OtWorld2d.h"


//
//	OtKinematicBodyClass::init
//

void OtKinematicBodyClass::init(OtObject w) {
	// sanity check
	w.expect<OtWorld2dClass>("World");
	auto worldId = OtWorld2d(w)->getWorldId();

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_kinematicBody;
	bodyDef.userData = this;
	bodyId = b2CreateBody(worldId, &bodyDef);
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
