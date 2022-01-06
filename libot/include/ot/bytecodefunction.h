//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "internal.h"
#include "bytecode.h"


//
//	OtByteCodeFunction
//

class OtByteCodeFunctionClass;
typedef std::shared_ptr<OtByteCodeFunctionClass> OtByteCodeFunction;

class OtByteCodeFunctionClass : public OtInternalClass {
public:
	// constructor
	OtByteCodeFunctionClass() = default;
	OtByteCodeFunctionClass(OtByteCode c, size_t p) : bytecode(c), parameterCount(p) {}

	// call code
	OtObject operator () (size_t count, OtObject* parameters);

	// get parameter count
	size_t getParameterCount() { return parameterCount; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtByteCodeFunction create(OtByteCode bytecode, size_t parameterCount);

private:
	OtByteCode bytecode;
	size_t parameterCount;
};
