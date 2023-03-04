//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCallback.h"
#include "OtFunction.h"
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
//	OtWorldClass::addStaticBody
//

OtObject OtWorldClass::addStaticBody(size_t count, OtObject* parameters) {
	// sanity check
	if (count > 1) {
		OtExcept("[addStaticBody] expect up to 1 parameter, not %ld", count);
	}

	b2BodyDef def;
	def.type = b2_staticBody;
	b2Body* b = world->CreateBody(&def);
	OtBody body;

	if (count) {
		parameters[0]->expectKindOf("Body");
		body = OtBody(parameters[0]);
		body->setBody(b);

	} else {
		body = OtBody::create(b);
	}

	bodies.push_back(body);
	return body;
}


//
//	OtWorldClass::addKinematicBody
//

OtObject OtWorldClass::addKinematicBody(size_t count, OtObject* parameters) {
	// sanity check
	if (count > 1) {
		OtExcept("[addKinematicBody] expect up to 1 parameter, not %ld", count);
	}

	b2BodyDef def;
	def.type = b2_kinematicBody;
	b2Body* b = world->CreateBody(&def);
	OtBody body;

	if (count) {
		parameters[0]->expectKindOf("Body");
		body = OtBody(parameters[0]);
		body->setBody(b);

	} else {
		body = OtBody::create(b);
	}

	bodies.push_back(body);
	return body;
}


//
//	OtWorldClass::addDynamicBody
//

OtObject OtWorldClass::addDynamicBody(size_t count, OtObject* parameters) {
	// sanity check
	if (count > 1) {
		OtExcept("[addDynamicBody] expect up to 1 parameter, not %ld", count);
	}

	b2BodyDef def;
	def.type = b2_dynamicBody;
	b2Body* b = world->CreateBody(&def);
	OtBody body;

	if (count) {
		parameters[0]->expectKindOf("Body");
		body = OtBody(parameters[0]);
		body->setBody(b);

	} else {
		body = OtBody::create(b);
	}

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
			OtVM::instance()->callMemberFunction(callback.callback, "__call__", callback.body1, callback.body2);
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
		type = OtType::create<OtWorldClass>("World", OtSimulationClass::getMeta());
		type->set("setGravity", OtFunction::create(&OtWorldClass::setGravity));
		type->set("allowSleeping", OtFunction::create(&OtWorldClass::allowSleeping));
		type->set("continuousPhysics", OtFunction::create(&OtWorldClass::continuousPhysics));

		type->set("addStaticBody", OtFunction::create(&OtWorldClass::addStaticBody));
		type->set("addKinematicBody", OtFunction::create(&OtWorldClass::addKinematicBody));
		type->set("addDynamicBody", OtFunction::create(&OtWorldClass::addDynamicBody));

		type->set("addBeginContactCallback", OtFunction::create(&OtWorldClass::addBeginContactCallback));
		type->set("addEndContactCallback", OtFunction::create(&OtWorldClass::addEndContactCallback));
	}

	return type;
}
