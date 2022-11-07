//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
typedef std::shared_ptr<OtFillerClass> OtFiller;

class OtFillerClass : public OtWidgetClass {
public:
	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// set the height of the filler
	OtObject setSize(float size);

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFiller create();

private:
	// properties
	float size;
};
