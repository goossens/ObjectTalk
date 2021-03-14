//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "primitive.h"


//
//	OtBoolean
//

class OtBooleanClass;
typedef std::shared_ptr<OtBooleanClass> OtBoolean;

class OtBooleanClass : public OtPrimitiveClass {
public:
	// constructors
	OtBooleanClass() = default;
	OtBooleanClass(bool boolean) : value(boolean) {}

	// conversions
	operator bool() { return value; }
	operator long() { return value ? 1 : 0; }
	operator size_t() { return value ? 1 : 0; }
	operator double() { return value ? 1.0 : 0.0; }
	operator std::string() { return value ? "true" : "false"; }

	// arithmetic
	bool logicalAnd(bool operand) { return value && operand; }
	bool logicalOr(bool operand) { return value || operand; }
	bool logicalNot() { return !value; }

	// comparison
	bool equal(bool operand) { return value == operand; }
	bool notEqual(bool operand) { return value != operand; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBoolean create(bool value);

private:
	bool value = false;
};