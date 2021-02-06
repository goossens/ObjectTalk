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

inline std::string OtTextLeftTrim(std::string text, const std::string& chars ="\t\n\v\f\r ") {
	text.erase(0, text.find_first_not_of(chars));
	return text;
}

inline std::string OtTextRightTrim(std::string text, const std::string& chars ="\t\n\v\f\r ") {
	text.erase(text.find_last_not_of(chars) + 1);
	return text;
}

inline std::string OtTextTrim(std::string text, const std::string& chars ="\t\n\v\f\r ") {
	return OtTextLeftTrim(OtTextRightTrim(text, chars), chars);
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
		if ((*c >='a' && *c <='z') || (*c >='A' && *c <='Z') || (*c >='0' && *c <='9')) {
			o << *c;

		} else if (*c ==' ') {
			o <<'+';

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
