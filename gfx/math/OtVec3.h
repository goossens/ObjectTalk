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
#include "OtLog.h"
#include "OtObject.h"
#include "OtReal.h"
#include "OtValue.h"


//
//	OtVec3
//

class OtVec3Class;
using OtVec3 = OtObjectPointer<OtVec3Class>;

class OtVec3Class : public OtObjectClass {
public:
	// constructors
	OtVec3Class() {}
	OtVec3Class(float scalar) : value(scalar) {}
	OtVec3Class(float x, float y, float z) : value(x, y, z) {}
	OtVec3Class(const glm::vec3& v) : value(v) {}

	// initialize
	inline void init(size_t count, OtObject* parameters);

	// debugging support
	inline std::string describe() override { return fmt::format("Vec3({}, {}, {})", value.x, value.y, value.z); }
	inline operator std::string() override { return describe(); }

	// access members
	inline operator glm::vec3() { return value; }

	OtObject set(OtID id, OtObject value) override;
	OtObject get(OtID id) override;

	inline OtObject setX(float x) { value.x = x; return OtVec3(this); }
	inline OtObject setY(float y) { value.y = y; return OtVec3(this); }
	inline OtObject setZ(float z) { value.z = z; return OtVec3(this); }

	inline float getX() { return value.x; }
	inline float getY() { return value.y; }
	inline float getZ() { return value.z; }

	// vector arithmetic
	inline glm::vec3 add(glm::vec3 operand) { return value + operand; }
	inline glm::vec3 subtract(glm::vec3 operand) { return value - operand; }
	inline glm::vec3 multiply(glm::vec3 operand) { return value * operand; }
	inline glm::vec3 divide(glm::vec3 operand) { return value / operand; }
	inline glm::vec3 modulo(glm::vec3 operand) { return glm::mod(value, operand); }

	inline bool equal(glm::vec3 operand) { return value == operand; }
	inline bool notEqual(glm::vec3 operand) { return value != operand; }

	inline float length() { return glm::length(value); }
	inline float distance(glm::vec3 operand) { return glm::distance(value, operand); }

	inline float dot(glm::vec3 operand) { return glm::dot(value, operand); }
	inline glm::vec3 cross(glm::vec3 operand) { return glm::cross(value, operand); }

	inline glm::vec3 min(glm::vec3 operand) { return glm::min(value, operand); }
	inline glm::vec3 max(glm::vec3 operand) { return glm::max(value, operand); }

	inline glm::vec3 abs() { return glm::abs(value); }
	inline glm::vec3 ceil() { return glm::ceil(value); }
	inline glm::vec3 floor() { return glm::floor(value); }
	inline glm::vec3 fract() { return glm::fract(value); }
	inline glm::vec3 normalize() { return glm::normalize(value); }
	inline glm::vec3 round() { return glm::round(value); }
	inline glm::vec3 sign() { return glm::sign(value); }
	inline glm::vec3 trunc() { return glm::trunc(value); }

	inline glm::vec3 clamp(glm::vec3 low, glm::vec3 high) { return glm::clamp(value, low, high); }
	inline glm::vec3 smoothstep(glm::vec3 operand, float x) { return glm::mix(value, operand, x); }
	inline glm::vec3 step(glm::vec3 operand) { return glm::step(value, operand); }

	inline glm::vec3 lerp(glm::vec3 operand, float delta) {
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
	glm::vec3 value = glm::vec3(1.0f);

	inline static OtID xID = OtIdentifier::create("x");
	inline static OtID yID = OtIdentifier::create("y");
	inline static OtID zID = OtIdentifier::create("z");
};


//
//	Allow glm::vec3 as an ObjectTalk value
//

template <>
struct OtValue<glm::vec3> {
	static inline OtObject encode(glm::vec3 vec3) {
		return OtVec3::create(vec3);
	}

	static inline glm::vec3 decode(OtObject object) {
		if (object.isKindOf<OtVec3Class>()) {
			return OtVec3(object)->operator glm::vec3();

		} else if (object.isKindOf<OtIntegerClass>() || object.isKindOf<OtRealClass>()) {
			return glm::vec3(object->operator float());

		} else {
			OtLogError("Expected a [Vec3], [Real] or [Integer], not a [{}]", object->getType()->getName());
			return glm::vec3();
		}
	}
};
