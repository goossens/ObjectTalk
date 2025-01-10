//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

	// debugging support
	inline std::string describe() override { return fmt::format("Vec4({}, {}, {}, {})", value.x, value.y, value.z, value.w); }
	inline operator std::string() override { return describe(); }

	// access members
	inline operator glm::vec4() { return value; }

	OtObject set(OtID id, OtObject value) override;
	OtObject get(OtID id) override;

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
	inline glm::vec4 modulo(glm::vec4 operand) { return glm::mod(value, operand); }

	inline bool equal(glm::vec4 operand) { return value == operand; }
	inline bool notEqual(glm::vec4 operand) { return value != operand; }

	inline float length() { return glm::length(value); }
	inline float distance(glm::vec4 operand) { return glm::distance(value, operand); }
	inline glm::vec4 homogonize() { return glm::vec4(value.x / value.w, value.y / value.w, value.z / value.w, 1.0f); }

	inline glm::vec4 min(glm::vec4 operand) { return glm::min(value, operand); }
	inline glm::vec4 max(glm::vec4 operand) { return glm::max(value, operand); }

	inline glm::vec4 abs() { return glm::abs(value); }
	inline glm::vec4 ceil() { return glm::ceil(value); }
	inline glm::vec4 floor() { return glm::floor(value); }
	inline glm::vec4 fract() { return glm::fract(value); }
	inline glm::vec4 normalize() { return glm::normalize(value); }
	inline glm::vec4 round() { return glm::round(value); }
	inline glm::vec4 sign() { return glm::sign(value); }
	inline glm::vec4 trunc() { return glm::trunc(value); }

	inline glm::vec4 clamp(glm::vec4 low, glm::vec4 high) { return glm::clamp(value, low, high); }
	inline glm::vec4 smoothstep(glm::vec4 operand, float x) { return glm::mix(value, operand, x); }
	inline glm::vec4 step(glm::vec4 operand) { return glm::step(value, operand); }

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

	inline static OtID xID = OtIdentifier::create("x");
	inline static OtID yID = OtIdentifier::create("y");
	inline static OtID zID = OtIdentifier::create("z");
	inline static OtID wID = OtIdentifier::create("w");
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
