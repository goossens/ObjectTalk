//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.



#pragma once


//
//	Include files
//

#include "OtInternal.h"


//
//	OtBoundFunction
//

class OtBoundFunctionClass;
using OtBoundFunction = OtObjectPointer<OtBoundFunctionClass>;

class OtBoundFunctionClass : public OtInternalClass {
public:
	// constructor
	OtBoundFunctionClass() = default;
	OtBoundFunctionClass(OtObject o, OtObject f) : object(o), function(f) {}

	// debugging support
	inline std::string describe() override { return object.getTypeName() + " " + "function"; }

	// call bound function
	OtObject operator()(size_t count, OtObject* parameters) override;

	// get type definition
	static OtType getMeta();

private:
	OtObject object;
	OtObject function;
};
