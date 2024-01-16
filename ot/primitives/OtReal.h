//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "OtException.h"
#include "OtNumbers.h"
#include "OtPrimitive.h"


//
//	OtReal
//

class OtRealClass;
using OtReal = OtObjectPointer<OtRealClass>;

class OtRealClass : public OtPrimitiveClass {
public:
	// constructors
	OtRealClass() = default;
	OtRealClass(double real) : value(real) {}

	// convertors
	operator bool() override { return value != 0.0; }
	operator int() override { return (int) value; }
	operator int64_t() override { return (int64_t) value; }
	operator size_t() override { return (size_t) value; }
	operator float() override { return (float) value; }
	operator double() override { return value; }
	operator std::string() override {return std::to_string(value); }

	// debugging support
	std::string describe() override { return operator std::string(); }

	// comparison
	bool operator==(OtObject operand) override { return value == operand->operator double(); }
	bool operator<(OtObject operand) override { return value < operand->operator double(); }

	bool equal(double operand) { return value == operand; }
	bool notEqual(double operand) { return value != operand; }
	bool greaterThan(double operand) { return value > operand; }
	bool lessThan(double operand) { return value < operand; }
	bool greaterEqual(double operand) { return value >= operand; }
	bool lessEqual(double operand) { return value <= operand; }

	// arithmetic
	double add(double operand) { return value + operand; }
	double subtract(double operand) { return value - operand; }
	double multiply(double operand) { return value * operand; }
	double divide(double operand) { if (operand == 0.0) OtError("Divide by zero"); return value / operand; }
	double modulo(double operand) { if (operand == 0.0) OtError("Divide by zero"); return std::fmod(value, operand); }
	double power(double operand) { return std::pow(value, operand); }

	// functions
	double negate() { return -value; }
	double positive() { return value; }
	double abs() { return std::abs(value); }
	int64_t sign() { return (0.0 < value) - (value < 0.0); }
	int64_t round() { return std::round(value); }
	int64_t ceil() { return std::ceil(value); }
	int64_t floor() { return std::floor(value); }
	int64_t trunc() { return std::trunc(value); }
	double min(double operand) { return std::min(value, operand); }
	double max(double operand) { return std::max(value, operand); }
	double clamp(double min, double max) { return std::clamp(value, min, max); }
	double lerp(double min, double max) { return std::lerp(min, max, value); }

	double sin() { return std::sin(value); }
	double cos() { return std::cos(value); }
	double tan() { return std::tan(value); }
	double asin() { return std::asin(value); }
	double acos() { return std::acos(value); }
	double atan() { return std::atan(value); }
	double atan2(double operand) { return std::atan2(value, operand); }
	double radians() { return value * std::numbers::pi / 180.0; }
	double degrees() { return value / std::numbers::pi * 180.0; }

	double sqrt() { if (value < 0.0) OtError("Sqrt requires number >= zero"); return std::sqrt(value); }
	double pow(double exp) { return std::pow(value, exp); }

	double log() { if (value <= 0.0) OtError("Log requires number > zero"); return std::log(value); }
	double exp() { return std::exp(value); }
	double log10() { if (value <= 0.0) OtError("Log10 requires number > zero"); return std::log10(value); }

	double random() { return OtRandom(value); };

	std::string toFixed(int precision);

	// get type definition
	static OtType getMeta();

private:
	double value = 0.0;
};
