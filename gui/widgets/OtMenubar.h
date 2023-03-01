//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtMenubarClass
//

class OtMenubarClass;
using OtMenubar = OtObjectPointer<OtMenubarClass>;

class OtMenubarClass : public OtWidgetClass {
public:
	// ensure specified node is allowed as a child
	void validateChild(OtNode child) override;

	// what is our height?
	size_t getHeight() { return height; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	size_t height;
};
