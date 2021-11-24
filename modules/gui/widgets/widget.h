//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "component.h"


//
//	OtWidgetClass
//

class OtWidgetClass;
typedef std::shared_ptr<OtWidgetClass> OtWidget;

class OtWidgetClass : public OtComponentClass {
public:
	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// update state (called every frame so be carefull)
    virtual void update();

    // render content
    virtual void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWidget create();
};
