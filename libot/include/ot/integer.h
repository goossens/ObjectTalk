//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cmath>
#include <string>

#include "exception.h"
#include "primitive.h"


//
//	OtInteger
//

class OtIntegerClass;
typedef std::shared_ptr<OtIntegerClass> OtInteger;

class OtIntegerClass : public OtPrimitiveClass {
public:
	// constructors
	OtIntegerClass() = default;
	OtIntegerClass(long integer) : value(integer) {}

	// convertors
	operator bool() { return value != 0; }
	operator int() { return (int) value; }
	operator long() { return value; }
	operator size_t() { return (size_t) value; }
	operator double() { return double(value); }
	operator std::string() {return std::to_string(value); }

	// debugging support
	std::string describe() { return operator std::string(); }

	// comparison
	bool operator == (OtObject operand) { return value == operand->operator long(); }
	bool operator < (OtObject operand) { return value < operand->operator long(); }

	bool equal(long operand) { return value == operand; }
	bool notEqual(long operand) { return value != operand; }
	bool greaterThan(long operand) { return value > operand; }
	bool lessThan(long operand) { return value < operand; }
	bool greaterEqual(long operand) { return value >= operand; }
	bool lessEqual(long operand) { return value <= operand; }

	// arithmetic
	long add(long operand) { return value + operand; }
	long subtract(long operand) { return value - operand; }
	long multiply(long operand) { return value * operand; }
	long divide(long operand) { if (operand == 0) OtExcept("Divide by zero"); return value / operand; }
	long modulo(long operand) { if (operand == 0) OtExcept("Divide by zero"); return value % operand; }
	long power(long operand) { return std::pow(value, operand); }

	long increment() { return value + 1; }
	long decrement() { return value - 1; }

	long shiftLeft(long operand) { return value << operand; }
	long shiftRight(long operand) { return value >> operand; }

	long bitwiseAnd(long operand) { return value & operand; }
	long bitwiseOr(long operand) { return value | operand; }
	long bitwiseXor(long operand) { return value ^ operand; }
	long bitwiseNot() { return ~value; }

	// functions
	long negate() { return -value; }
	long positive() { return value; }
	long abs() { return std::abs(value); }
	long sign() { return (0 < value) - (value < 0); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtInteger create(long value);

private:
	long value = 0;
};
