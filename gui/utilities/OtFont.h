//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "imstb_truetype.h"

#include "OtGui.h"
#include "OtShape.h"


//
//	OtFont
//

class OtFontClass;
typedef std::shared_ptr<OtFontClass> OtFont;

class OtFontClass : public OtGuiClass {
public:
	// destructor
	~OtFontClass();

	// initialize
	void init(size_t count, OtObject* parameters);

	// update properties
	OtObject setFont(const std::string& file);
	OtObject setSize(float size);

	// get the width of a text string
	float getWidth(const std::string& text);

	// add text to shape
	void createShape(OtShape shape, const std::string& text);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFont create();

private:
	// properties
	float size = 12.0;

	// font data
	uint8_t* data = nullptr;
	stbtt_fontinfo font;
};
