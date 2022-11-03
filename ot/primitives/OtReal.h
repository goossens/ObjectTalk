//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
typedef std::shared_ptr<OtRealClass> OtReal;

class OtRealClass : public OtPrimitiveClass {
public:
	// constructors
	OtRealClass() = default;
	OtRealClass(double real) : value(real) {}

	// convertors
	operator bool() { return value != 0.0; }
	operator int() { return (int) value; }
	operator long() { return (long) value; }
	operator size_t() { return (size_t) value; }
	operator float() { return (float) value; }
	operator double() { return value; }
	operator std::string() {return std::to_string(value); }

	// debugging support
	std::string describe() { return operator std::string(); }

	// comparison
	bool operator == (OtObject operand) { return value == operand->operator double(); }
	bool operator < (OtObject operand) { return value < operand->operator double(); }

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
	double divide(double operand) { if (operand == 0.0) OtExcept("Divide by zero"); return value / operand; }
	double modulo(double operand) { if (operand == 0.0) OtExcept("Divide by zero"); return std::fmod(value, operand); }
	double power(double operand) { return std::pow(value, operand); }

	// functions
	double negate() { return -value; }
	double positive() { return value; }
	double abs() { return std::abs(value); }
	long sign() { return (0.0 < value) - (value < 0.0); }
	long round() { return std::round(value); }
	long ceil() { return std::ceil(value); }
	long floor() { return std::floor(value); }
	long trunc() { return std::trunc(value); }
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

	double sqrt() { if (value < 0.0) OtExcept("Sqrt requires number >= zero"); return std::sqrt(value); }
	double pow(double exp) { return std::pow(value, exp); }

	double log() { if (value <= 0.0) OtExcept("Log requires number > zero"); return std::log(value); }
	double exp() { return std::exp(value); }
	double log10() { if (value <= 0.0) OtExcept("Log10 requires number > zero"); return std::log10(value); }

	double random() { return OtRandom(value); };

	std::string toFixed(int precision);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtReal create(double value);

private:
	double value = 0.0;
};
