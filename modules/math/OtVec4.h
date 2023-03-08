//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtVec4
//

class OtVec4Class;
using OtVec4 = OtObjectPointer<OtVec4Class>;

class OtVec4Class : public OtObjectClass {
public:
	// constructors
	OtVec4Class() {}
	OtVec4Class(float scalar) : vec4(scalar) {}
	OtVec4Class(float x, float y, float z, float w) : vec4(x, y, z, w) {}
	OtVec4Class(const glm::vec4& v3) : vec4(v3) {}

	// initialize
	void init(size_t count, OtObject* parameters);

	// access members
	operator glm::vec4() { return vec4; }

	OtObject setX(float value) { vec4.x = value; return OtObject(this); }
	OtObject setY(float value) { vec4.y = value; return OtObject(this); }
	OtObject setZ(float value) { vec4.z = value; return OtObject(this); }
	OtObject setW(float value) { vec4.w = value; return OtObject(this); }

	float getX() { return vec4.x; }
	float getY() { return vec4.y; }
	float getZ() { return vec4.z; }
	float getW() { return vec4.w; }

	// vector arithmetic
	OtObject add(OtObject object);
	OtObject subtract(OtObject object);
	OtObject multiply(OtObject object);
	OtObject divide(OtObject object);

	float length();
	OtObject normalize();
	OtObject homogonize();
	OtObject lerp(OtObject object, float delta);

	// get type definition
	static OtType getMeta();

private:
	glm::vec4 vec4 = glm::vec4(1.0f);
};
