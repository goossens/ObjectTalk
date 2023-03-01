//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
using OtFixture = OtObjectPointer<OtFixtureClass>;

class OtFixtureClass : public OtGuiClass {
public:
	// constructors
	OtFixtureClass() = default;
	OtFixtureClass(b2Fixture* f) : fixture(f) {}

	// get type definition
	static OtType getMeta();

	// access properties
	OtObject setDensity(float density);
	OtObject setFriction(float friction);
	OtObject setRestitution(float restitution);

	float getDensity() { return fixture->GetDensity(); }
	float getFriction() { return fixture->GetFriction(); }
	float getRestitution() { return fixture->GetRestitution(); }

private:
	// tracking our Box2D object
	b2Fixture* fixture = nullptr;
};
