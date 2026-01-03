//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBody.h"


//
//	OtKinematicBodyClass
//

class OtKinematicBodyClass;
using OtKinematicBody = OtObjectPointer<OtKinematicBodyClass>;

class OtKinematicBodyClass : public OtBodyClass {
public:
	// initializer (ObjectTalk constructor)
	void init(OtObject world);

	// get type definition
	static OtType getMeta();
};
