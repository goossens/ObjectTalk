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
#include <iostream>


//
//	OtException
//

class OtException {
public:
	OtException(std::string m) : message(m) {}
	virtual const char* what() const throw() { return message.c_str(); }

private:
	std::string message;
};


//
//	OtExcept
//

template <typename... ARGS>
void OtExcept(const char* format, ARGS && ...args) {
	if constexpr(sizeof...(ARGS) == 0) {
		throw OtException(format);

	} else {
		auto size = std::snprintf(nullptr, 0, format, std::forward<ARGS>(args)...);
		std::string result(size, '\0');
		std::sprintf(&result[0], format, std::forward<ARGS>(args)...);
		throw OtException(result);
	}
}

inline void OtExcept(const std::string message) {
	throw OtException(message);
}

//
//	Macros
//

#define OT_FATAL(format, ...) OtExcept("%s: line %d: " format, __FILE__, __LINE__, __VA_ARGS__)
#define OT_ASSERT(assertion) if (!(assertion)) OT_FATAL("Assertion error: %s", #assertion)
#define OT_DEBUG(value) std::cout << (value) << std::endl
