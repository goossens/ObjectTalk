//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cctype>
#include <string>
#include <sstream>


//
//	Text manipulation functions (assuming strings are UTF-8 encoded)
//

size_t OtTextLen(const std::string& text);

std::string OtTextLeft(const std::string& text, size_t length);
std::string OtTextRight(const std::string& text, size_t length);
std::string OtTextMid(const std::string& text, size_t start, size_t length);

std::string OtTextGet(const std::string& text, size_t offset);
std::string OtTextSet(const std::string& text, size_t offset, std::string& ch);

std::string OtTextLower(const std::string& text);
std::string OtTextUpper(const std::string& text);

int32_t OtTextCaseCmp(const std::string& s1, const std::string& s2);
bool OtTextCaseEqual(const std::string& s1, const std::string& s2) ;

bool OtTextStartsWith(const std::string& text, const std::string& part);
bool OtTextContains(const std::string& text, const std::string& part);

std::string OtTextLeftTrim(std::string text, const std::string& chars="\t\n\v\f\r ");
std::string OtTextRightTrim(std::string text, const std::string& chars="\t\n\v\f\r ");
std::string OtTextTrim(std::string text, const std::string& chars="\t\n\v\f\r ");
std::string OtTextCompress(const std::string& text, const std::string& chars="\t\n\v\f\r ");

template <class CONTAINER>
void OtTextSplit(const std::string& text, CONTAINER& container, char delimeter =' ') {
	std::stringstream ss(text);
	std::string token;

	while (std::getline(ss, token, delimeter)) {
		container.push_back(token);
	}
}

template <class FUNCTION>
void OtTextSplitIterator(const char* begin, const char* end, char character, FUNCTION function) {
	auto i = begin;
	auto start = begin;

	while (i < end) {
		if (*i == character) {
			function(start, i);
			start = i + 1;
		}

		i++;
	}

	if (start < end) {
		function(start, end);
	}
}

template <class FUNCTION>
void OtTextSplitTrimIterator(const char* begin, const char* end, char character, FUNCTION function) {
	OtTextSplitIterator(begin, end, character, [&](const char *b, const char *e) {
		while (isspace(*b) && b < e) {
			b++;
		}

		while (isspace(*(e - 1)) && e > b) {
			e--;
		}

		function(b, e);
	});
}


//
//	URL Conversion Functions
//

std::string OtTextEncodeURL(const std::string& text);
std::string OtTextDecodeURL(const std::string& text);

//
//	JSON Conversion Functions
//

std::string OtTextToJSON(const std::string& text);
std::string OtTextFromJSON(const std::string text);
