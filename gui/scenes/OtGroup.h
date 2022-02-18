//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject3d.h"


//
//	OtGroupClass
//

class OtGroupClass;
typedef std::shared_ptr<OtGroupClass> OtGroup;

class OtGroupClass : public OtObject3dClass {
public:
	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// render in BGFX
	void render(OtRenderingContext context);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtGroup create();
};
