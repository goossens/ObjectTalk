//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "box2d/box2d.h"

#include "OtIdentifier.h"

#include "OtPhysics2D.h"


//
//	OtBodyClass
//

class OtBodyClass;
using OtBody = OtObjectPointer<OtBodyClass>;

class OtBodyClass : public OtPhysics2DClass {
public:
	// destructor
	~OtBodyClass();

	// update position, velocity and angle
	OtObject setPosition(float x, float y);
	OtObject setLinearVelocity(float x, float y);
	OtObject setAngle(float angle);

	// set body state
	OtObject enable();
	OtObject disable();

	// add different fixture types
	OtObject addCircularShape(float x, float y, float radius);
	OtObject addRectangularShape(float x, float y, float w, float h);

	// get information
	OtObject get(OtID id) override;

	inline float getX() { return b2Body_GetPosition(bodyId).x; }
	inline float getY() { return b2Body_GetPosition(bodyId).y; }
	inline float getLinearVelocityX() { return b2Body_GetLinearVelocity(bodyId).x; }
	inline float getLinearVelocityY() { return b2Body_GetLinearVelocity(bodyId).y; }

	// manipulate body
	void applyLinearImpulse(float x, float y);

	// get type definition
	static OtType getMeta();

protected:
	// tracking our Box2D body
	b2BodyId bodyId = b2_nullBodyId;

private:
	inline static OtID xID = OtIdentifier::create("x");
	inline static OtID yID = OtIdentifier::create("y");
	inline static OtID vxID = OtIdentifier::create("vx");
	inline static OtID vyID = OtIdentifier::create("vy");
};
