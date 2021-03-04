//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Text manipulation functions (assuming strings are UTF-8 encoded)
//

inline size_t OtTextLen(const std::string& text) {
	size_t result = 0;

	for (auto i = text.begin(); i != text.end(); i += OtCodePointSize(i)) {
		result++;
	}

	return result;
}

inline std::string OtTextLeft(const std::string& text, size_t length) {
	auto end = text.begin();

	while (length > 0 && end != text.end()) {
		end += OtCodePointSize(end);
		length--;
	}

	return std::string(text.begin(), end);
}

inline std::string OtTextRight(const std::string& text, size_t length) {
	auto start = text.end();

	while (length > 0 && start != text.begin()) {
		do {
			start--;
		} while ((*start & 0xC0) == 0x80);

		length--;
	}

	return std::string(start, text.end());
}

inline std::string OtTextMid(const std::string& text, size_t start, size_t length) {
	auto s = text.begin();

	while (start > 0 && s != text.end()) {
		s += OtCodePointSize(s);
		start--;
	}

	auto e = s;

	while (length > 0 && e != text.end()) {
		e += OtCodePointSize(e);
		length--;
	}

	return std::string(s, e);
}

inline std::string OtTextGet(const std::string& text, size_t offset) {
	auto pos = text.begin();

	while (offset > 0 && pos != text.end()) {
		pos += OtCodePointSize(pos);
		offset--;
	}

	if (offset < 0 || pos == text.end()) {
		return "";

	} else {
		return std::string(pos, pos + OtCodePointSize(pos));
	}
}

inline std::string OtTextSet(const std::string& text, size_t offset, std::string& ch) {
	auto length = OtTextLen(text);

	if (offset >= 0 && offset < length) {
		return OtTextLeft(text, offset) +
			OtTextGet(ch, 0) +
			OtTextRight(text, length - offset - 1);

	} else {
		return text;
	}
}

inline std::string OtTextLower(const std::string& text) {
	std::string result(text.length(), 0);
	auto i = text.begin();
	auto i2 = result.begin();

	while (i < text.end()) {
		int32_t codepoint;

		i = OtCodePointGet(i, &codepoint);
		i2 = OtCodePointPut(i2, OtCodePointLower(codepoint));
	}

	return result;
}

inline std::string OtTextUpper(const std::string& text) {
	std::string result(text.length(), 0);
	auto i = text.begin();
	auto i2 = result.begin();

	while (i < text.end()) {
		int32_t codepoint;

		i = OtCodePointGet(i, &codepoint);
		i2 = OtCodePointPut(i2, OtCodePointUpper(codepoint));
	}

	return result;
}

inline int32_t OtTextCaseCmp(const std::string& s1, const std::string& s2) {
	auto i1 = s1.begin();
	auto i2 = s2.begin();

	while (i1 != s1.end() && i2 != s2.end()) {
		int32_t cp1, cp2;

		i1 = OtCodePointGet(i1, &cp1);
		i2 = OtCodePointGet(i2, &cp2);

		cp1 = OtCodePointLower(cp1);
		cp2 = OtCodePointLower(cp2);

		if (cp1 != cp2) {
			return cp1 - cp2;
		}
	}

	if (i1 == s1.end() && i2 == s2.end()) {
		return 0;

	} else if (i1 == s1.end()) {
		return -1;

	} else {
		return 1;
	}
}

inline bool OtTextCaseEqual(const std::string& s1, const std::string& s2) {
	return OtTextCaseCmp(s1, s2) == 0;
}

inline bool OtTextStartsWith(const std::string& text, const std::string& part) {
	return text.rfind(part, 0) == 0;
}

inline bool OtTextContains(const std::string& text, const std::string& part) {
	return text.find(part) != std::string::npos;
}

inline std::string OtTextLeftTrim(std::string text, const std::string& chars ="\t\n\v\f\r ") {
	text.erase(0, text.find_first_not_of(chars));
	return text;
}

