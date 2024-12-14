//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "fmt/format.h"
#include "glm/glm.hpp"

#include "OtIdentifier.h"
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

	// debugging support
	inline std::string describe() override { return fmt::format("Vec2({}, {})", value.x, value.y); }
	inline operator std::string() override { return describe(); }

	// access members
	inline operator glm::vec2() { return value; }

	OtObject set(OtID id, OtObject value) override;
	OtObject get(OtID id) override;

	inline OtObject setX(float x) { value.x = x; return OtVec2(this); }
	inline OtObject setY(float y) { value.y = y; return OtVec2(this); }

	inline float getX() { return value.x; }
	inline float getY() { return value.y; }

	// vector arithmetic
	inline glm::vec2 add(glm::vec2 operand) { return value + operand; }
	inline glm::vec2 subtract(glm::vec2 operand) { return value - operand; }
	inline glm::vec2 multiply(glm::vec2 operand) { return value * operand; }
	inline glm::vec2 divide(glm::vec2 operand) { return value / operand; }
	inline glm::vec2 modulo(glm::vec2 operand) { return glm::mod(value, operand); }

	inline bool equal(glm::vec2 operand) { return value == operand; }
	inline bool notEqual(glm::vec2 operand) { return value != operand; }

	inline float length() { return glm::length(value); }
	inline float distance(glm::vec2 operand) { return glm::distance(value, operand); }

	inline glm::vec2 min(glm::vec2 operand) { return glm::min(value, operand); }
	inline glm::vec2 max(glm::vec2 operand) { return glm::max(value, operand); }

	inline glm::vec2 abs() { return glm::abs(value); }
	inline glm::vec2 ceil() { return glm::ceil(value); }
	inline glm::vec2 floor() { return glm::floor(value); }
	inline glm::vec2 fract() { return glm::fract(value); }
	inline glm::vec2 normalize() { return glm::normalize(value); }
	inline glm::vec2 round() { return glm::round(value); }
	inline glm::vec2 sign() { return glm::sign(value); }
	inline glm::vec2 trunc() { return glm::trunc(value); }

	inline glm::vec2 clamp(glm::vec2 low, glm::vec2 high) { return glm::clamp(value, low, high); }
	inline glm::vec2 smoothstep(glm::vec2 operand, float x) { return glm::mix(value, operand, x); }
	inline glm::vec2 step(glm::vec2 operand) { return glm::step(value, operand); }

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

	inline static OtID xID = OtIdentifier::create("x");
	inline static OtID yID = OtIdentifier::create("y");
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
