//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//  Include files
//

#include "gui.h"


//
//	OtTheme Class
//

class OtTheme {
public:
	// initialize style;
	static void init();

	// colors
	static ImU32 color100;
	static ImU32 color80;
	static ImU32 color60;
	static ImU32 color40;
	static ImU32 color20;
	static ImU32 background;
	static ImU32 blend(ImU32 color1, ImU32 color2, float ratio);

	// fonts
	static ImFont* robotoFont;
	static ImFont* dsegFont;
	static ImFont* dsegBoldFont;
};
