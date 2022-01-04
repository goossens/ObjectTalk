//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "fixture.h"


//
//	OtFixtureClass::setDensity
//

OtObject OtFixtureClass::setDensity(float density) {
	fixture->SetDensity(density);
	fixture->GetBody()->ResetMassData();
	return shared();
}


//
//	OtFixtureClass::setFriction
//

OtObject OtFixtureClass::setFriction(float friction) {
	fixture->SetFriction(friction);
	fixture->GetBody()->ResetMassData();
	return shared();
}


//
//	OtFixtureClass::setRestitution
//

OtObject OtFixtureClass::setRestitution(float restitution) {
	fixture->SetRestitution(restitution);
	fixture->GetBody()->ResetMassData();
	return shared();
}


//
//	OtFixtureClass::getMeta
//

OtType OtFixtureClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtFixtureClass>("Fixture", OtGuiClass::getMeta());
		type->set("setDensity", OtFunctionClass::create(&OtFixtureClass::setDensity));
		type->set("setFriction", OtFunctionClass::create(&OtFixtureClass::setFriction));
		type->set("setRestitution", OtFunctionClass::create(&OtFixtureClass::setRestitution));

		type->set("getDensity", OtFunctionClass::create(&OtFixtureClass::getDensity));
		type->set("getFriction", OtFunctionClass::create(&OtFixtureClass::getFriction));
		type->set("getRestitution", OtFunctionClass::create(&OtFixtureClass::getRestitution));
	}

	return type;
}


//
//	OtFixtureClass::create
//

OtFixture OtFixtureClass::create() {
	OtFixture fixture = std::make_shared<OtFixtureClass>();
	fixture->setType(getMeta());
	return fixture;
}


OtFixture OtFixtureClass::create(b2Fixture* f) {
	OtFixture fixture = create();
	fixture->fixture = f;
	f->GetUserData().pointer = reinterpret_cast<uintptr_t>(fixture.get());
	return fixture;
}
