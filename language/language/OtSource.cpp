//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <string>

#include "OtSource.h"


//
//	OtSourceClass::getStartOfLine
//

size_t OtSourceClass::getStartOfLine(size_t location) {
	auto startOfLine = source.rfind('\n', location);

	if (startOfLine == std::string::npos) {
		return 0;

	} else {
		return startOfLine + 1;
	}
}


//
//	OtSourceClass::getEndOfLine
//

size_t OtSourceClass::getEndOfLine(size_t location) {
	auto endOfLine = source.find('\n', location);

	if (endOfLine == std::string::npos) {
		return source.size();

	} else {
		return endOfLine;
	}
}


//
//	OtSourceClass::getLine
//

std::string OtSourceClass::getLine(size_t location) {
	// find start and end of line
	auto startOfLine = getStartOfLine(location);
	auto endOfLine = getEndOfLine(location);

	// get text line
	return source.substr(startOfLine, endOfLine - startOfLine);
}


//
//	OtSourceClass::getLines
//

std::string OtSourceClass::getLines(size_t start, size_t end) {
	// find start and end of lines
	auto startOfLine = getStartOfLine(start);
	auto skipWhitespace = source.find_last_not_of("\t\n\v\f\r ", end - 1);
	auto endOfLine = getEndOfLine(skipWhitespace);

	// get text lines
	return source.substr(startOfLine, endOfLine - startOfLine);
}


//
//	OtSourceClass::getLineNumber
//

size_t OtSourceClass::getLineNumber(size_t location) {
	return std::count(source.begin(), source.begin() + location, '\n') + 1;
}


OtSource OtSourceClass::create(const std::string& module, const std::string& source) {
	return std::make_shared<OtSourceClass>(module, source);
}
