//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "box2d/box2d.h"

#include "OtPhysics2D.h"


//
//	OtBodyShapeClass
//

class OtBodyShapeClass;
using OtBodyShape = OtObjectPointer<OtBodyShapeClass>;

class OtBodyShapeClass : public OtPhysics2DClass {
public:
	// constructors
	OtBodyShapeClass() = default;
	inline OtBodyShapeClass(b2ShapeId shape) : shape(shape) {}

	// get type definition
	static OtType getMeta();

	// access properties
	OtObject enableContextEvents(bool flag);
	OtObject setDensity(float density);
	OtObject setFriction(float friction);
	OtObject setRestitution(float restitution);

	inline float areContextEventsEnabled() { return b2Shape_AreContactEventsEnabled(shape); }
	inline float getDensity() { return b2Shape_GetDensity(shape); }
	inline float getFriction() { return b2Shape_GetFriction(shape); }
	inline float getRestitution() { return b2Shape_GetRestitution(shape); }

private:
	// tracking our Box2D shape
	b2ShapeId shape = b2_nullShapeId;
};
