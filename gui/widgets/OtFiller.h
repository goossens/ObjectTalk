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
//	OtFillerClass
//

class OtFillerClass;
using OtFiller = OtObjectPointer<OtFillerClass>;

class OtFillerClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// set the height of the filler
	OtObject setSize(float size);

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	float size;
};
