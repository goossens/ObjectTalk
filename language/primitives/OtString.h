//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	inline operator bool() override { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::tolower); return v == "true"; }
	inline operator int() override { try { return std::stoi(value, nullptr, 0); } catch(...) { return 0; }}
	inline operator int64_t() override { try { return std::stol(value, nullptr, 0); } catch(...) { return 0; }}
	inline operator size_t() override { try { return (size_t) std::stol(value, nullptr, 0); } catch(...) { return 0; }}
	inline operator float() override { try { return std::stof(value); } catch(...) { return 0.0f; }}
	inline operator double() override { try { return std::stod(value); } catch(...) { return 0.0; }}
	inline operator std::string() override {return value; }

	inline std::string json() override { return OtText::toJSON(value); }

	// debugging support
	inline std::string describe() override { return "\"" + (len() > 32 ? left(32) + "...\"" : value) + "\""; }

	// comparison
	inline bool operator==(OtObject operand) override { return value == operand->operator std::string(); }
	inline bool operator<(OtObject operand) override { return value < operand->operator std::string(); }

	inline bool equal(const std::string& operand) { return value == operand; }
	inline bool notEqual(const std::string& operand) { return value != operand; }
	inline bool greaterThan(const std::string& operand) { return value > operand; }
	inline bool lessThan(const std::string& operand) { return value < operand; }
	inline bool greaterEqual(const std::string& operand) { return value >= operand; }
	inline bool lessEqual(const std::string& operand) { return value <= operand; }

	int64_t casecmp(const std::string& operand) { return OtText::caseCmp(value, operand); }

	// access string members (i.e. code points)
	std::string getEntry(size_t index);
	std::string setEntry(size_t index, const std::string& string);

	// support indexing
	OtObject index(size_t index);

	// start iterator
	OtObject iterate();

	// "arithmetic"
	inline std::string add(const std::string& operand) { return value + operand; }

	// functions
	inline size_t len() { return OtText::len(value); }

	inline std::string left(size_t count) { return OtText::left(value, count); }
	inline std::string right(size_t count) { return OtText::right(value, count); }
	inline std::string mid(size_t start, size_t count) { return OtText::mid(value, start, count); }

	inline size_t find(const std::string& sub) { std::string::size_type p = value.find(sub); return p == std::string::npos ? -1 : (size_t) p; }
	inline bool startsWith(const std::string& sub) { return OtText::startsWith(value, sub); }
	inline bool contains(const std::string& sub) { return OtText::contains(value, sub); }

	inline std::string trim() { return OtText::trim(value); }
	inline std::string ltrim() { return OtText::leftTrim(value); }
	inline std::string rtrim() { return OtText::rightTrim(value); }
	inline std::string compress() { return OtText::compressWhitespace(value); }

	inline std::string lower() { return OtText::lower(value); }
	inline std::string upper() { return OtText::upper(value); }

	int toCodePoint();
	std::string fromCodePoint(int codepoint);

	OtObject split(const std::string& delimiter);

	OtObject format(size_t count, OtObject* parameters);

	// get type definition
	static OtType getMeta();

private:
	std::string value = "";
};
