//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtLog.h"

#include "OtDynamicBody.h"
#include "OtWorld2d.h"


//
//	OtDynamicBodyClass::init
//

void OtDynamicBodyClass::init(OtObject w) {
	// sanity check
	w.expect<OtWorld2dClass>("World");
	auto worldId = OtWorld2d(w)->getWorldId();

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.userData = this;
	bodyId = b2CreateBody(worldId, &bodyDef);
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
