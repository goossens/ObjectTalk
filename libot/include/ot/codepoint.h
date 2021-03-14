//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
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
//	Codepoint Functions
//

size_t OtCodePointSize(std::string::const_iterator i);
std::string::const_iterator OtCodePointGet(std::string::const_iterator i, int32_t* codeword);
std::string::iterator OtCodePointPut(std::string::iterator i, int32_t codeword);
int32_t OtCodePointLower(int32_t cp);
int32_t OtCodePointUpper(int32_t cp);
