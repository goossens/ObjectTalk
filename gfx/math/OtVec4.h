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
//	OtVec4
//

class OtVec4Class;
using OtVec4 = OtObjectPointer<OtVec4Class>;

class OtVec4Class : public OtObjectClass {
public:
	// constructors
	OtVec4Class() {}
	OtVec4Class(float scalar) : value(scalar) {}
	OtVec4Class(float x, float y, float z, float w) : value(x, y, z, w) {}
	OtVec4Class(const glm::vec4& v) : value(v) {}

	// initialize
	void init(size_t count, OtObject* parameters);

	// access members
	inline operator glm::vec4() { return value; }

	inline OtObject setX(float x) { value.x = x; return OtVec4(this); }
	inline OtObject setY(float y) { value.y = y; return OtVec4(this); }
	inline OtObject setZ(float z) { value.z = z; return OtVec4(this); }
	inline OtObject setW(float w) { value.w = w; return OtVec4(this); }

	inline float getX() { return value.x; }
	inline float getY() { return value.y; }
	inline float getZ() { return value.z; }
	inline float getW() { return value.w; }

	// vector arithmetic
	inline glm::vec4 add(glm::vec4 operand) { return value + operand; }
	inline glm::vec4 subtract(glm::vec4 operand) { return value - operand; }
	inline glm::vec4 multiply(glm::vec4 operand) { return value * operand; }
	inline glm::vec4 divide(glm::vec4 operand) { return value / operand; }

	inline bool equal(glm::vec4 operand) { return value == operand; }
	inline bool notEqual(glm::vec4 operand) { return value != operand; }

	inline float length() { return glm::length(value); }
	inline glm::vec4 normalize() { return glm::normalize(value); }
	inline glm::vec4 homogonize() { return glm::vec4(value.x / value.w, value.y / value.w, value.z / value.w, 1.0f); }

	inline glm::vec4 lerp(glm::vec4 operand, float delta) {
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
	glm::vec4 value = glm::vec4(1.0f);
};


//
//	Allow glm::vec4 as an ObjectTalk value
//

template <>
struct OtValue<glm::vec4> {
	static inline OtObject encode(glm::vec4 vec4) {
		return OtVec4::create(vec4);
	}

	static inline glm::vec4 decode(OtObject object) {
		if (object.isKindOf<OtVec4Class>()) {
			return OtVec4(object)->operator glm::vec4();

		} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
			return glm::vec4(object->operator float());

		} else {
			OtError("Expected a [Vec4], [Real] or [Integer], not a [{}]", object->getType()->getName());
			return glm::vec4();
		}
	}
};
