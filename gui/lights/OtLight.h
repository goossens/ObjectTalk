//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtRenderer.h"
#include "OtSceneObject.h"


//
//	OtLightClass
//

class OtLightClass;
using OtLight = OtObjectPointer<OtLightClass>;

class OtLightClass : public OtSceneObjectClass {
public:
	// add light properties to renderer
	virtual void addPropertiesToRenderer(OtRenderer& renderer) {};

	// get type definition
	static OtType getMeta();
};
