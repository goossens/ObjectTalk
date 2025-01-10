//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "box2d/box2d.h"

#include "OtPhysics2D.h"


//
//	OtFixtureClass
//

class OtFixtureClass;
using OtFixture = OtObjectPointer<OtFixtureClass>;

class OtFixtureClass : public OtPhysics2DClass {
public:
	// constructors
	OtFixtureClass() = default;
	inline OtFixtureClass(b2Fixture* f) : fixture(f) {}

	// get type definition
	static OtType getMeta();

	// access properties
	OtObject setDensity(float density);
	OtObject setFriction(float friction);
	OtObject setRestitution(float restitution);

	inline float getDensity() { return fixture->GetDensity(); }
	inline float getFriction() { return fixture->GetFriction(); }
	inline float getRestitution() { return fixture->GetRestitution(); }

private:
	// tracking our Box2D object
	b2Fixture* fixture = nullptr;
};
