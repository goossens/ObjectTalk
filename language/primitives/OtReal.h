//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "OtLog.h"
#include "OtNumbers.h"
#include "OtPrimitive.h"
#include "OtRandom.h"


//
//	OtReal
//

class OtRealClass;
using OtReal = OtObjectPointer<OtRealClass>;

class OtRealClass : public OtPrimitiveClass {
public:
	// conversion operators
	inline operator bool() override { return value != 0.0; }
	inline operator int() override { return (int) value; }
	inline operator int64_t() override { return static_cast<int64_t>(value); }
	inline operator size_t() override { return static_cast<size_t>(value); }
	inline operator float() override { return static_cast<float>(value); }
	inline operator double() override { return value; }
	inline operator std::string() override {return std::to_string(value); }

	// debugging support
	inline std::string describe() override { return operator std::string(); }

	// comparison
	inline bool operator==(OtObject operand) override { return value == operand->operator double(); }
	inline bool operator<(OtObject operand) override { return value < operand->operator double(); }

	inline bool equal(double operand) { return value == operand; }
	inline bool notEqual(double operand) { return value != operand; }
	inline bool greaterThan(double operand) { return value > operand; }
	inline bool lessThan(double operand) { return value < operand; }
	inline bool greaterEqual(double operand) { return value >= operand; }
	inline bool lessEqual(double operand) { return value <= operand; }

	// arithmetic
	inline double add(double operand) { return value + operand; }
	inline double subtract(double operand) { return value - operand; }
	inline double multiply(double operand) { return value * operand; }
	inline double divide(double operand) { if (operand == 0.0) OtLogError("Divide by zero"); return value / operand; }
	inline double modulo(double operand) { if (operand == 0.0) OtLogError("Divide by zero"); return std::fmod(value, operand); }
	inline double power(double operand) { return std::pow(value, operand); }

	// functions
	inline double negate() { return -value; }
	inline double positive() { return value; }
	inline double abs() { return std::abs(value); }
	inline int64_t sign() { return (0.0 < value) - (value < 0.0); }
	inline int64_t round() { return static_cast<int64_t>(std::round(value)); }
	inline int64_t ceil() { return static_cast<int64_t>(std::ceil(value)); }
	inline int64_t floor() { return static_cast<int64_t>(std::floor(value)); }
	inline int64_t trunc() { return static_cast<int64_t>(std::trunc(value)); }
	inline double fraction() { return std::fmod(value, 1.0f); }
	inline double min(double operand) { return std::min(value, operand); }
	inline double max(double operand) { return std::max(value, operand); }
	inline double clamp(double min, double max) { return std::clamp(value, min, max); }
	inline double lerp(double min, double max) { return std::lerp(min, max, value); }

	inline double sin() { return std::sin(value); }
	inline double cos() { return std::cos(value); }
	inline double tan() { return std::tan(value); }
	inline double asin() { return std::asin(value); }
	inline double acos() { return std::acos(value); }
	inline double atan() { return std::atan(value); }
	inline double atan2(double operand) { return std::atan2(value, operand); }
	inline double radians() { return value * std::numbers::pi / 180.0; }
	inline double degrees() { return value / std::numbers::pi * 180.0; }

	inline double sqrt() { if (value < 0.0) OtLogError("Sqrt requires number >= zero"); return std::sqrt(value); }
	inline double pow(double exp) { return std::pow(value, exp); }

	inline double log() { if (value <= 0.0) OtLogError("Log requires number > zero"); return std::log(value); }
	inline double exp() { return std::exp(value); }
	inline double log10() { if (value <= 0.0) OtLogError("Log10 requires number > zero"); return std::log10(value); }

	inline double random() { return OtRandom(value); };

	std::string toFixed(int precision);

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtRealClass>;
	OtRealClass() = default;
	OtRealClass(double real) : value(real) {}

private:
	// data
	double value = 0.0;
};
