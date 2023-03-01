//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtNode.h"


//
//	OtWidgetClass
//

class OtWidgetClass;
using OtWidget = OtObjectPointer<OtWidgetClass>;

class OtWidgetClass : public OtNodeClass {
public:
	// ensure specified node is allowed as a child
	void validateChild(OtNode child) override;

	// update state (called every frame so be carefull)
	virtual void update();

	// render content
	virtual void render();

	// get type definition
	static OtType getMeta();
};
