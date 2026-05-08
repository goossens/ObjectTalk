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
	world = new b2World(b2Vec2(0.0f, 0.0f));
	world->SetContactListener(this);
}


//
//	OtWorld2dClass::~OtWorld2dClass
//

OtWorld2dClass::~OtWorld2dClass() {
	delete world;
}


//
//	OtWorld2dClass::clear
//

void OtWorld2dClass::clear() {
	bodies.clear();
	callbacks.clear();

	for (auto body = world->GetBodyList(); body; body->GetNext()) {
		world->DestroyBody(body);
	}
}


//
//	OtWorld2dClass::setGravity
//

OtObject OtWorld2dClass::setGravity(float x, float y) {
	world->SetGravity(b2Vec2(x, y));
	return OtObject(this);
}


//
//	OtWorld2dClass::allowSleeping
//

OtObject OtWorld2dClass::allowSleeping(bool sleeping) {
	world->SetAllowSleeping(sleeping);
	return OtObject(this);
}


//
//	OtWorld2dClass::continuousPhysics
//

OtObject OtWorld2dClass::continuousPhysics(bool continuous) {
	world->SetContinuousPhysics(continuous);
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
//	OtWorld2dClass::BeginContact
//

void OtWorld2dClass::BeginContact(b2Contact* contact) {
	if (beginContactCallback) {
		callbacks.push_back(Callback(
			beginContactCallback,
			OtBody(reinterpret_cast<OtBodyClass*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)),
			OtBody(reinterpret_cast<OtBodyClass*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer))));
	}
}


//
//	OtWorld2dClass::EndContact
//

void OtWorld2dClass::EndContact(b2Contact* contact) {
	if (endContactCallback) {
		callbacks.push_back(Callback(
			endContactCallback,
			OtBody(reinterpret_cast<OtBodyClass*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)),
			OtBody(reinterpret_cast<OtBodyClass*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer))));
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
