//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "box2d/box2d.h"

#include "OtGui.h"


//
//	OtFixtureClass
//

class OtFixtureClass;
typedef std::shared_ptr<OtFixtureClass> OtFixture;

class OtFixtureClass : public OtGuiClass {
public:
	// get type definition
	static OtType getMeta();

	// access properties
	OtObject setDensity(float density);
	OtObject setFriction(float friction);
	OtObject setRestitution(float restitution);

	float getDensity() { return fixture->GetDensity(); }
	float getFriction() { return fixture->GetFriction(); }
	float getRestitution() { return fixture->GetRestitution(); }

	// create a new object
	static OtFixture create();
	static OtFixture create(b2Fixture* body);

private:
	// tracking our Box2D object
	b2Fixture* fixture = nullptr;
};
