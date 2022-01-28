//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCamera.h"
#include "OtComponent.h"
#include "OtRenderingContext.h"


//
//	OtSceneObject
//

class OtSceneObjectClass;
typedef std::shared_ptr<OtSceneObjectClass> OtSceneObject;

class OtSceneObjectClass : public OtComponentClass {
public:
	// render in BGFX
	virtual void preRender(OtRenderingContext* context) {}
	virtual void render(OtRenderingContext* context) {}

	// get type definition
	static OtType getMeta();
};
