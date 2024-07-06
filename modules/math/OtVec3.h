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
//	OtVec3
//

class OtVec3Class;
using OtVec3 = OtObjectPointer<OtVec3Class>;

class OtVec3Class : public OtObjectClass {
public:
	// constructors
	OtVec3Class() {}
	OtVec3Class(float scalar) : vec3(scalar) {}
	OtVec3Class(float x, float y, float z) : vec3(x, y, z) {}
	OtVec3Class(const glm::vec3& v3) : vec3(v3) {}

	// initialize
	void init(size_t count, OtObject* parameters);

	// access members
	operator glm::vec3() { return vec3; }

	OtObject setX(float value) { vec3.x = value; return OtVec3(this); }
	OtObject setY(float value) { vec3.y = value; return OtVec3(this); }
	OtObject setZ(float value) { vec3.z = value; return OtVec3(this); }

	float getX() { return vec3.x; }
	float getY() { return vec3.y; }
	float getZ() { return vec3.z; }

	// vector arithmetic
	OtObject add(OtObject object);
	OtObject subtract(OtObject object);
	OtObject multiply(OtObject object);
	OtObject divide(OtObject object);

	float length();
	OtObject normalize();
	float distance(OtObject object);
	float dot(OtObject object);
	OtObject cross(OtObject object);
	OtObject lerp(OtObject object, float delta);

	// get type definition
	static OtType getMeta();

private:
	glm::vec3 vec3 = glm::vec3(1.0f);
};
