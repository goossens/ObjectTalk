//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "guimath.h"


//
//	OtVec3
//

class OtVec3Class;
typedef std::shared_ptr<OtVec3Class> OtVec3;

class OtVec3Class : public OtMathClass {
public:
	// constructors
	OtVec3Class() {}
	OtVec3Class(float scalar) : vector(scalar) {}
	OtVec3Class(float x, float y, float z) : vector(x, y, z) {}

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// access members
	operator glm::vec3() { return vector; }

	OtObject setX(float value) { vector.x = value; return shared(); }
	OtObject setY(float value) { vector.y = value; return shared(); }
	OtObject setZ(float value) { vector.z = value; return shared(); }

	float getX() { return vector.x; }
	float getY() { return vector.y; }
	float getZ() { return vector.z; }

	// vector arithmetic
	OtObject add(OtObject object);
	OtObject subtract(OtObject object);
	OtObject multiply(OtObject object);
	OtObject divide(OtObject object);

	// functions
	float length() { return glm::length(vector); }
	OtObject normalize() { return create(glm::normalize(vector)); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtVec3 create();
	static OtVec3 create(glm::vec3 vec3);
	static OtVec3 create(float x, float y, float z);

private:
	glm::vec3 vector = glm::vec3(0.0);
};
