//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtTexture.h"
#include "OtWidget.h"


//
//	OtPictureClass
//

class OtPictureClass;
typedef std::shared_ptr<OtPictureClass> OtPicture;

class OtPictureClass : public OtWidgetClass {
public:
	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// specify a new picture
	OtObject setTexture(OtObject texture);

	// set the margin around the picture
	OtObject setMargin(int margin);

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPicture create();

private:
	// properties
	OtTexture texture;
	int margin = 5;
};