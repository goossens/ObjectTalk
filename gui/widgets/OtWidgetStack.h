//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtWidgetStackClass
//

class OtWidgetStackClass;
using OtWidgetStack = OtObjectPointer<OtWidgetStackClass>;

class OtWidgetStackClass : public OtWidgetClass {
public:
	// get maximum number of children
	inline int getMaxChildren() override { return -1; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();
};
