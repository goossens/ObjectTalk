//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <string>


//
//	OtCodepoint
//

class OtCodePoint {
public:
	// get the number of bytes required for this code point
	static size_t size(std::string::const_iterator i);

	// get the next code point and update iterator
	static std::string::const_iterator get(std::string::const_iterator i, int32_t* codeword);

	// write code point in UTF-8 and update iterator
	static std::string::iterator put(std::string::iterator i, int32_t codeword);

	// convert code point to lower case
	static int32_t lower(int32_t cp);

	// convert code point to upper case
	static int32_t upper(int32_t cp);
};
