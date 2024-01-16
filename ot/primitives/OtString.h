//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <string>
#include <string_view>

#include "OtPrimitive.h"
#include "OtText.h"


//
//	OtString
//

class OtStringClass;
using OtString = OtObjectPointer<OtStringClass>;

class OtStringClass : public OtPrimitiveClass {
	friend class OtStringIteratorClass;

public:
	// constructors
	OtStringClass() = default;
	OtStringClass(const char* string) : value(string) {}
	OtStringClass(const std::string& string) : value(string) {}
	OtStringClass(const std::string_view string) : value(string) {}

	// convertors
	operator bool() override { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::tolower); return v == "true"; }
	operator int() override { try { return std::stoi(value); } catch(...) { return 0; }}
	operator int64_t() override { try { return std::stol(value); } catch(...) { return 0; }}
	operator size_t() override { try { return (size_t) std::stol(value); } catch(...) { return 0; }}
	operator float() override { try { return std::stof(value); } catch(...) { return 0.0; }}
	operator double() override { try { return std::stod(value); } catch(...) { return 0.0; }}
	operator std::string() override {return value; }

	std::string json() override { return OtText::toJSON(value); }

	// debugging support
	std::string describe() override { return "\"" + (len() > 32 ? left(32) + "...\"" : value) + "\""; }

	// comparison
	bool operator==(OtObject operand) override { return value == operand->operator std::string(); }
	bool operator<(OtObject operand) override { return value < operand->operator std::string(); }

	bool equal(const std::string& operand) { return value == operand; }
	bool notEqual(const std::string& operand) { return value != operand; }
	bool greaterThan(const std::string& operand) { return value > operand; }
	bool lessThan(const std::string& operand) { return value < operand; }
	bool greaterEqual(const std::string& operand) { return value >= operand; }
	bool lessEqual(const std::string& operand) { return value <= operand; }

	int64_t casecmp(const std::string& operand) { return OtText::caseCmp(value, operand); }

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
	size_t len() { return OtText::len(value); }

	std::string left(size_t count) { return OtText::left(value, count); }
	std::string right(size_t count) { return OtText::right(value, count); }
	std::string mid(size_t start, size_t count) { return OtText::mid(value, start, count); }

	size_t find(const std::string& sub) { std::string::size_type p = value.find(sub); return p == std::string::npos ? -1 : (size_t) p; }
	bool startsWith(const std::string& sub) { return OtText::startsWith(value, sub); }
	bool contains(const std::string& sub) { return OtText::contains(value, sub); }

	std::string trim() { return OtText::trim(value); }
	std::string ltrim() { return OtText::leftTrim(value); }
	std::string rtrim() { return OtText::rightTrim(value); }
	std::string compress() { return OtText::compressWhitespace(value); }

	std::string lower() { return OtText::lower(value); }
	std::string upper() { return OtText::upper(value); }

	OtObject split(const std::string& delimiter);

	OtObject format(size_t count, OtObject* parameters);

	// get type definition
	static OtType getMeta();

private:
	std::string value = "";
};
