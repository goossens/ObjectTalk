//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtMenuBarClass
//

class OtMenuBarClass;
using OtMenuBar = OtObjectPointer<OtMenuBarClass>;

class OtMenuBarClass : public OtWidgetClass {
public:
	// ensure specified node is allowed as a child
	void validateChild(OtWidget child) override;

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();
};
