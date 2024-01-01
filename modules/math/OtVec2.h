//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtObject.h"


//
//	OtVec2
//

class OtVec2Class;
using OtVec2 = OtObjectPointer<OtVec2Class>;

class OtVec2Class : public OtObjectClass {
public:
	// constructors
	OtVec2Class() {}
	OtVec2Class(float scalar) : vec2(scalar) {}
	OtVec2Class(float x, float y) : vec2(x, y) {}
	OtVec2Class(const glm::vec2& v2) : vec2(v2) {}

	// initialize
	void init(size_t count, OtObject* parameters);

	// access members
	operator glm::vec2() { return vec2; }

	OtObject setX(float value) { vec2.x = value; return OtObject(this); }
	OtObject setY(float value) { vec2.y = value; return OtObject(this); }

	float getX() { return vec2.x; }
	float getY() { return vec2.y; }

	// vector arithmetic
	OtObject add(OtObject object);
	OtObject subtract(OtObject object);
	OtObject multiply(OtObject object);
	OtObject divide(OtObject object);

	float length();
	OtObject normalize();
	float distance(OtObject object);
	OtObject lerp(OtObject object, float delta);

	// get type definition
	static OtType getMeta();

private:
	glm::vec2 vec2 = glm::vec2(1.0f);
};
