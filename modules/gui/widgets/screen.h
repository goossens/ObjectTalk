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
//	OtScreenClass
//

class OtScreenClass;
typedef std::shared_ptr<OtScreenClass> OtScreen;

class OtScreenClass : public OtWidgetClass {
public:
	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// get menubar height
	size_t getMenubarHeight();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScreen create();
};
