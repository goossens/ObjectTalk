//	ObjectTalk Scripting Language
//	Copyright 1993-2020 Johan A. Goossens
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

inline std::wstring OtTextLeftTrim(std::wstring text, const std::wstring& chars = L"\t\n\v\f\r ") {
	text.erase(0, text.find_first_not_of(chars));
	return text;
}

inline std::wstring OtTextRightTrim(std::wstring text, const std::wstring& chars = L"\t\n\v\f\r ") {
	text.erase(text.find_last_not_of(chars) + 1);
	return text;
}

inline std::wstring OtTextTrim(std::wstring text, const std::wstring& chars = L"\t\n\v\f\r ") {
	return OtTextLeftTrim(OtTextRightTrim(text, chars), chars);
}

inline std::wstring OtTextCompress(const std::wstring& text, const std::wstring& chars = L"\t\n\v\f\r ") {
	auto result = OtTextTrim(text);
	auto begin = result.find_first_of(chars);

	while (begin != std::wstring::npos) {
		result.replace(begin, result.find_first_not_of(chars, begin) - begin, L" ");
		begin = result.find_first_of(chars, begin + 1);
	}

	return result;
}

template <class Container>
void OtTextSplit(const std::wstring& text, Container& container, wchar_t delimeter = L' ') {
	std::wstringstream ss(text);
	std::wstring token;

	while (std::getline(ss, token, delimeter)) {
		container.push_back(token);
	}
}

inline bool OtTextCaseInsensitiveEqual(const std::wstring& s1, const std::wstring & s2) {
	return s1.size() == s2.size() && std::equal(s1.begin(), s1.end(), s2.begin(), [](wchar_t a, wchar_t b) { return tolower(a) == tolower(b); });
}


//
//	Conversion Functions
//

inline std::wstring OtTextToWide(const std::string& narrow) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(narrow);
}

inline std::string OtTextToNarrow(const std::wstring& wide) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(wide);
}

inline std::wstring OtTextToJSON(const std::wstring& text) {
	std::wostringstream o;
	o << '"';

	for (auto c = text.cbegin(); c != text.cend(); c++) {
		switch (*c) {
			case L'"': o << L"\\\""; break;
			case L'\\': o << L"\\\\"; break;
			case L'\b': o << L"\\b"; break;
			case L'\f': o << L"\\f"; break;
			case L'\n': o << L"\\n"; break;
			case L'\r': o << L"\\r"; break;
			case L'\t': o << L"\\t"; break;

			default:
				if (*c < L' ' || *c > L'~') {
					o << L"\\u" << std::hex << std::setw(4) << std::setfill(L'0') << static_cast<unsigned int>(*c);

				} else {
					o << *c;
				}
		}
	}

	o << '"';
	return o.str();
}

inline std::wstring OtTextFromJSON(const std::wstring text) {
	std::wostringstream o;
	auto c = text.cbegin();

	while (c < text.cend()) {
		if (*c == L'\\') {
			c++;

			if (c < text.cend()) {
				switch (*c) {
					case L'b':
						c++;
						o << L'\b';
						break;

					case L'f':
						c++;
						o << L'\f';
						break;

					case L'n':
						c++;
						o << L'\n';
						break;

					case L'r':
						c++;
						o << L'\r';
						break;

					case L't':
						c++;
						o << L'\t';
						break;

					case L'u':
						c++;

						if (c + 4 < text.cend()) {
							o << ((wchar_t) std::wcstol(std::wstring(c, c + 4).c_str(), nullptr, 16));
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
