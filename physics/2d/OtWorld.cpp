//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
#include "OtWorld.h"


//
//	OtWorldClass::OtWorldClass
//

OtWorldClass::OtWorldClass() {
	world = new b2World(b2Vec2(0.0, 0.0));
	world->SetContactListener(this);
}


//
//	OtWorldClass::~OtWorldClass
//

OtWorldClass::~OtWorldClass() {
	delete world;
}


//
//	OtWorldClass::clear
//

void OtWorldClass::clear() {
	bodies.clear();
	callbacks.clear();

	for (auto body = world->GetBodyList(); body; body->GetNext()) {
		world->DestroyBody(body);
	}
}


//
//	OtWorldClass::setGravity
//

OtObject OtWorldClass::setGravity(float x, float y) {
	world->SetGravity(b2Vec2(x, y));
	return OtObject(this);
}


//
//	OtWorldClass::allowSleeping
//

OtObject OtWorldClass::allowSleeping(bool sleeping) {
	world->SetAllowSleeping(sleeping);
	return OtObject(this);
}


//
//	OtWorldClass::continuousPhysics
//

OtObject OtWorldClass::continuousPhysics(bool continuous) {
	world->SetContinuousPhysics(continuous);
	return OtObject(this);
}


//
//	OtWorldClass::addBeginContactCallback
//

OtObject OtWorldClass::addBeginContactCallback(OtObject callback) {
	OtCallbackValidate(callback, 2);
	beginContactCallback = callback;
	return OtObject(this);
}


//
//	OtWorldClass::addEndContactCallback
//

OtObject OtWorldClass::addEndContactCallback(OtObject callback) {
	OtCallbackValidate(callback, 2);
	endContactCallback = callback;
	return OtObject(this);
}


//
//	OtWorldClass::start
//

OtObject OtWorldClass::start() {
	running = true;
	delta = 0.0;
	return OtObject();
}


//
//	OtWorldClass::stop
//

OtObject OtWorldClass::stop() {
	running = false;
	return OtObject();
}


//
//	OtWorldClass::step
//

void OtWorldClass::step() {
	// ensure we are running
	if (running) {
		// calculate time since last update
		delta += double(ImGui::GetIO().DeltaTime);

		// run step if required
		if (delta > secondsPerUpdate) {
			delta -= secondsPerUpdate;
			world->Step(secondsPerUpdate, 8, 1);

			for (auto& callback : callbacks) {
				OtVM::callMemberFunction(callback.callback, "__call__", callback.body1, callback.body2);
			}

			callbacks.clear();
		}
	}
}


//
//	OtWorldClass::BeginContact
//

void OtWorldClass::BeginContact(b2Contact* contact) {
	if (beginContactCallback) {
		callbacks.push_back(Callback(
			beginContactCallback,
			OtBody(reinterpret_cast<OtBodyClass*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)),
			OtBody(reinterpret_cast<OtBodyClass*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer))));
	}
}


//
//	OtWorldClass::EndContact
//

void OtWorldClass::EndContact(b2Contact* contact) {
	if (endContactCallback) {
		callbacks.push_back(Callback(
			endContactCallback,
			OtBody(reinterpret_cast<OtBodyClass*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)),
			OtBody(reinterpret_cast<OtBodyClass*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer))));
	}
}


//
//	OtWorldClass::getMeta
//

OtType OtWorldClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtWorldClass>("World", OtPhysics2DClass::getMeta());
		type->set("setGravity", OtFunction::create(&OtWorldClass::setGravity));
		type->set("allowSleeping", OtFunction::create(&OtWorldClass::allowSleeping));
		type->set("continuousPhysics", OtFunction::create(&OtWorldClass::continuousPhysics));

		type->set("addBeginContactCallback", OtFunction::create(&OtWorldClass::addBeginContactCallback));
		type->set("addEndContactCallback", OtFunction::create(&OtWorldClass::addEndContactCallback));

		type->set("start", OtFunction::create(&OtWorldClass::start));
		type->set("stop", OtFunction::create(&OtWorldClass::stop));
		type->set("step", OtFunction::create(&OtWorldClass::step));
	}

	return type;
}
