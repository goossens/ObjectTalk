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
//	OtEmptyClass
//

class OtEmptyClass;
using OtEmpty = OtObjectPointer<OtEmptyClass>;

class OtEmptyClass : public OtWidgetClass {
public:
	// get maximum number of children
	inline int getMaxChildren() override { return 0; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();
};
