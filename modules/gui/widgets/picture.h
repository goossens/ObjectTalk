//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "texture.h"
#include "widget.h"


//
//	OtPictureClass
//

class OtPictureClass;
typedef std::shared_ptr<OtPictureClass> OtPicture;

class OtPictureClass : public OtWidgetClass {
public:
	// initialize checkbox
	void init(const std::string& filename);

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPicture create();

private:
	// properties
	OtTexture texture;
};
