//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtCallback.h"
#include "OtFunction.h"
#include "OtLog.h"
#include "OtVM.h"

#include "OtBody.h"
#include "OtWorld2d.h"


//
//	OtWorld2dClass::OtWorld2dClass
//

OtWorld2dClass::OtWorld2dClass() {
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = b2Vec2{0.0f, -0.0f};
	 worldId = b2CreateWorld(&worldDef);
}


//
//	OtWorld2dClass::~OtWorld2dClass
//

OtWorld2dClass::~OtWorld2dClass() {
	b2DestroyWorld(worldId);
	worldId = b2_nullWorldId;
}


//
//	OtWorld2dClass::clear
//

void OtWorld2dClass::clear() {
	callbacks.clear();
}


//
//	OtWorld2dClass::setGravity
//

OtObject OtWorld2dClass::setGravity(float x, float y) {
	b2World_SetGravity(worldId, b2Vec2{x, y});
	return OtObject(this);
}


//
//	OtWorld2dClass::allowSleeping
//

OtObject OtWorld2dClass::allowSleeping(bool sleeping) {
	b2World_EnableSleeping(worldId, sleeping);
	return OtObject(this);
}


//
//	OtWorld2dClass::continuousPhysics
//

OtObject OtWorld2dClass::continuousPhysics(bool continuous) {
	b2World_EnableContinuous(worldId, continuous);
	return OtObject(this);
}


//
//	OtWorld2dClass::addBeginContactCallback
//

OtObject OtWorld2dClass::addBeginContactCallback(OtObject callback) {
	OtCallbackValidate(callback, 2);
	beginContactCallback = callback;
	return OtObject(this);
}


//
//	OtWorld2dClass::addEndContactCallback
//

OtObject OtWorld2dClass::addEndContactCallback(OtObject callback) {
	OtCallbackValidate(callback, 2);
	endContactCallback = callback;
	return OtObject(this);
}


//
//	OtWorld2dClass::start
//

OtObject OtWorld2dClass::start() {
	running = true;
	delta = 0.0f;
	return OtObject();
}


//
//	OtWorld2dClass::stop
//

OtObject OtWorld2dClass::stop() {
	running = false;
	return OtObject();
}


//
//	OtWorld2dClass::step
//

void OtWorld2dClass::step() {
	// ensure we are running
	if (running) {
		// calculate time since last update
		delta += ImGui::GetIO().DeltaTime;

		// run step if required
		if (delta > secondsPerUpdate) {
			delta -= secondsPerUpdate;
			b2World_Step(worldId, secondsPerUpdate, 4);

			b2ContactEvents contactEvents = b2World_GetContactEvents(worldId);

			if (beginContactCallback) {
				for (auto c = 0 ; c < contactEvents.beginCount; c++) {
					auto& event = contactEvents.beginEvents[c];
					auto b1 = OtBody(reinterpret_cast<OtBodyClass*>(b2Body_GetUserData(b2Shape_GetBody(event.shapeIdA))));
					auto b2 = OtBody(reinterpret_cast<OtBodyClass*>(b2Body_GetUserData(b2Shape_GetBody(event.shapeIdB))));
					OtVM::callMemberFunction(beginContactCallback, "__call__", b1, b2);
				}
			}

			if (endContactCallback) {
				for (auto c = 0 ; c < contactEvents.endCount; c++) {
					auto& event = contactEvents.endEvents[c];
					auto b1 = OtBody(reinterpret_cast<OtBodyClass*>(b2Body_GetUserData(b2Shape_GetBody(event.shapeIdA))));
					auto b2 = OtBody(reinterpret_cast<OtBodyClass*>(b2Body_GetUserData(b2Shape_GetBody(event.shapeIdB))));
					OtVM::callMemberFunction(beginContactCallback, "__call__", b1, b2);
				}
			}
		}
	}
}


//
//	OtWorld2dClass::getMeta
//

OtType OtWorld2dClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtWorld2dClass>("World", OtPhysics2DClass::getMeta());
		type->set("setGravity", OtFunction::create(&OtWorld2dClass::setGravity));
		type->set("allowSleeping", OtFunction::create(&OtWorld2dClass::allowSleeping));
		type->set("continuousPhysics", OtFunction::create(&OtWorld2dClass::continuousPhysics));

		type->set("addBeginContactCallback", OtFunction::create(&OtWorld2dClass::addBeginContactCallback));
		type->set("addEndContactCallback", OtFunction::create(&OtWorld2dClass::addEndContactCallback));

		type->set("start", OtFunction::create(&OtWorld2dClass::start));
		type->set("stop", OtFunction::create(&OtWorld2dClass::stop));
		type->set("step", OtFunction::create(&OtWorld2dClass::step));
	}

	return type;
}
