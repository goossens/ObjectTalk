//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"

#include "component.h"
#include "fog.h"
#include "light.h"


//
//	OtSceneClass
//

class OtSceneClass;
typedef std::shared_ptr<OtSceneClass> OtScene;

class OtSceneClass : public OtComponentClass {
public:
	// constructor/destructor
	OtSceneClass();
	~OtSceneClass();

	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// render content
	void render(int view, const glm::mat4& viewMatrix);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScene create();

private:
	// to pass information to shaders
	bgfx::UniformHandle lightUniform = BGFX_INVALID_HANDLE;
};