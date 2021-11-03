//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "guimath.h"


//
//	OtVector3
//

class OtVector3Class;
typedef std::shared_ptr<OtVector3Class> OtVector3;

class OtVector3Class : public OtMathClass {
public:
	// constructors
	OtVector3Class() {}
	OtVector3Class(float scalar) : vec3(scalar) {}
	OtVector3Class(float x, float y, float z) : vec3(x, y, z) {}

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// access members
	operator glm::vec3() { return vec3; }

	OtObject setX(float value) { vec3.x = value; return shared(); }
	OtObject setY(float value) { vec3.y = value; return shared(); }
	OtObject setZ(float value) { vec3.z = value; return shared(); }

	float getX() { return vec3.x; }
	float getY() { return vec3.y; }
	float getZ() { return vec3.z; }

	// vector arithmetic
	OtObject add(OtObject object);
	OtObject subtract(OtObject object);
	OtObject multiply(OtObject object);
	OtObject divide(OtObject object);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtVector3 create();
	static OtVector3 create(glm::vec3 vec3);
	static OtVector3 create(float x, float y, float z);

private:
	glm::vec3 vec3 = glm::vec3(1.0);
};
