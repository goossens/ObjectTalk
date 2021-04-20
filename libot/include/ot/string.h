//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>

#include "primitive.h"
#include "text.h"


//
//	OtString
//

class OtStringClass;
typedef std::shared_ptr<OtStringClass> OtString;

class OtStringClass : public OtPrimitiveClass {
	friend class OtStringIteratorClass;

public:
	// constructors
	OtStringClass() = default;
	OtStringClass(const std::string& string) : value(string) {}

	// convertors
	operator bool() { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::tolower); return v =="true"; }
	operator int() { return std::stoi(value); }
	operator long() { try { return std::stol(value); } catch(...) { return 0; }}
	operator size_t() { return (size_t) std::stol(value); }
	operator double() { try { return std::stof(value); } catch(...) { return 0.0; }}
	operator std::string() {return value; }

	std::string json() { return OtText::toJSON(value); }

	// debugging support
	std::string describe() { return "\"" + (len() > 32 ? left(32) + "...\"" : value) + "\""; }

	// comparison
	bool operator ==(OtObject operand) { return value == operand->operator std::string(); }
	bool operator <(OtObject operand) { return value < operand->operator std::string(); }

	bool equal(const std::string& operand) { return value == operand; }
	bool notEqual(const std::string& operand) { return value != operand; }
	bool greaterThan(const std::string& operand) { return value > operand; }
	bool lessThan(const std::string& operand) { return value < operand; }
	bool greaterEqual(const std::string& operand) { return value >= operand; }
	bool lessEqual(const std::string& operand) { return value <= operand; }

	long casecmp(const std::string& operand) { return OtText::caseCmp(value, operand); }

	// access string members (i.e. code points)
	std::string getEntry(size_t index);
	std::string setEntry(size_t index, const std::string& string);

	// support indexing
	OtObject index(size_t index);

	// start iterator
	OtObject iterate();

	// "arithmetic"
	std::string add(const std::string& operand) { return value + operand; }

	// functions
	size_t len() { return (size_t) OtText::len(value); }

	std::string left(size_t count) { return OtText::left(value, count); }
	std::string right(size_t count) { return OtText::right(value, count); }
	std::string mid(size_t start, size_t count) { return OtText::mid(value, start, count); }

	size_t find(const std::string& sub) { std::string::size_type p = value.find(sub); return p == std::string::npos ? -1 : (size_t) p; }
	bool startsWith(const std::string& sub) { return OtText::startsWith(value, sub); }
	bool contains(const std::string& sub) { return OtText::contains(value, sub); }

	std::string trim() { return OtText::trim(value); }
	std::string ltrim() { return OtText::leftTrim(value); }
	std::string rtrim() { return OtText::rightTrim(value); }
	std::string compress() { return OtText::compress(value); }

	std::string lower() { return OtText::lower(value); }
	std::string upper() { return OtText::upper(value); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtString create(const std::string& value);

private:
	std::string value = "";
};
