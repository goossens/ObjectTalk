//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	  Include files
//

#include <stdio.h>

#include <string>


//
//	OtFormat
//

template <typename... ARGS>
std::string OtFormat(const char* format, ARGS && ...args) {
	auto size = std::snprintf(nullptr, 0, format, std::forward<ARGS>(args)...);
	std::string result(size, '\0');
	std::sprintf(&result[0], format, std::forward<ARGS>(args)...);
	return result;
}
