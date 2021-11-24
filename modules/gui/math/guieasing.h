//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "gui.h"


//
//	OtEasing functions
//

typedef bx::EaseFn OtEasingFunction;

int OtEasingGetCount();
std::string OtEasingGetLabel(int index);
OtEasingFunction OtEasingGetFunction(int index);
OtEasingFunction OtEasingGetFunction(const std::string& name);

namespace ImGui {
	bool Easing(const char* label, int* value);
}
