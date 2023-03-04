//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cctype>
#include <iomanip>
#include <sstream>

#include "OtArray.h"
#include "OtCodePoint.h"
#include "OtFunction.h"
#include "OtString.h"
#include "OtStringReference.h"
#include "OtStringIterator.h"
#include "OtText.h"


//
//	OtStringClass::getEntry
//

std::string OtStringClass::getEntry(size_t index) {
	// sanity check
	if (index < 0 || index >= len()) {
		OtExcept("Invalid index [%ld] for string of size [%ld]", index, len());
	}

	return OtText::get(value, index);
}


//
//	OtStringClass::setEntry
//

std::string OtStringClass::setEntry(size_t index, const std::string& string) {
	// sanity check
	if (index < 0 || index >= len()) {
		OtExcept("Invalid index [%ld] for string of size [%ld]", index, len());
	}

	value = OtText::set(value, index, string);
	return value;
}


//
//	OtStringClass::index
//

OtObject OtStringClass::index(size_t index) {
	// sanity check
	if (index < 0 || index >= len()) {
		OtExcept("Invalid index [%ld] for string of size [%ld]", index, len());
	}

	return OtStringReference::create(OtString(this), index);
}


//
//	OtStringClass::iterate
//

OtObject OtStringClass::iterate() {
	return OtStringIterator::create(OtString(this));
}


//
//	OtStringClass::split
//

OtObject OtStringClass::split(const std::string& delimiter) {
	OtArray result = OtArray::create();
	size_t start = 0;
	size_t end;

	while ((end = value.find(delimiter, start)) != std::string::npos) {
		result->append(OtString::create(value.substr(start, end - start)));
		start = end + delimiter.size();
	}

	result->append(OtString::create(value.substr(start)));
	return result;
}


//
//	OtStringClass::format
//

struct formatParameters {
	bool left = false;
	char sign = 0;
	char pad = ' ';
	bool alternative = false;
	int width = -1;
	int precision = -1;
	int radix = 10;
	char type;
};


static const char* parseFormat(formatParameters& format, const char* string) {
	// decode flags
	while (*string == '-' || *string == '+' || *string == '#' || *string == ' ' || *string == '0') {
		if (*string == '-')	format.left = true;
		else if (*string == '+') format.sign = '+';
		else if (*string == '#') format.alternative = true;
		else if (*string == ' ') format.sign = ' ';
		else if (*string == '0') format.pad = '0';
		string++;
	}

	// decode field width
	if (*string == '*') {
		format.width = -2;
		string++;

	} else if (std::isdigit(*string)) {
		format.width = 0;

		while (std::isdigit(*string)) {
			format.width = (10 * format.width) + (*string++ - '0');
		}
	}

	// decode precision
	if (*string == '.') {
		string++;

		if (*string == '*') {
			format.precision = -2;
			string++;

		} else {
			format.precision = 0;

			while (std::isdigit(*string)) {
				format.precision = (10 * format.precision) + (*string++ - '0');
			}
		}
	}

	// ignore decode flags
	while (*string == 'h' || *string == 'l' || *string == 'L') {
		string++;
	}

	// get format
	format.type = *string++;

	// return pointer
	return string;
}


#define SANITY_CHECK() if (index == count) { OtExcept("Not enough parameters for string.format"); }


