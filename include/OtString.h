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
//	OtString
//

class OtStringClass;
typedef std::shared_ptr<OtStringClass> OtString;


//
//	OtStringClass
//

class OtStringClass : public OtPrimitiveClass {
public:
	OtStringClass() = default;
	OtStringClass(const std::wstring& string) : value(string) {}

	operator bool() { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::tolower); return v == L"true"; }
	operator long() { return std::stol(value); }
	operator size_t() { return (size_t) std::stol(value); }
	operator double() { return std::stof(value); }
	operator std::wstring() {return value; }

	std::wstring repr() { return OtTextToJSON(value); }

	std::wstring add(const std::wstring& operand) { return value + operand; }

	bool equal(const std::wstring& operand) { return value == operand; }
	bool notEqual(const std::wstring& operand) { return value != operand; }
	bool greaterThan(const std::wstring& operand) { return value > operand; }
	bool lessThan(const std::wstring& operand) { return value < operand; }
	bool greaterEqual(const std::wstring& operand) { return value >= operand; }
	bool lessEqual(const std::wstring& operand) { return value <= operand; }

	size_t len() { return (size_t) value.length(); }

	std::wstring left(size_t count) { return value.substr(0, count); }
	std::wstring right(size_t count) { return value.substr(value.length() - count); }
	std::wstring mid(size_t start, size_t count) { return value.substr(start, count); }

	size_t find(const std::wstring& sub) { std::wstring::size_type p = value.find(sub); return p == std::wstring::npos ? -1 : (size_t) p; }

	std::wstring trim() { return OtTextTrim(value); }
	std::wstring ltrim() { return OtTextLeftTrim(value); }
	std::wstring rtrim() { return OtTextRightTrim(value); }
	std::wstring compress() { return OtTextCompress(value); }

	std::wstring lower() { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::tolower); return v; }
	std::wstring upper() { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::toupper); return v; }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtStringClass>(L"String", OtPrimitiveClass::getMeta());

			type->set(L"__add__", OtFunctionCreate(&OtStringClass::add));

			type->set(L"__eq__", OtFunctionCreate(&OtStringClass::equal));
			type->set(L"__ne__", OtFunctionCreate(&OtStringClass::notEqual));
			type->set(L"__gt__", OtFunctionCreate(&OtStringClass::greaterThan));
			type->set(L"__lt__", OtFunctionCreate(&OtStringClass::lessThan));
			type->set(L"__ge__", OtFunctionCreate(&OtStringClass::greaterEqual));
			type->set(L"__le__", OtFunctionCreate(&OtStringClass::lessEqual));

			type->set(L"len", OtFunctionCreate(&OtStringClass::len));

			type->set(L"left", OtFunctionCreate(&OtStringClass::left));
			type->set(L"right", OtFunctionCreate(&OtStringClass::right));
			type->set(L"mid", OtFunctionCreate(&OtStringClass::mid));

			type->set(L"find", OtFunctionCreate(&OtStringClass::find));

			type->set(L"trim", OtFunctionCreate(&OtStringClass::trim));
			type->set(L"ltrim", OtFunctionCreate(&OtStringClass::ltrim));
			type->set(L"rtrim", OtFunctionCreate(&OtStringClass::rtrim));
			type->set(L"compress", OtFunctionCreate(&OtStringClass::compress));
			type->set(L"lower", OtFunctionCreate(&OtStringClass::lower));
			type->set(L"upper", OtFunctionCreate(&OtStringClass::upper));
		}

		return type;
	}

	// create a new object
	static OtString create(const std::wstring& value) {
		OtString string = std::make_shared<OtStringClass>(value);
		string->setType(getMeta());
		return string;
	}

private:
	std::wstring value {L""};
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate(const std::wstring& value) { return OtStringClass::create(value); }
