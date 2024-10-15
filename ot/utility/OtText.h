//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtText
//

class OtText {
public:
	// get number of code points in UTF-8 string
	static size_t len(const std::string& text);

	// get parts of UTF-8 string
	static std::string left(const std::string& text, size_t length);
	static std::string right(const std::string& text, size_t length);
	static std::string mid(const std::string& text, size_t start, size_t length);
	static std::string from(const std::string& text, size_t start);

	// get code point at offset
	static std::string get(const std::string& text, size_t offset);

	// update code point at offset and return new string
	static std::string set(const std::string& text, size_t offset, const std::string& ch);

	// change case of UTF-8 string
	static std::string lower(const std::string& text);
	static std::string upper(const std::string& text);

	// compare UTF-8 strings
	static int32_t caseCmp(const std::string& s1, const std::string& s2);
	static bool caseEqual(const std::string& s1, const std::string& s2) ;
	static bool startsWith(const std::string& text, const std::string& part);
	static bool contains(const std::string& text, const std::string& part);

	// remove white space from UTF-8 string
	static std::string leftTrim(std::string text, const std::string& chars="\t\n\v\f\r ");
	static std::string rightTrim(std::string text, const std::string& chars="\t\n\v\f\r ");
	static std::string trim(std::string text, const std::string& chars="\t\n\v\f\r ");
	static std::string compressWhitespace(const std::string& text, const std::string& chars="\t\n\v\f\r ");
	static std::string removeAllWhitespace(const std::string& text, const std::string& chars="\t\n\v\f\r ");

	// split UTF-8 string by delimeter into a container
	template <class CONTAINER>
	static inline void split(const std::string& text, CONTAINER& container, char delimeter=' ') {
		std::stringstream ss(text);
		std::string token;

		while (std::getline(ss, token, delimeter)) {
			container.emplace_back(token);
		}
	}

	// UTF-8 string iterators
	template <class FUNCTION>
	static inline void splitIterator(const char* begin, const char* end, char character, FUNCTION function) {
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
	static inline void splitTrimIterator(const char* begin, const char* end, char character, FUNCTION function) {
		splitIterator(begin, end, character, [&](const char *b, const char *e) {
			while (isspace(*b) && b < e) {
				b++;
			}

			while (isspace(*(e - 1)) && e > b) {
				e--;
			}

			function(b, e);
		});
	}

	//	URL Conversion Functions
	static std::string encodeURL(const std::string& text);
	static std::string decodeURL(const std::string& text);

	//	JSON Conversion Functions
	static std::string toJSON(const std::string& text);
	static std::string fromJSON(const std::string text);
};
