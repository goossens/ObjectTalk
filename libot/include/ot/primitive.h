//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "object.h"


//
//	OtPrimitive
//

class OtPrimitiveClass;
typedef std::shared_ptr<OtPrimitiveClass> OtPrimitive;

class OtPrimitiveClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();
};
