//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "imgui.h"


//
//	CodePoint
//

class CodePoint {
public:
	// get the number of bytes required in UTF-8 for this codepoint
	static size_t size(std::string::const_iterator i);

	// get the next codepoint in UTF-8 and update iterator
	static std::string::const_iterator get(std::string::const_iterator i, ImWchar* codepoint);

	// write codepoint in UTF-8 and update iterator
	static std::string::iterator put(std::string::iterator i, ImWchar codepoint);

	// get information about codepoint
	static bool isAlphaNumeric(ImWchar codepoint);
	static bool isWhiteSpace(ImWchar codepoint);
	static bool isUpper(ImWchar codepoint);
	static bool isLower(ImWchar codepoint);

	// convert codepoint
	static ImWchar toLower(ImWchar codepoint);
	static ImWchar toUpper(ImWchar codepoint);
};
