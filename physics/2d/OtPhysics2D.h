//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"


//
//	OtPhysics2D
//

class OtPhysics2DClass;
using OtPhysics2D = OtObjectPointer<OtPhysics2DClass>;

class OtPhysics2DClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();
};
