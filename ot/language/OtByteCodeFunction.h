//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtByteCode.h"
#include "OtInternal.h"


//
//	OtByteCodeFunction
//

class OtByteCodeFunctionClass;
using OtByteCodeFunction = OtObjectPointer<OtByteCodeFunctionClass>;

class OtByteCodeFunctionClass : public OtInternalClass {
public:
	// constructor
	OtByteCodeFunctionClass() = default;
	OtByteCodeFunctionClass(OtByteCode c, size_t p) : bytecode(c), parameterCount(p) {}

	// call code
	OtObject operator()(size_t count, OtObject* parameters) override;

	// get parameter count
	size_t getParameterCount() { return parameterCount; }

	// get type definition
	static OtType getMeta();

private:
	OtByteCode bytecode;
	size_t parameterCount;
};
