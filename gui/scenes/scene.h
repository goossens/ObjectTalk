//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "component.h"
#include "renderingcontext.h"


//
//	OtSceneClass
//

class OtSceneClass;
typedef std::shared_ptr<OtSceneClass> OtScene;

class OtSceneClass : public OtComponentClass {
public:
	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// rendering phases
	void preRender(OtRenderingContext* context);
	void render(OtRenderingContext* context);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScene create();
};
