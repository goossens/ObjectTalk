//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "box2d/box2d.h"

#include "OtGui.h"


//
//	OtBodyClass
//

class OtBodyClass;
typedef std::shared_ptr<OtBodyClass> OtBody;

class OtBodyClass : public OtGuiClass {
public:
	// clear all content
	void clear();

	// update position, velocity and angle
	OtObject setPosition(float x, float y);
	OtObject setLinearVelocity(float x, float y);
	OtObject setAngle(float angle);

	// set body state
	OtObject enable();
	OtObject disable();

	// add different fixture types
	OtObject addCircularFixture(float x, float y, float radius);
	OtObject addEdgeFixture(float x1, float y1, float x2, float y2);
	OtObject addRectangularFixture(float x, float y, float w, float h);

	// get information
	float getX() { return body->GetPosition().x; }
	float getY() { return body->GetPosition().y; }
	float getLinearVelocityX() { return body->GetLinearVelocity().x; }
	float getLinearVelocityY() { return body->GetLinearVelocity().y; }

	// manipulate body
	void applyLinearImpulse(float x, float y);

	// specify a new body
	void setBody(b2Body* body);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBody create();
	static OtBody create(b2Body* body);

private:
	// tracking our Box2D object
	b2Body* body = nullptr;

	// list of our fixtures
	std::vector<OtObject> fixtures;
};
