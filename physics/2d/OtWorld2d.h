//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
//	OtWorld2dClass
//

class OtWorld2dClass;
using OtWorld2d = OtObjectPointer<OtWorld2dClass>;

class OtWorld2dClass : public OtPhysics2DClass {
public:
	// constructor/destructor
	OtWorld2dClass();
	~OtWorld2dClass();

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

	// get world ID
	inline b2WorldId getWorldId() { return worldId; }

	// get type definition
	static OtType getMeta();

private:
	// tracking our Box2D object
	b2WorldId worldId = b2_nullWorldId;

	// tracking time in update loop to ensure constant simulation speed
	static constexpr float secondsPerUpdate = 1.0f / 60.0f;
	bool running = false;
	float delta = 0.0f;

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
};
