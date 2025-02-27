//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

	// conversion operators
	inline operator bool() override { return value; }
	inline operator int() override { return value ? 1 : 0; }
	inline operator int64_t() override { return value ? 1 : 0; }
	inline operator size_t() override { return value ? 1 : 0; }
	inline operator float() override { return value ? 1.0f : 0.0f; }
	inline operator double() override { return value ? 1.0 : 0.0; }
	inline operator std::string() override { return value ? "true" : "false"; }

	// debugging support
	inline std::string describe() override { return operator std::string(); }

	// comparison
	inline bool operator==(OtObject operand) override { return value == operand->operator bool(); }
	inline bool operator<(OtObject operand) override { return value < operand->operator bool(); }

	inline bool equal(bool operand) { return value == operand; }
	inline bool notEqual(bool operand) { return value != operand; }

	// arithmetic
	inline bool logicalAnd(bool operand) { return value && operand; }
	inline bool logicalOr(bool operand) { return value || operand; }
	inline bool logicalNot() { return !value; }

	inline bool bitwiseAnd(bool operand) { return value && operand; }
	inline bool bitwiseOr(bool operand) { return value || operand; }
	inline bool bitwiseXor(bool operand) { return value ^ operand; }
	inline bool bitwiseNot() { return !value; }

	// get type definition
	static OtType getMeta();

private:
	bool value = false;
};
