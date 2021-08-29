//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "object3d.h"


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
    virtual void render(int view, glm::mat4 parentTransform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtGroup create();
};
