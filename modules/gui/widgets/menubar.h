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


//
//	OtMenubarClass
//

class OtMenubarClass;
typedef std::shared_ptr<OtMenubarClass> OtMenubar;

class OtMenubarClass : public OtWidgetClass {
public:
	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// what is our height?
	size_t getHeight() { return height; }

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMenubar create();

private:
	size_t height;
};