inline std::string OtTextRightTrim(std::string text, const std::string& chars ="\t\n\v\f\r ") {
	text.erase(text.find_last_not_of(chars) + 1);
	return text;
}

inline std::string OtTextTrim(std::string text, const std::string& chars ="\t\n\v\f\r ") {
	text.erase(0, text.find_first_not_of(chars));
	text.erase(text.find_last_not_of(chars) + 1);
	return text;
}

inline std::string OtTextCompress(const std::string& text, const std::string& chars ="\t\n\v\f\r ") {
	auto result = OtTextTrim(text);
	auto begin = result.find_first_of(chars);

	while (begin != std::string::npos) {
		result.replace(begin, result.find_first_not_of(chars, begin) - begin," ");
		begin = result.find_first_of(chars, begin + 1);
	}

	return result;
}

template <class Container>
void OtTextSplit(const std::string& text, Container& container, char delimeter =' ') {
	std::stringstream ss(text);
	std::string token;

	while (std::getline(ss, token, delimeter)) {
		container.push_back(token);
	}
}

template <class Function>
void OtTextSplitIterator(const char* begin, const char* end, char character, Function function) {
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

template <class Function>
void OtTextSplitTrimIterator(const char* begin, const char* end, char character, Function function) {
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

inline std::string OtTextEncodeURL(const std::string& text) {
	std::ostringstream o;

	for (auto c = text.cbegin(); c != text.cend(); c++) {
		if (isalnum(*c) || *c == '-' || *c == '_' || *c == '.' || *c == '~') {
			o << *c;

		} else {
			o << '%' << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(*c);
		}
	}

	return o.str();
}

inline std::string OtTextDecodeURL(const std::string& text) {
	std::ostringstream o;
	auto c = text.cbegin();

	while (c < text.cend()) {
		if (*c =='+') {
			c++;
			o <<' ';

		} else if (*c == '%') {
			c++;

			if (c + 2 < text.cend()) {
				o << ((char) std::strtol(std::string(c, c + 2).c_str(), nullptr, 16));
				c += 2;

			} else {
				c = text.cend();
			}

		} else {
			o << *c++;
		}
	}

	return o.str();
}


//
//	JSON Conversion Functions
//

inline std::string OtTextToJSON(const std::string& text) {
	std::ostringstream o;
	o << '"';

	for (auto c = text.begin(); c != text.end(); c += OtCodePointSize(c)) {
		switch (*c) {
			case '"': o << "\\\""; break;
			case '\\': o << "\\\\"; break;
			case '\b': o << "\\b"; break;
			case '\f': o << "\\f"; break;
			case '\n': o << "\\n"; break;
			case '\r': o << "\\r"; break;
			case '\t': o << "\\t"; break;

			default:
				if (*c & 0x80) {
					int32_t codepoint;
					OtCodePointGet(c, &codepoint);
					o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << codepoint;

				} else {
					o << *c;
				}
		}
	}

	o << '"';
	return o.str();
}

inline std::string OtTextFromJSON(const std::string text) {
	std::ostringstream o;
	auto c = text.begin();

	while (c < text.end()) {
		if (*c =='\\') {
			c++;

			if (c < text.cend()) {
				switch (*c) {
					case'b':
						c++;
						o << '\b';
						break;

					case'f':
						c++;
						o << '\f';
						break;

					case'n':
						c++;
						o << '\n';
						break;

					case'r':
						c++;
						o << '\r';
						break;

					case't':
						c++;
						o << '\t';
						break;

					case'u':
						c++;

						if (c + 4 <= text.cend()) {
							std::string utf8(4, 0);

							int32_t codepoint = std::strtol(std::string(c, c + 4).c_str(), nullptr, 16);
							auto end = OtCodePointPut(utf8.begin(), codepoint);

							o << std::string(utf8.begin(), end);
							c += 4;

						} else {
							throw OtException("Invalid UTF-8 string in JSON");
						}

						break;

					default:
						o << *c++;
				}
			}

		} else {
			o << *c++;
		}
	}

	return o.str();
}
