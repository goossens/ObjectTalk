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

#include "component.h"


//
//	OtSceneObject
//

class OtSceneObjectClass;
typedef std::shared_ptr<OtSceneObjectClass> OtSceneObject;

class OtSceneObjectClass : public OtComponentClass {
public:
	// render in BGFX
    virtual void render(int view, glm::mat4 parentTransform) {}

	// get type definition
	static OtType getMeta();
};