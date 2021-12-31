//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/vm.h"

#include "body.h"
#include "world.h"


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
	return shared();
}


//
//	OtWorldClass::allowSleeping
//

OtObject OtWorldClass::allowSleeping(bool sleeping) {
	world->SetAllowSleeping(sleeping);
	return shared();
}


//
//	OtWorldClass::continuousPhysics
//

OtObject OtWorldClass::continuousPhysics(bool continuous) {
	world->SetContinuousPhysics(continuous);
	return shared();
}


//
//	OtWorldClass::addBeginContactCallback
//

OtObject OtWorldClass::addBeginContactCallback(OtObject callback) {
	beginContactCallback = callback;
	return shared();
}


//
//	OtWorldClass::addEndContactCallback
//

OtObject OtWorldClass::addEndContactCallback(OtObject callback) {
	endContactCallback = callback;
	return shared();
}


//
//	OtWorldClass::addStaticBody
//

OtObject OtWorldClass::addStaticBody() {
	b2BodyDef def;
	def.type = b2_staticBody;
	OtObject body = OtBodyClass::create(world->CreateBody(&def));
	bodies.push_back(body);
	return body;
}


//
//	OtWorldClass::addKinematicBody
//

OtObject OtWorldClass::addKinematicBody() {
	b2BodyDef def;
	def.type = b2_kinematicBody;
	OtObject body = OtBodyClass::create(world->CreateBody(&def));
	bodies.push_back(body);
	return body;
}


//
//	OtWorldClass::addDynamicBody
//

OtObject OtWorldClass::addDynamicBody() {
	b2BodyDef def;
	def.type = b2_dynamicBody;
	OtObject body = OtBodyClass::create(world->CreateBody(&def));
	bodies.push_back(body);
	return body;
}


//
//	OtWorldClass::step
//

static const double secondsPerUpdate = 1/30.0;

void OtWorldClass::step(int32_t deltaMilliseconds) {
	// convert to seconds
	delta += ((double) deltaMilliseconds) / 1000.0;

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


//
//	OtWorldClass::BeginContact
//

void OtWorldClass::BeginContact(b2Contact* contact) {
	if (beginContactCallback) {
		callbacks.push_back(Callback(
			beginContactCallback,
			reinterpret_cast<OtBodyClass*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)->shared(),
			reinterpret_cast<OtBodyClass*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer)->shared()));
	}
}


//
//	OtWorldClass::EndContact
//

void OtWorldClass::EndContact(b2Contact* contact) {
	if (endContactCallback) {
		callbacks.push_back(Callback(
			endContactCallback,
			reinterpret_cast<OtBodyClass*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer)->shared(),
			reinterpret_cast<OtBodyClass*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer)->shared()));
	}
}


//
//	OtWorldClass::getMeta
//

OtType OtWorldClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtWorldClass>("World", OtSimulationClass::getMeta());
		type->set("setGravity", OtFunctionClass::create(&OtWorldClass::setGravity));
		type->set("allowSleeping", OtFunctionClass::create(&OtWorldClass::allowSleeping));
		type->set("continuousPhysics", OtFunctionClass::create(&OtWorldClass::continuousPhysics));

		type->set("addStaticBody", OtFunctionClass::create(&OtWorldClass::addStaticBody));
		type->set("addKinematicBody", OtFunctionClass::create(&OtWorldClass::addKinematicBody));
		type->set("addDynamicBody", OtFunctionClass::create(&OtWorldClass::addDynamicBody));

		type->set("addBeginContactCallback", OtFunctionClass::create(&OtWorldClass::addBeginContactCallback));
		type->set("addEndContactCallback", OtFunctionClass::create(&OtWorldClass::addEndContactCallback));
	}

	return type;
}


//
//	OtWorldClass::create
//

OtWorld OtWorldClass::create() {
	OtWorld world = std::make_shared<OtWorldClass>();
	world->setType(getMeta());
	return world;
}