OtObject OtStringClass::format(size_t count, OtObject* objects) {
	// setup parser
	const char* mask = value.c_str();
	size_t index = 0;
	std::stringstream ss;

	// process entire mask
	while (*mask) {
		// handle parameter references
		if (*mask == '%') {
			// parse format definition
			formatParameters format;
			mask = parseFormat(format, ++mask);

			// get field width if required
			if (format.width == -2) {
				SANITY_CHECK();
				format.width = objects[index++]->operator int();

				if (format.width < 0) {
					format.left = true;
					format.width = -format.width;
				}
			}

			// get precision if required
			if (format.precision == -2) {
				SANITY_CHECK();
				format.precision = objects[index++]->operator int();

				if (format.precision < 0) {
					format.left = true;
					format.precision = -format.precision;
				}
			}

			switch (format.type) {
				case 'd':
				case 'i':
					// get integer number
					if (format.sign) { ss << format.sign; }

					if (format.width != -1) {
						ss << std::setw(format.width)
							<< std::setfill(format.pad)
							<< (format.left ? std::left : std::right);
					}

					SANITY_CHECK();
					ss << objects[index++]->operator int64_t();
					break;

				case 'o':
				case 'p':
				case 'u':
				case 'x':
				case 'X':
					// get number base
					if (format.type == 'o') { ss << std::oct; }
					if (format.type == 'p' || format.type == 'x' || format.type == 'X') { ss << std::hex; }

					SANITY_CHECK();
					ss << std::showbase << objects[index++]->operator int64_t();
					break;

				case 'f':
				case 'F':
				case 'g':
				case 'G':
					if (format.sign) { ss << format.sign; }
					if (format.precision != -1) { ss << std::setprecision(format.precision); }

					if (format.width != -1) {
						ss << std::setw(format.width)
							<< std::setfill(format.pad)
							<< (format.left ? std::left : std::right);
					}

					SANITY_CHECK();
					ss << std::fixed << objects[index++]->operator double();
					break;

				case 'e':
				case 'E':
					if (format.sign) { ss << format.sign; }
					if (format.precision != -1) { ss << std::setprecision(format.precision); }

					if (format.width != -1) {
						ss << std::setw(format.width)
							<< std::setfill(format.pad)
						<< (format.left ? std::left: std::right);
					}

					SANITY_CHECK();
					ss << std::scientific << objects[index++]->operator double();
					break;

				case 'c':
					SANITY_CHECK();
					ss << (char) objects[index++]->operator int();
					break;

				case 's':
					SANITY_CHECK();
					ss << objects[index++]->operator std::string();
					break;

				case '%':
					ss << '%';
					break;
			}

		} else {
			ss << *mask++;
		}
	}

	// return result
	return OtString::create(ss.str());
}


//
//	OtStringClass::getMeta
//

OtType OtStringClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtStringClass>("String", OtPrimitiveClass::getMeta());

		type->set("boolean", OtFunction::create(&OtStringClass::operator bool));
		type->set("integer", OtFunction::create(&OtStringClass::operator int64_t));
		type->set("real", OtFunction::create(&OtStringClass::operator double));
		type->set("string", OtFunction::create(&OtStringClass::operator std::string));
		type->set("json", OtFunction::create(&OtStringClass::json));

		type->set("__eq__", OtFunction::create(&OtStringClass::equal));
		type->set("__ne__", OtFunction::create(&OtStringClass::notEqual));
		type->set("__gt__", OtFunction::create(&OtStringClass::greaterThan));
		type->set("__lt__", OtFunction::create(&OtStringClass::lessThan));
		type->set("__ge__", OtFunction::create(&OtStringClass::greaterEqual));
		type->set("__le__", OtFunction::create(&OtStringClass::lessEqual));

		type->set("casecmp", OtFunction::create(&OtStringClass::casecmp));

		type->set("__index__", OtFunction::create(&OtStringClass::index));
		type->set("__iter__", OtFunction::create(&OtStringClass::iterate));
		type->set("__add__", OtFunction::create(&OtStringClass::add));

		type->set("len", OtFunction::create(&OtStringClass::len));

		type->set("left", OtFunction::create(&OtStringClass::left));
		type->set("right", OtFunction::create(&OtStringClass::right));
		type->set("mid", OtFunction::create(&OtStringClass::mid));

		type->set("find", OtFunction::create(&OtStringClass::find));
		type->set("startsWith", OtFunction::create(&OtStringClass::startsWith));
		type->set("contains", OtFunction::create(&OtStringClass::contains));
		type->set("__contains__", OtFunction::create(&OtStringClass::contains));

		type->set("trim", OtFunction::create(&OtStringClass::trim));
		type->set("ltrim", OtFunction::create(&OtStringClass::ltrim));
		type->set("rtrim", OtFunction::create(&OtStringClass::rtrim));
		type->set("compress", OtFunction::create(&OtStringClass::compress));

		type->set("lower", OtFunction::create(&OtStringClass::lower));
		type->set("upper", OtFunction::create(&OtStringClass::upper));

		type->set("split", OtFunction::create(&OtStringClass::split));

		type->set("format", OtFunction::create(&OtStringClass::format));
	}

	return type;
}
