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

#include "OtInteger.h"
#include "OtObject.h"
#include "OtReal.h"
#include "OtValue.h"


//
//	OtVec2
//

class OtVec2Class;
using OtVec2 = OtObjectPointer<OtVec2Class>;

class OtVec2Class : public OtObjectClass {
public:
	// constructors
	OtVec2Class() {}
	OtVec2Class(float scalar) : value(scalar) {}
	OtVec2Class(float x, float y) : value(x, y) {}
	OtVec2Class(const glm::vec2& v) : value(v) {}

	// initialize
	void init(size_t count, OtObject* parameters);

	// access members
	inline operator glm::vec2() { return value; }

	inline OtObject setX(float x) { value.x = x; return OtVec2(this); }
	inline OtObject setY(float y) { value.y = y; return OtVec2(this); }

	inline float getX() { return value.x; }
	inline float getY() { return value.y; }

	// vector arithmetic
	inline glm::vec2 add(glm::vec2 operand) { return value + operand; }
	inline glm::vec2 subtract(glm::vec2 operand) { return value - operand; }
	inline glm::vec2 multiply(glm::vec2 operand) { return value * operand; }
	inline glm::vec2 divide(glm::vec2 operand) { return value / operand; }

	inline bool equal(glm::vec2 operand) { return value == operand; }
	inline bool notEqual(glm::vec2 operand) { return value != operand; }

	inline float length() { return glm::length(value); }
	inline glm::vec2 normalize() { return glm::normalize(value); }
	inline float distance(glm::vec2 operand) { return glm::distance(value, operand); }

	inline glm::vec2 lerp(glm::vec2 operand, float delta) {
		if (delta < 0.0f) {
			return value;

		} else if (delta > 1.0f) {
			return operand;

		} else {
			return value + ((operand - value) * delta);
		}
	}

	// get type definition
	static OtType getMeta();

private:
	glm::vec2 value = glm::vec2(1.0f);
};


//
//	Allow glm::vec2 as an ObjectTalk value
//

template <>
struct OtValue<glm::vec2> {
	static inline OtObject encode(glm::vec2 vec2) {
		return OtVec2::create(vec2);
	}

	static inline glm::vec2 decode(OtObject object) {
		if (object.isKindOf<OtVec2Class>()) {
			return OtVec2(object)->operator glm::vec2();

		} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
			return glm::vec2(object->operator float());

		} else {
			OtError("Expected a [Vec2], [Real] or [Integer], not a [{}]", object->getType()->getName());
			return glm::vec2();
		}
	}
};