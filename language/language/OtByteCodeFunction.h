//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtByteCode.h"
#include "OtIdentifier.h"
#include "OtInternal.h"


//
//	OtByteCodeFunction
//

class OtByteCodeFunctionClass;
using OtByteCodeFunction = OtObjectPointer<OtByteCodeFunctionClass>;

class OtByteCodeFunctionClass : public OtInternalClass {
public:
	// debugging support
	inline std::string describe() override { return std::string(OtIdentifier::name(bytecode->getID())); }

	// call code
	OtObject operator()(size_t count, OtObject* parameters) override;

	// get parameter count
	inline size_t getParameterCount() { return parameterCount; }

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtByteCodeFunctionClass>;
	OtByteCodeFunctionClass() = default;
	OtByteCodeFunctionClass(OtByteCode c, size_t p) : bytecode(c), parameterCount(p) {}

private:
	// data
	OtByteCode bytecode;
	size_t parameterCount;
};
