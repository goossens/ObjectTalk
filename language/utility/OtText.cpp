//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>
#include <iomanip>
#include <string>

#include "OtCodePoint.h"
#include "OtLog.h"
#include "OtText.h"


//
//	OtText::load
//

std::string OtText::load(const std::string& path) {
	std::string text;
	load(path, text);
	return text;
}

void OtText::load(const std::string& path, std::string& text) {
	try {
		// try to load the text
		std::ifstream stream(path.c_str());

		if (stream.fail()) {
			OtLogError("Can't open file [{}] for reading", path);
		}

		stream.seekg(0, std::ios::end);
		text.reserve(stream.tellg());
		stream.seekg(0, std::ios::beg);

		text.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		stream.close();

	} catch (std::exception& e) {
		OtLogError("Can't read from file [{}], error: {}", path, e.what());
	}
}


//
//	OtText::save
//

void OtText::save(const std::string& path, const std::string& text) {
	try {
		std::ofstream stream(path);

		if (stream.fail()) {
			OtLogError("Can't open file [{}] for writing", path);
		}

		stream << text;
		stream.close();

	} catch (std::exception& e) {
		OtLogError("Can't write to file [{}], error: {}", path, e.what());
	}
}


//
//	OtText::len
//

size_t OtText::len(const std::string& text) {
	size_t result = 0;

	for (auto i = text.begin(); i != text.end(); i += OtCodePoint::size(i)) {
		result++;
	}

	return result;
}


//
//	OtText::left
//

std::string OtText::left(const std::string& text, size_t length) {
	auto end = text.begin();

	while (length > 0 && end != text.end()) {
		end += OtCodePoint::size(end);
		length--;
	}

	return std::string(text.begin(), end);
}


//
//	OtText::right
//

std::string OtText::right(const std::string& text, size_t length) {
	auto start = text.end();

	while (length > 0 && start != text.begin()) {
		do {
			start--;
		} while ((*start & 0xC0) == 0x80);

		length--;
	}

	return std::string(start, text.end());
}


//
//	OtText::mid
//

std::string OtText::mid(const std::string& text, size_t start, size_t length) {
	auto s = text.begin();

	while (start > 0 && s != text.end()) {
		s += OtCodePoint::size(s);
		start--;
	}

	auto e = s;

	while (length > 0 && e != text.end()) {
		e += OtCodePoint::size(e);
		length--;
	}

	return std::string(s, e);
}


//
//	OtText::from
//

std::string OtText::from(const std::string& text, size_t start) {
	auto s = text.begin();

	while (start > 0 && s != text.end()) {
		s += OtCodePoint::size(s);
		start--;
	}

	return std::string(s, text.end());
}


//
//	OtText::get
//

std::string OtText::get(const std::string& text, size_t offset) {
	auto pos = text.begin();

	while (offset > 0 && pos != text.end()) {
		pos += OtCodePoint::size(pos);
		offset--;
	}

	if (pos == text.end()) {
		return "";

	} else {
		return std::string(pos, pos + OtCodePoint::size(pos));
	}
}


//
//	OtText::set
//

std::string OtText::set(const std::string& text, size_t offset, const std::string& ch) {
	auto length = OtText::len(text);

	if (offset < length) {
		return OtText::left(text, offset) + OtText::get(ch, 0) + OtText::right(text, length - offset - 1);

	} else {
		return text;
	}
}


//
//	OtText::lower
//

std::string OtText::lower(const std::string& text) {
	std::string result;
	auto end = text.end();
	auto i = text.begin();
	std::string utf8(4, 0);

	while (i < end) {
		char32_t codepoint;
		i = OtCodePoint::read(i, end, &codepoint);
		result.append(utf8.begin(), OtCodePoint::write(utf8.begin(), OtCodePoint::toLowerCase(codepoint)));
	}

	return result;
}


//
//	OtText::upper
//

std::string OtText::upper(const std::string& text) {
	std::string result;
	auto end = text.end();
	auto i = text.begin();
	std::string utf8(4, 0);

	while (i < end) {
		char32_t codepoint;
		i = OtCodePoint::read(i, end, &codepoint);
		result.append(utf8.begin(), OtCodePoint::write(utf8.begin(), OtCodePoint::toUpperCase(codepoint)));
	}

	return result;
}


//
//	OtText::caseCmp
//

