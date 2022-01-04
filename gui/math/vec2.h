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
//	OtVec2
//

class OtVec2Class;
typedef std::shared_ptr<OtVec2Class> OtVec2;

class OtVec2Class : public OtMathClass {
public:
	// constructors
	OtVec2Class() {}
	OtVec2Class(float scalar) : vector(scalar) {}
	OtVec2Class(float x, float y) : vector(x, y) {}

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// access members
	operator glm::vec2() { return vector; }

	OtObject setX(float value) { vector.x = value; return shared(); }
	OtObject setY(float value) { vector.y = value; return shared(); }

	float getX() { return vector.x; }
	float getY() { return vector.y; }

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
	static OtVec2 create();
	static OtVec2 create(glm::vec2 vec2);
	static OtVec2 create(float x, float y);

private:
	glm::vec2 vector = glm::vec2(0.0);
};
