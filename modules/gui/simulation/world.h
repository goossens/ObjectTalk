//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "box2d/box2d.h"

#include "simulation.h"


//
//	OtWorldClass
//

class OtWorldClass;
typedef std::shared_ptr<OtWorldClass> OtWorld;

class OtWorldClass : public OtSimulationClass, public b2ContactListener {
public:
	// constructor/destructor
	OtWorldClass();
	~OtWorldClass();

	// clear all content
	void clear();

	// update world state
	OtObject setGravity(float x, float y);
	OtObject allowSleeping(bool sleeping);
	OtObject continuousPhysics(bool continuous);

	// add callbacks
	OtObject addBeginContactCallback(OtObject callback);
	OtObject addEndContactCallback(OtObject callback);

	// create a new body
	OtObject addStaticBody();
	OtObject addKinematicBody();
	OtObject addDynamicBody();

	// run simulation
	void step(int32_t deltaMilliseconds);

	// handle simulation events
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWorld create();

private:
	// tracking our Box2D object
	b2World* world = nullptr;

	// tracking time in update loop to ensure constant simulation speed
	double delta = 0.0;

	// list of our bodies
	std::vector<OtObject> bodies;

	// callback management
	struct Callback {
		Callback(OtObject c, OtObject b1, OtObject b2) : callback(c), body1(b1), body2(b2) {}
		OtObject callback;
		OtObject body1;
		OtObject body2;
	};

	std::vector<Callback> callbacks;
	OtObject beginContactCallback;
	OtObject endContactCallback;
};
