//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtBodyShape.h"


//
//	OtBodyShapeClass::enableContextEvents
//

OtObject OtBodyShapeClass::enableContextEvents(bool flag) {
	b2Shape_EnableContactEvents(shape, flag);
	return OtObject();
}


//
//	OtBodyShapeClass::setDensity
//

OtObject OtBodyShapeClass::setDensity(float density) {
	b2Shape_SetDensity(shape, density, true);
	return OtObject(this);
}


//
//	OtBodyShapeClass::setFriction
//

OtObject OtBodyShapeClass::setFriction(float friction) {
	b2Shape_SetFriction(shape, friction);
	return OtObject(this);
}


//
//	OtBodyShapeClass::setRestitution
//

OtObject OtBodyShapeClass::setRestitution(float restitution) {
	b2Shape_SetRestitution(shape, restitution);
	return OtObject(this);
}


//
//	OtBodyShapeClass::getMeta
//

OtType OtBodyShapeClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtBodyShapeClass>("Fixture", OtPhysics2DClass::getMeta());
		type->set("enableContextEvents", OtFunction::create(&OtBodyShapeClass::enableContextEvents));
		type->set("setDensity", OtFunction::create(&OtBodyShapeClass::setDensity));
		type->set("setFriction", OtFunction::create(&OtBodyShapeClass::setFriction));
		type->set("setRestitution", OtFunction::create(&OtBodyShapeClass::setRestitution));

		type->set("areContextEventsEnabled", OtFunction::create(&OtBodyShapeClass::areContextEventsEnabled));
		type->set("getDensity", OtFunction::create(&OtBodyShapeClass::getDensity));
		type->set("getFriction", OtFunction::create(&OtBodyShapeClass::getFriction));
		type->set("getRestitution", OtFunction::create(&OtBodyShapeClass::getRestitution));
	}

	return type;
}

