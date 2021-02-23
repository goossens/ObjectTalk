//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


//
//	Text manipulation functions
//

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

inline bool OtTextCaseInsensitiveEqual(const std::string& s1, const std::string& s2) {
	return s1.size() == s2.size() &&
		std::equal(s1.begin(), s1.end(), s2.begin(), [](char a, char b) {
			return tolower(a) == tolower(b);
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
			o <<'%' << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(*c);
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

		} else if (*c =='%') {
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

	for (auto c = text.cbegin(); c != text.cend(); c++) {
		switch (*c) {
			case'"': o <<"\\\""; break;
			case'\\': o <<"\\\\"; break;
			case'\b': o <<"\\b"; break;
			case'\f': o <<"\\f"; break;
			case'\n': o <<"\\n"; break;
			case'\r': o <<"\\r"; break;
			case'\t': o <<"\\t"; break;

			default:
				if (*c <' ' || *c >'~') {
					o <<"\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(*c);

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
	auto c = text.cbegin();

	while (c < text.cend()) {
		if (*c =='\\') {
			c++;

			if (c < text.cend()) {
				switch (*c) {
					case'b':
						c++;
						o <<'\b';
						break;

					case'f':
						c++;
						o <<'\f';
						break;

					case'n':
						c++;
						o <<'\n';
						break;

					case'r':
						c++;
						o <<'\r';
						break;

					case't':
						c++;
						o <<'\t';
						break;

					case'u':
						c++;

						if (c + 4 < text.cend()) {
							o << ((char) std::strtol(std::string(c, c + 4).c_str(), nullptr, 16));
							c += 4;

						} else {
							c = text.cend();
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
