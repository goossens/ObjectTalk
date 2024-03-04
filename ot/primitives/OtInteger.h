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
#include <string>

#include "OtException.h"
#include "OtNumbers.h"
#include "OtPrimitive.h"


//
//	OtInteger
//

class OtIntegerClass;
using OtInteger = OtObjectPointer<OtIntegerClass>;

class OtIntegerClass : public OtPrimitiveClass {
public:
	// constructors
	OtIntegerClass() = default;
	OtIntegerClass(int64_t integer) : value(integer) {}

	// convertors
	operator bool() override { return value != 0; }
	operator int() override { return (int) value; }
	operator int64_t()  override{ return value; }
	operator size_t() override { return (size_t) value; }
	operator float() override { return (float) value; }
	operator double() override { return (double) value; }
	operator std::string() override {return std::to_string(value); }

	// debugging support
	std::string describe() override { return operator std::string(); }

	// comparison
	bool operator==(OtObject operand) override { return value == operand->operator int64_t(); }
	bool operator<(OtObject operand) override { return value < operand->operator int64_t(); }

	bool equal(int64_t operand) { return value == operand; }
	bool notEqual(int64_t operand) { return value != operand; }
	bool greaterThan(int64_t operand) { return value > operand; }
	bool lessThan(int64_t operand) { return value < operand; }
	bool greaterEqual(int64_t operand) { return value >= operand; }
	bool lessEqual(int64_t operand) { return value <= operand; }

	// arithmetic
	int64_t add(int64_t operand) { return value + operand; }
	int64_t subtract(int64_t operand) { return value - operand; }
	int64_t multiply(int64_t operand) { return value * operand; }
	int64_t divide(int64_t operand) { if (operand == 0) OtError("Divide by zero"); return value / operand; }
	int64_t modulo(int64_t operand) { if (operand == 0) OtError("Divide by zero"); return value % operand; }
	int64_t power(int64_t operand) { return std::pow(value, operand); }

	int64_t increment() { return value + 1; }
	int64_t decrement() { return value - 1; }

	int64_t shiftLeft(int64_t operand) { return value << operand; }
	int64_t shiftRight(int64_t operand) { return value >> operand; }

	int64_t bitwiseAnd(int64_t operand) { return value & operand; }
	int64_t bitwiseOr(int64_t operand) { return value | operand; }
	int64_t bitwiseXor(int64_t operand) { return value ^ operand; }
	int64_t bitwiseNot() { return ~value; }

	// functions
	int64_t negate() { return -value; }
	int64_t positive() { return value; }
	int64_t abs() { return std::abs(value); }
	int64_t sign() { return (0 < value) - (value < 0); }
	int64_t min(int64_t operand) { return std::min(value, operand); }
	int64_t max(int64_t operand) { return std::max(value, operand); }
	int64_t clamp(int64_t min, int64_t max) { return std::clamp(value, min, max); }
	int64_t random() { return (int64_t) OtRandom(double(value)); };

	// get type definition
	static OtType getMeta();

private:
	int64_t value = 0;
};
