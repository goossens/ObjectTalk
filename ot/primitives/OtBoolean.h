//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtPrimitive.h"


//
//	OtBoolean
//

class OtBooleanClass;
using OtBoolean = OtObjectPointer<OtBooleanClass>;

class OtBooleanClass : public OtPrimitiveClass {
public:
	// constructors
	OtBooleanClass() = default;
	OtBooleanClass(bool boolean) : value(boolean) {}

	// conversions
	operator bool() override { return value; }
	operator int() override { return value ? 1 : 0; }
	operator int64_t() override { return value ? 1 : 0; }
	operator size_t() override { return value ? 1 : 0; }
	operator float() override { return value ? 1.0 : 0.0; }
	operator double() override { return value ? 1.0 : 0.0; }
	operator std::string() override { return value ? "true" : "false"; }

	std::string describe() override { return operator std::string(); }

	// comparison
	bool operator==(OtObject operand) override { return value == operand->operator bool(); }
	bool operator<(OtObject operand) override { return value < operand->operator bool(); }

	bool equal(bool operand) { return value == operand; }
	bool notEqual(bool operand) { return value != operand; }

	// arithmetic
	bool logicalAnd(bool operand) { return value && operand; }
	bool logicalOr(bool operand) { return value || operand; }
	bool logicalNot() { return !value; }

	// get type definition
	static OtType getMeta();

private:
	bool value = false;
};
