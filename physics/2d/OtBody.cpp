//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtReal.h"

#include "OtBody.h"
#include "OtBodyShape.h"


//
//	OtBodyClass::~OtBodyClass
//

OtBodyClass::~OtBodyClass() {
	if (B2_IS_NON_NULL(bodyId)) {
		b2DestroyBody(bodyId);
		bodyId = b2_nullBodyId;
	}
}


//
//	OtBodyClass::setPosition
//

OtObject OtBodyClass::setPosition(float x, float y) {
	b2Body_SetTransform(bodyId, b2Vec2(x, y), b2Body_GetRotation(bodyId));
	return OtBody(this);
}


//
//	OtBodyClass::setLinearVelocity
//

OtObject OtBodyClass::setLinearVelocity(float x, float y) {
	b2Body_SetLinearVelocity(bodyId, b2Vec2(x, y));
	return OtBody(this);
}


//
//	OtBodyClass::setAngle
//

OtObject OtBodyClass::setAngle(float angle) {
	b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), b2MakeRot(angle));
	return OtBody(this);
}


//
//	OtBodyClass::enable
//

OtObject OtBodyClass::enable() {
	b2Body_Enable(bodyId);
	return OtBody(this);
}


//
//	OtBodyClass::disable
//

OtObject OtBodyClass::disable() {
	b2Body_Disable(bodyId);
	return OtBody(this);
}


//
//	OtBodyClass::addCircularShape
//

OtObject OtBodyClass::addCircularShape(float x, float y, float radius) {
	b2Circle circle(b2Vec2{x, y}, radius);
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.enableContactEvents = true;
	b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
	return OtBodyShape::create(shapeId);
}


//
//	OtBodyClass::addRectangularShape
//

OtObject OtBodyClass::addRectangularShape(float x, float y, float w, float h) {
	b2Polygon box = b2MakeOffsetBox(w / 2.0f, h / 2.0f, b2Vec2(x, y), b2MakeRot(0.0f));
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.enableContactEvents = true;
	b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
	return OtBodyShape::create(shapeId);
}


//
//	OtBodyClass::get
//

OtObject OtBodyClass::get(OtID id) {
	if (id == xID) {
		return OtReal::create(b2Body_GetPosition(bodyId).x);

	} else if (id == yID) {
		return OtReal::create(b2Body_GetPosition(bodyId).y);

	} else if (id == vxID) {
		return OtReal::create(b2Body_GetLinearVelocity(bodyId).x);

	} else if (id == vyID) {
		return OtReal::create(b2Body_GetLinearVelocity(bodyId).y);

	} else {
		return OtPhysics2DClass::get(id);
	}
}


//
//	OtBodyClass::applyLinearImpulse
//

void OtBodyClass::applyLinearImpulse(float x, float y) {
	b2Body_ApplyLinearImpulseToCenter(bodyId, b2Vec2(x, y), true);
}


//
//	OtBodyClass::getMeta
//

OtType OtBodyClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtBodyClass>("Body", OtPhysics2DClass::getMeta());
		type->set("setPosition", OtFunction::create(&OtBodyClass::setPosition));
		type->set("setLinearVelocity", OtFunction::create(&OtBodyClass::setLinearVelocity));
		type->set("setAngle", OtFunction::create(&OtBodyClass::setAngle));

		type->set("enable", OtFunction::create(&OtBodyClass::enable));
		type->set("disable", OtFunction::create(&OtBodyClass::disable));

		type->set("addCircularShape", OtFunction::create(&OtBodyClass::addCircularShape));
		type->set("addRectangularShape", OtFunction::create(&OtBodyClass::addRectangularShape));

		type->set("applyLinearImpulse", OtFunction::create(&OtBodyClass::applyLinearImpulse));
	}

	return type;
}
