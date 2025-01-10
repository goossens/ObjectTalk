//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtFixture.h"


//
//	OtFixtureClass::setDensity
//

OtObject OtFixtureClass::setDensity(float density) {
	fixture->SetDensity(density);
	fixture->GetBody()->ResetMassData();
	return OtObject(this);
}


//
//	OtFixtureClass::setFriction
//

OtObject OtFixtureClass::setFriction(float friction) {
	fixture->SetFriction(friction);
	fixture->GetBody()->ResetMassData();
	return OtObject(this);
}


//
//	OtFixtureClass::setRestitution
//

OtObject OtFixtureClass::setRestitution(float restitution) {
	fixture->SetRestitution(restitution);
	fixture->GetBody()->ResetMassData();
	return OtObject(this);
}


//
//	OtFixtureClass::getMeta
//

OtType OtFixtureClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtFixtureClass>("Fixture", OtPhysics2DClass::getMeta());
		type->set("setDensity", OtFunction::create(&OtFixtureClass::setDensity));
		type->set("setFriction", OtFunction::create(&OtFixtureClass::setFriction));
		type->set("setRestitution", OtFunction::create(&OtFixtureClass::setRestitution));

		type->set("getDensity", OtFunction::create(&OtFixtureClass::getDensity));
		type->set("getFriction", OtFunction::create(&OtFixtureClass::getFriction));
		type->set("getRestitution", OtFunction::create(&OtFixtureClass::getRestitution));
	}

	return type;
}

