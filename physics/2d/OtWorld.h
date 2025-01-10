//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "box2d/box2d.h"

#include "OtPhysics2D.h"


//
//	OtWorldClass
//

class OtWorldClass;
using OtWorld = OtObjectPointer<OtWorldClass>;

class OtWorldClass : public OtPhysics2DClass, public b2ContactListener {
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

	// start/stop simulation
	OtObject start();
	OtObject stop();
	inline bool isRunning() { return running; }

	// run simulation
	void step();

	// get type definition
	static OtType getMeta();

private:
	// so they can access the "world" member
	friend class OtDynamicBodyClass;
	friend class OtKinematicBodyClass;
	friend class OtStaticBodyClass;

	// tracking our Box2D object
	b2World* world = nullptr;

	// tracking time in update loop to ensure constant simulation speed
	static constexpr double secondsPerUpdate = 1 / 30.0;
	bool running = false;
	double delta = 0.0;

	// list of our bodies
	std::vector<OtObject> bodies;

	// callback management
	struct Callback {
		inline Callback(OtObject c, OtObject b1, OtObject b2) : callback(c), body1(b1), body2(b2) {}
		OtObject callback;
		OtObject body1;
		OtObject body2;
	};

	std::vector<Callback> callbacks;
	OtObject beginContactCallback;
	OtObject endContactCallback;

	// handle physics events
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
};
