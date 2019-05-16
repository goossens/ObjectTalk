//	ObjectTalk Scripting Language
//	Copyright 1993-2018 Johan A. Goossens
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

inline std::string OtTextLeftTrim(std::string text, const std::string& chars = "\t\n\v\f\r ")
{
	text.erase(0, text.find_first_not_of(chars));
	return text;
}

inline std::string OtTextRightTrim(std::string text, const std::string& chars = "\t\n\v\f\r ")
{
	text.erase(text.find_last_not_of(chars) + 1);
	return text;
}

inline std::string OtTextTrim(std::string text, const std::string& chars = "\t\n\v\f\r ")
{
	return OtTextLeftTrim(OtTextRightTrim(text, chars), chars);
}

inline std::string OtTextCompress(const std::string& text)
{
	auto result = OtTextTrim(text);
	auto begin = result.find_first_of(" \t\n\r\f\v");

	while (begin != std::string::npos)
	{
		result.replace(begin, result.find_first_not_of(" \t\n\r\f\v", begin) - begin, " ");
		begin = result.find_first_of(" \t\n\r\f\v", begin + 1);
	}

	return result;
}

template <class Container>
void OtTextSplit(const std::string& text, Container& container, char delimeter = ' ')
{
	std::stringstream ss(text);
	std::string token;

	while (std::getline(ss, token, delimeter))
		container.push_back(token);
}

inline bool OtTextCaseInsensitiveEqual(const std::string& s1, const std::string & s2)
{
	return s1.size() == s2.size() && std::equal(s1.begin(), s1.end(), s2.begin(), [](char a, char b) { return tolower(a) == tolower(b); });
}


//
//	JSON Functions
//

inline std::string OtTextToJSON(const std::string &text)
{
	std::ostringstream o;
	o << '"';

	for (auto c = text.cbegin(); c != text.cend(); c++)
	{
		switch (*c)
		{
			case '"': o << "\\\""; break;
			case '\\': o << "\\\\"; break;
			case '\b': o << "\\b"; break;
			case '\f': o << "\\f"; break;
			case '\n': o << "\\n"; break;
			case '\r': o << "\\r"; break;
			case '\t': o << "\\t"; break;

			default:
				if ('\x00' <= *c && *c <= '\x1f')
					o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int) *c;

				else
					o << *c;

		}
	}

	o << '"';
	return o.str();
}
