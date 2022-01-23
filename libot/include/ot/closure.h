//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <unordered_map>

#include "internal.h"
#include "bytecodefunction.h"
#include "stack.h"


//
//	OtClosureClass
//

class OtClosureClass;
typedef std::shared_ptr<OtClosureClass> OtClosure;

class OtClosureClass : public OtInternalClass {
public:
	// constructor
	OtClosureClass() = default;
	OtClosureClass(OtByteCodeFunction f, const std::unordered_map<std::string, OtStackItem>& c) : function(f), captures(c) {}

	// capture required variables in returned clone
	OtObject capture();

	// call enclosed function
	OtObject operator () (size_t count, OtObject* parameters);

	// get parameter count
	size_t getParameterCount() { return function->getParameterCount(); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtClosure create(OtByteCodeFunction function, const std::unordered_map<std::string, OtStackItem>& captures);

private:
	OtByteCodeFunction function;
	std::unordered_map<std::string, OtStackItem> captures;
};
