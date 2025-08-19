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
//	OtVec2
//

class OtVec2Class;
using OtVec2 = OtObjectPointer<OtVec2Class>;

class OtVec2Class : public OtObjectClass {
public:
	// initialize
	inline void init(size_t count, OtObject* parameters) {
		if (count == 2) {
			value = glm::vec2(parameters[0]->operator float(), parameters[1]->operator float());

		} else if (count == 1) {
			value = glm::vec2(parameters[0]->operator float());

		} else if (count != 0) {
			OtLogError("[Vec2] constructor expects 0, 1 or 2 arguments (not {})", count);
		}
	}

	// debugging support
	inline std::string describe() override { return fmt::format("Vec2({}, {})", value.x, value.y); }
	inline operator std::string() override { return fmt::format("{}, {}", value.x, value.y); }

	// access members
	inline operator glm::vec2() { return value; }

	inline OtObject set(OtID id, OtObject v) override {
		if (id == xID) {
			value.x = v->operator float();

		} else if (id == yID) {
			value.y = v->operator float();

		} else {
			OtObjectClass::set(id, v);
		}

		return v;
	}

	inline OtObject get(OtID id) override {
		if (id == xID) {
			return OtReal::create(value.x);

		} else if (id == yID) {
			return OtReal::create(value.y);

		} else {
			return OtObjectClass::get(id);
		}
	}

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

protected:
	// constructors
	friend class OtObjectPointer<OtVec2Class>;
	OtVec2Class() {}
	OtVec2Class(float scalar) : value(scalar) {}
	OtVec2Class(float x, float y) : value(x, y) {}
	OtVec2Class(const glm::vec2& v) : value(v) {}

private:
	// data
	glm::vec2 value = glm::vec2(1.0f);

	// identifiers
	inline static OtID xID = OtIdentifier::create("x");
	inline static OtID yID = OtIdentifier::create("y");
};


//
//	OtVec3
//

class OtVec3Class;
using OtVec3 = OtObjectPointer<OtVec3Class>;

class OtVec3Class : public OtObjectClass {
public:
	// initialize
	inline void init(size_t count, OtObject* parameters) {
		if (count == 3) {
			value = glm::vec3(
				parameters[0]->operator float(),
				parameters[1]->operator float(),
				parameters[2]->operator float());

		} else if (count == 1) {
			value = glm::vec3(parameters[0]->operator float());

		} else if (count != 0) {
			OtLogError("[Vec3] constructor expects 0, 1 or 3 arguments (not {})", count);
		}
	}

	// debugging support
	inline std::string describe() override { return fmt::format("Vec3({}, {}, {})", value.x, value.y, value.z); }
	inline operator std::string() override { return fmt::format("{}, {}, {}", value.x, value.y, value.z); }

	// access members
	inline operator glm::vec3() { return value; }

	inline OtObject set(OtID id, OtObject v) override {
		if (id == xID) {
			value.x = v->operator float();

		} else if (id == yID) {
			value.y = v->operator float();

		} else if (id == zID) {
			value.z = v->operator float();

		} else {
			OtObjectClass::set(id, v);
		}

		return v;
	}

	inline OtObject get(OtID id) override {
		if (id == xID) {
			return OtReal::create(value.x);

		} else if (id == yID) {
			return OtReal::create(value.y);

		} else if (id == zID) {
			return OtReal::create(value.z);

		} else {
			return OtObjectClass::get(id);
		}
	}

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

protected:
	// constructors
	friend class OtObjectPointer<OtVec3Class>;
	OtVec3Class() {}
	OtVec3Class(float scalar) : value(scalar) {}
	OtVec3Class(float x, float y, float z) : value(x, y, z) {}
	OtVec3Class(const glm::vec3& v) : value(v) {}

private:
	// data
	glm::vec3 value = glm::vec3(1.0f);

	// identifiers
	inline static OtID xID = OtIdentifier::create("x");
	inline static OtID yID = OtIdentifier::create("y");
	inline static OtID zID = OtIdentifier::create("z");
};


//
//	OtVec4
//

class OtVec4Class;
using OtVec4 = OtObjectPointer<OtVec4Class>;

class OtVec4Class : public OtObjectClass {
public:
	// initialize
	inline void init(size_t count, OtObject* parameters) {
		if (count == 4) {
			value = glm::vec4(
				parameters[0]->operator float(),
				parameters[1]->operator float(),
				parameters[2]->operator float(),
				parameters[3]->operator float());

		} else if (count == 1) {
			value = glm::vec4(parameters[0]->operator float());

		} else if (count != 0) {
			OtLogError("[Vec4] constructor expects 0, 1 or 4 arguments (not {})", count);
		}
	}

	// debugging support
	inline std::string describe() override { return fmt::format("Vec4({}, {}, {}, {})", value.x, value.y, value.z, value.w); }
	inline operator std::string() override { return fmt::format("{}, {}, {}, {}", value.x, value.y, value.z, value.w); }

	// access members
	inline operator glm::vec4() { return value; }

	inline OtObject set(OtID id, OtObject v) override {
		if (id == xID) {
			value.x = v->operator float();

		} else if (id == yID) {
			value.y = v->operator float();

		} else if (id == zID) {
			value.z = v->operator float();

		} else if (id == wID) {
			value.w = v->operator float();

		} else {
			OtObjectClass::set(id, v);
		}

		return v;
	}

	inline OtObject get(OtID id) override {
		if (id == xID) {
			return OtReal::create(value.x);

		} else if (id == yID) {
			return OtReal::create(value.y);

		} else if (id == zID) {
			return OtReal::create(value.z);

		} else if (id == wID) {
			return OtReal::create(value.w);

		} else {
			return OtObjectClass::get(id);
		}
	}

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
	inline glm::vec4 homogenize() { return glm::vec4(value.x / value.w, value.y / value.w, value.z / value.w, 1.0f); }

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

protected:
	// constructors
	friend class OtObjectPointer<OtVec4Class>;
	OtVec4Class() {}
	OtVec4Class(float scalar) : value(scalar) {}
	OtVec4Class(float x, float y, float z, float w) : value(x, y, z, w) {}
	OtVec4Class(const glm::vec4& v) : value(v) {}

private:
	// data
	glm::vec4 value = glm::vec4(1.0f);

	// identifiers
	inline static OtID xID = OtIdentifier::create("x");
	inline static OtID yID = OtIdentifier::create("y");
	inline static OtID zID = OtIdentifier::create("z");
	inline static OtID wID = OtIdentifier::create("w");
};


//
//	Allow vectors as ObjectTalk values
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
			OtLogError("Expected a [Vec2], [Real] or [Integer], not a [{}]", object.getTypeName());
			return glm::vec2();
		}
	}
};


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
			OtLogError("Expected a [Vec3], [Real] or [Integer], not a [{}]", object.getTypeName());
			return glm::vec3();
		}
	}
};


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
			OtLogError("Expected a [Vec4], [Real] or [Integer], not a [{}]", object.getTypeName());
			return glm::vec4();
		}
	}
};
