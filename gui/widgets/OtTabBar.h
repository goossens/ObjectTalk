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
//	OtTabBarClass
//

class OtTabBarClass;
using OtTabBar = OtObjectPointer<OtTabBarClass>;

class OtTabBarClass : public OtWidgetClass {
public:
	// ensure specified node is allowed as a child
	void validateChild(OtWidget child) override;

	// get maximum number of children
	inline int getMaxChildren() override { return -1; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();
};