int32_t OtText::caseCmp(const std::string& s1, const std::string& s2) {
	auto i1 = s1.begin();
	auto e1 = s1.end();
	auto i2 = s2.begin();
	auto e2 = s2.end();

	while (i1 != e1 && i2 != e2) {
		char32_t cp1, cp2;

		i1 = OtCodePoint::read(i1, e1, &cp1);
		i2 = OtCodePoint::read(i2, e2, &cp2);

		cp1 = OtCodePoint::toLowerCase(cp1);
		cp2 = OtCodePoint::toLowerCase(cp2);

		if (cp1 != cp2) {
			return static_cast<int32_t>(cp1 - cp2);
		}
	}

	if (i1 == e1 && i2 == e2) {
		return 0;

	} else if (i1 == e1) {
		return -1;

	} else {
		return 1;
	}
}


//
//	OtText::caseEqual
//

bool OtText::caseEqual(const std::string& s1, const std::string& s2) {
	return OtText::caseCmp(s1, s2) == 0;
}


//
//	OtText::startsWith
//

bool OtText::startsWith(const std::string& text, const std::string& part) {
	return text.rfind(part, 0) == 0;
}


//
//	OtText::contains
//

bool OtText::contains(const std::string& text, const std::string& part) {
	return text.find(part) != std::string::npos;
}


//
//	OtText::leftTrim
//

std::string OtText::leftTrim(std::string text, const std::string& chars) {
	text.erase(0, text.find_first_not_of(chars));
	return text;
}


//
//	OtText::rightTrim
//

std::string OtText::rightTrim(std::string text, const std::string& chars) {
	text.erase(text.find_last_not_of(chars) + 1);
	return text;
}


//
//	OtText::trim
//

std::string OtText::trim(std::string text, const std::string& chars) {
	text.erase(0, text.find_first_not_of(chars));
	text.erase(text.find_last_not_of(chars) + 1);
	return text;
}


//
//	OtText::compressWhitespace
//

std::string OtText::compressWhitespace(const std::string& text, const std::string& chars) {
	auto result = OtText::trim(text, chars);
	auto begin = result.find_first_of(chars);

	while (begin != std::string::npos) {
		result.replace(begin, result.find_first_not_of(chars, begin) - begin, " ");
		begin = result.find_first_of(chars, begin + 1);
	}

	return result;
}


//
//	OtText::removeAllWhitespace
//

std::string OtText::removeAllWhitespace(const std::string& text, const std::string& chars) {
	auto result = OtText::trim(text, chars);
	auto begin = result.find_first_of(chars);

	while (begin != std::string::npos) {
		result.replace(begin, result.find_first_not_of(chars, begin) - begin, "");
		begin = result.find_first_of(chars, begin + 1);
	}

	return result;
}


//
//	OtText::pad
//

std::string OtText::pad(const std::string& text, size_t size, char character) {
	auto currentSize = len(text);

	if (size < currentSize) {
		return left(text, size);

	} else if (size == currentSize) {
		return text;

	} else {
		return text + std::string(size - currentSize, character);
	}
}


//
//	OtText::EncodeURL
//

std::string OtText::encodeURL(const std::string& text) {
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


//
//	OtText::DecodeURL
//

std::string OtText::decodeURL(const std::string& text) {
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
//	OtText::toJSON
//

std::string OtText::toJSON(const std::string& text) {
	std::ostringstream o;
	o << '"';
	auto end = text.end();

	for (auto c = text.begin(); c != end; c += OtCodePoint::size(c)) {
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
					char32_t codepoint;
					OtCodePoint::read(c, end, &codepoint);
					o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << codepoint;

				} else {
					o << *c;
				}
		}
	}

	o << '"';
	return o.str();
}


//
//	OtText::fromJSON
//

std::string OtText::fromJSON(const std::string text) {
	std::ostringstream o;
	auto c = text.begin();

	while (c < text.end()) {
		if (*c =='\\') {
			c++;

			if (c < text.cend()) {
				switch (*c) {
					case 'b': c++; o << '\b'; break;
					case 'f': c++; o << '\f'; break;
					case 'n': c++; o << '\n'; break;
					case 'r': c++; o << '\r'; break;
					case 't': c++; o << '\t'; break;

					case 'u':
						c++;

						if (c + 4 <= text.cend()) {
							std::string utf8(4, 0);

							auto codepoint = static_cast<int32_t>(std::strtol(std::string(c, c + 4).c_str(), nullptr, 16));
							auto end = OtCodePoint::write(utf8.begin(), codepoint);

							o << std::string(utf8.begin(), end);
							c += 4;

						} else {
							OtLogError("Invalid UTF-8 string in JSON");
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
