//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <unordered_map>

#include "OtByteCodeFunction.h"
#include "OtInternal.h"
#include "OtStack.h"


//
//	OtClosureClass
//

class OtClosureClass;
using OtClosure = OtObjectPointer<OtClosureClass>;

class OtClosureClass : public OtInternalClass {
public:
	// constructor
	OtClosureClass() = default;
	OtClosureClass(OtByteCodeFunction f, const std::unordered_map<size_t, OtStackItem>& c) : function(f), captures(c) {}

	// capture required variables in returned clone
	OtObject capture();

	// call enclosed function
	OtObject operator()(size_t count, OtObject* parameters) override;

	// get parameter count
	size_t getParameterCount() { return function->getParameterCount(); }

	// get type definition
	static OtType getMeta();

private:
	OtByteCodeFunction function;
	std::unordered_map<size_t, OtStackItem> captures;
};
