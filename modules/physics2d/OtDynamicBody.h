//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBody.h"


//
//	OtDynamicBodyClass
//

class OtDynamicBodyClass;
using OtDynamicBody = OtObjectPointer<OtDynamicBodyClass>;

class OtDynamicBodyClass : public OtBodyClass {
public:
	// initializer (ObjectTalk constructor)
	void init(OtObject world);

	// get type definition
	static OtType getMeta();
};
