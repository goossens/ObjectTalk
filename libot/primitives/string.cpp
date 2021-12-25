//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cctype>
#include <iomanip>
#include <sstream>

#include "ot/codepoint.h"
#include "ot/text.h"

#include "ot/function.h"
#include "ot/string.h"
#include "ot/stringreference.h"
#include "ot/stringiterator.h"
#include "ot/array.h"


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

	return OtStringReferenceClass::create(cast<OtStringClass>(), index);
}


//
//	OtStringClass::iterate
//

OtObject OtStringClass::iterate() {
	return OtStringIteratorClass::create(cast<OtStringClass>());
}


//
//	OtStringClass::split
//

OtObject OtStringClass::split(const std::string& delimiter) {
	OtArray result = OtArrayClass::create();
	size_t start = 0;
	size_t end;

	while ((end = value.find(delimiter, start)) != std::string::npos) {
		result->append(OtStringClass::create(value.substr(start, end - start)));
		start = end + delimiter.size();
	}

	result->append(OtStringClass::create(value.substr(start)));
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
					ss << objects[index++]->operator long();
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
					ss << std::showbase << objects[index++]->operator long();
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
	return OtStringClass::create(ss.str());
}


//
//	OtStringClass::getMeta
//

OtType OtStringClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtStringClass>("String", OtPrimitiveClass::getMeta());

		type->set("boolean", OtFunctionClass::create(&OtStringClass::operator bool));
		type->set("integer", OtFunctionClass::create(&OtStringClass::operator long));
		type->set("real", OtFunctionClass::create(&OtStringClass::operator double));
		type->set("string", OtFunctionClass::create(&OtStringClass::operator std::string));
		type->set("json", OtFunctionClass::create(&OtStringClass::json));

		type->set("__eq__", OtFunctionClass::create(&OtStringClass::equal));
		type->set("__ne__", OtFunctionClass::create(&OtStringClass::notEqual));
		type->set("__gt__", OtFunctionClass::create(&OtStringClass::greaterThan));
		type->set("__lt__", OtFunctionClass::create(&OtStringClass::lessThan));
		type->set("__ge__", OtFunctionClass::create(&OtStringClass::greaterEqual));
		type->set("__le__", OtFunctionClass::create(&OtStringClass::lessEqual));

		type->set("casecmp", OtFunctionClass::create(&OtStringClass::casecmp));

		type->set("__index__", OtFunctionClass::create(&OtStringClass::index));
		type->set("__iter__", OtFunctionClass::create(&OtStringClass::iterate));
		type->set("__add__", OtFunctionClass::create(&OtStringClass::add));

		type->set("len", OtFunctionClass::create(&OtStringClass::len));

		type->set("left", OtFunctionClass::create(&OtStringClass::left));
		type->set("right", OtFunctionClass::create(&OtStringClass::right));
		type->set("mid", OtFunctionClass::create(&OtStringClass::mid));

		type->set("find", OtFunctionClass::create(&OtStringClass::find));
		type->set("startsWith", OtFunctionClass::create(&OtStringClass::startsWith));
		type->set("contains", OtFunctionClass::create(&OtStringClass::contains));
		type->set("__contains__", OtFunctionClass::create(&OtStringClass::contains));

		type->set("trim", OtFunctionClass::create(&OtStringClass::trim));
		type->set("ltrim", OtFunctionClass::create(&OtStringClass::ltrim));
		type->set("rtrim", OtFunctionClass::create(&OtStringClass::rtrim));
		type->set("compress", OtFunctionClass::create(&OtStringClass::compress));

		type->set("lower", OtFunctionClass::create(&OtStringClass::lower));
		type->set("upper", OtFunctionClass::create(&OtStringClass::upper));

		type->set("split", OtFunctionClass::create(&OtStringClass::split));

		type->set("format", OtFunctionClass::create(&OtStringClass::format));
	}

	return type;
}


//
//	OtStringClass::create
//

OtString OtStringClass::create(const std::string& value) {
	auto string = std::make_shared<OtStringClass>(value);
	string->setType(getMeta());
	return string;
}
