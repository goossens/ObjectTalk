//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
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

template <typename... Args>
std::string OtFormat(const char* format, Args && ...args) {
	auto size = std::snprintf(nullptr, 0, format, std::forward<Args>(args)...);
	std::string result(size, '\0');
	std::sprintf(&result[0], format, std::forward<Args>(args)...);
	return result;
}
