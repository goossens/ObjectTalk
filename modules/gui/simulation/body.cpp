//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "body.h"
#include "fixture.h"


//
//	OtBodyClass::clear
//

void OtBodyClass::clear() {
	fixtures.clear();

	for (auto fixture = body->GetFixtureList(); fixture; fixture->GetNext()) {
		body->DestroyFixture(fixture);
	}
}


//
//	OtBodyClass::setPosition
//

OtObject OtBodyClass::setPosition(float x, float y) {
	body->SetTransform(b2Vec2(x, y), body->GetAngle());
	return shared();
}


//
//	OtBodyClass::setLinearVelocity
//

OtObject OtBodyClass::setLinearVelocity(float x, float y) {
	body->SetLinearVelocity(b2Vec2(x, y));
	return shared();
}


//
//	OtBodyClass::setAngle
//

OtObject OtBodyClass::setAngle(float angle) {
	body->SetTransform(body->GetPosition(), angle);
	return shared();
}


//
//	OtBodyClass::enable
//

OtObject OtBodyClass::enable() {
	body->SetEnabled(true);
	return shared();
}


//
//	OtBodyClass::disable
//

OtObject OtBodyClass::disable() {
	body->SetEnabled(false);
	return shared();
}


//
//	OtBodyClass::addCircularFixture
//

OtObject OtBodyClass::addCircularFixture(float x, float y, float radius) {
	b2CircleShape circle;
	circle.m_p = b2Vec2(x, y);
	circle.m_radius = radius;
	OtObject fixture = OtFixtureClass::create(body->CreateFixture(&circle, 0.0));
	fixtures.push_back(fixture);
	return fixture;
}


//
//	OtBodyClass::addEdgeFixture
//

OtObject OtBodyClass::addEdgeFixture(float x1, float y1, float x2, float y2) {
	b2EdgeShape edge;
	edge.SetTwoSided(b2Vec2(x1, y1), b2Vec2(x2, y2));
	OtObject fixture = OtFixtureClass::create(body->CreateFixture(&edge, 0.0));
	fixtures.push_back(fixture);
	return fixture;
}


//
//	OtBodyClass::addRectangularFixture
//

OtObject OtBodyClass::addRectangularFixture(float x, float y, float w, float h) {
	b2PolygonShape poly;
	poly.SetAsBox(w / 2.0, h / 2.0, b2Vec2(x, y), 0.0);
	OtObject fixture = OtFixtureClass::create(body->CreateFixture(&poly, 0.0));
	fixtures.push_back(fixture);
	return fixture;
}


//
//	OtBodyClass::applyLinearImpulse
//

void OtBodyClass::applyLinearImpulse(float x, float y) {
	body->ApplyLinearImpulseToCenter(b2Vec2(x, y), true);
}


//
//	OtBodyClass::getMeta
//

OtType OtBodyClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtBodyClass>("Body", OtGuiClass::getMeta());
		type->set("setPosition", OtFunctionClass::create(&OtBodyClass::setPosition));
		type->set("setLinearVelocity", OtFunctionClass::create(&OtBodyClass::setLinearVelocity));
		type->set("setAngle", OtFunctionClass::create(&OtBodyClass::setAngle));

		type->set("enable", OtFunctionClass::create(&OtBodyClass::enable));
		type->set("disable", OtFunctionClass::create(&OtBodyClass::disable));

		type->set("addCircularFixture", OtFunctionClass::create(&OtBodyClass::addCircularFixture));
		type->set("addEdgeFixture", OtFunctionClass::create(&OtBodyClass::addEdgeFixture));
		type->set("addRectangularFixture", OtFunctionClass::create(&OtBodyClass::addRectangularFixture));

		type->set("getX", OtFunctionClass::create(&OtBodyClass::getX));
		type->set("getY", OtFunctionClass::create(&OtBodyClass::getY));
		type->set("getLinearVelocityX", OtFunctionClass::create(&OtBodyClass::getLinearVelocityX));
		type->set("getLinearVelocityY", OtFunctionClass::create(&OtBodyClass::getLinearVelocityY));

		type->set("applyLinearImpulse", OtFunctionClass::create(&OtBodyClass::applyLinearImpulse));
	}

	return type;
}


//
//	OtBodyClass::create
//

OtBody OtBodyClass::create() {
	OtBody body = std::make_shared<OtBodyClass>();
	body->setType(getMeta());
	return body;
}


OtBody OtBodyClass::create(b2Body* b) {
	OtBody body = create();
	body->body = b;
	b->GetUserData().pointer = reinterpret_cast<uintptr_t>(body.get());
	return body;
}
