//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "widget.h"
#include "scene.h"


//
//	OtLightControllerClass
//

class OtLightControllerClass;
typedef std::shared_ptr<OtLightControllerClass> OtLightController;

class OtLightControllerClass : public OtWidgetClass {
public:
	// initialize controller
	void init(OtObject object);

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtLightController create();

private:
	// our scene
	OtScene scene;
};
