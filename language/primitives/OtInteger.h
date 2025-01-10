//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	inline operator bool() override { return value != 0; }
	inline operator int() override { return (int) value; }
	inline operator int64_t()  override{ return value; }
	inline operator size_t() override { return (size_t) value; }
	inline operator float() override { return (float) value; }
	inline operator double() override { return (double) value; }
	inline operator std::string() override { return std::to_string(value); }

	// debugging support
	inline std::string describe() override { return operator std::string(); }

	// comparison
	inline bool operator==(OtObject operand) override { return value == operand->operator int64_t(); }
	inline bool operator<(OtObject operand) override { return value < operand->operator int64_t(); }

	inline bool equal(int64_t operand) { return value == operand; }
	inline bool notEqual(int64_t operand) { return value != operand; }
	inline bool greaterThan(int64_t operand) { return value > operand; }
	inline bool lessThan(int64_t operand) { return value < operand; }
	inline bool greaterEqual(int64_t operand) { return value >= operand; }
	inline bool lessEqual(int64_t operand) { return value <= operand; }

	// arithmetic
	inline int64_t add(int64_t operand) { return value + operand; }
	inline int64_t subtract(int64_t operand) { return value - operand; }
	inline int64_t multiply(int64_t operand) { return value * operand; }
	inline int64_t divide(int64_t operand) { if (operand == 0) OtError("Divide by zero"); return value / operand; }
	inline int64_t modulo(int64_t operand) { if (operand == 0) OtError("Divide by zero"); return value % operand; }
	inline int64_t power(int64_t operand) { return std::pow(value, operand); }

	inline int64_t increment() { return value + 1; }
	inline int64_t decrement() { return value - 1; }

	inline int64_t shiftLeft(int64_t operand) { return value << operand; }
	inline int64_t shiftRight(int64_t operand) { return value >> operand; }

	inline int64_t bitwiseAnd(int64_t operand) { return value & operand; }
	inline int64_t bitwiseOr(int64_t operand) { return value | operand; }
	inline int64_t bitwiseXor(int64_t operand) { return value ^ operand; }
	inline int64_t bitwiseNot() { return ~value; }

	// functions
	inline int64_t negate() { return -value; }
	inline int64_t positive() { return value; }
	inline int64_t abs() { return std::abs(value); }
	inline int64_t sign() { return (0 < value) - (value < 0); }
	inline int64_t min(int64_t operand) { return std::min(value, operand); }
	inline int64_t max(int64_t operand) { return std::max(value, operand); }
	inline int64_t clamp(int64_t min, int64_t max) { return std::clamp(value, min, max); }
	inline int64_t random() { return (int64_t) OtRandom(double(value)); };

	// get type definition
	static OtType getMeta();

private:
	int64_t value = 0;
};
