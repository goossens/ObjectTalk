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
	void init(size_t count, OtObject* parameters);

	// specify a new picture
	OtObject setTexture(const std::string& texture);

	// set the margin around the picture
	OtObject setMargin(int margin);

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPicture create();

private:
	// properties
	OtTexture texture;
	int margin = 5;

	size_t width;
	size_t height;
};
