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
#include "fog.h"


//
//	OtFogControllerClass
//

class OtFogControllerClass;
typedef std::shared_ptr<OtFogControllerClass> OtFogController;

class OtFogControllerClass : public OtWidgetClass {
public:
	// initialize controller
	void init(OtObject object);

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFogController create();

private:
	// our fog
	OtFog fog;
};
