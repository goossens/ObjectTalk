//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtInternal.h"


//
//	OtDebugger
//

class OtDebuggerClass;
using OtDebugger = OtObjectPointer<OtDebuggerClass>;

class OtDebuggerClass : public OtInternalClass {
public:
	// debug
	void debug(size_t count, OtObject* parameters);

	// get type definition
	static OtType getMeta();

private:
};
