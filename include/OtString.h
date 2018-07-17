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
//	OtString
//

class OtStringClass;
typedef std::shared_ptr<OtStringClass> OtString;


//
//	OtStringClass
//

class OtStringClass : public OtPrimitiveClass
{
public:
	OtStringClass() {}
	OtStringClass(const std::string& string) { value = string; }

	operator bool() { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::tolower); return v == "true"; }
	operator long() { return std::stol(value); }
	operator size_t() { return (size_t) std::stol(value); }
	operator double() { return std::stof(value); }
	operator std::string() {return value; }

	std::string repr() { return '"' + value + '"'; }

	std::string add(const std::string& operand) { return value + operand; }

	bool equal(const std::string& operand) { return value == operand; }
	bool notEqual(const std::string& operand) { return value != operand; }
	bool greaterThan(const std::string& operand) { return value > operand; }
	bool lessThan(const std::string& operand) { return value < operand; }
	bool greaterEqual(const std::string& operand) { return value >= operand; }
	bool lessEqual(const std::string& operand) { return value <= operand; }

	size_t len() { return (size_t) value.length(); }

	std::string left(size_t count) { return value.substr(0, count); }
	std::string right(size_t count) { return value.substr(value.length() - count); }
	std::string mid(size_t start, size_t count) { return value.substr(start, count); }

	size_t find(const std::string& sub) { std::string::size_type p = value.find(sub); return p == std::string::npos ? -1 : (size_t) p; }

	std::string trim()
	{
		const auto begin = value.find_first_not_of(" \t\n\r\f\v");

		if (begin == std::string::npos)
			return "";

		else
			return value.substr(begin, value.find_last_not_of(" \t\n\r\f\v") - begin + 1);
	}

	std::string ltrim() { return value.substr(value.find_first_not_of(" \t\n\r\f\v")); }
	std::string rtrim() { return value.substr(0, value.find_last_not_of(" \t\n\r\f\v") + 1); }

	std::string compress()
	{
		auto result = trim();
		auto begin = result.find_first_of(" \t\n\r\f\v");

		while (begin != std::string::npos)
		{
			result.replace(begin, result.find_first_not_of(" \t\n\r\f\v", begin) - begin, " ");
			begin = result.find_first_of(" \t\n\r\f\v", begin + 1);
		}

		return result;
	}

	std::string lower() { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::tolower); return v; }
	std::string upper() { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::toupper); return v; }

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtStringClass>("String", OtPrimitiveClass::getMeta());

			type->set("__add__", OtFunctionCreate(&OtStringClass::add));

			type->set("__eq__", OtFunctionCreate(&OtStringClass::equal));
			type->set("__ne__", OtFunctionCreate(&OtStringClass::notEqual));
			type->set("__gt__", OtFunctionCreate(&OtStringClass::greaterThan));
			type->set("__lt__", OtFunctionCreate(&OtStringClass::lessThan));
			type->set("__ge__", OtFunctionCreate(&OtStringClass::greaterEqual));
			type->set("__le__", OtFunctionCreate(&OtStringClass::lessEqual));

			type->set("len", OtFunctionCreate(&OtStringClass::len));

			type->set("left", OtFunctionCreate(&OtStringClass::left));
			type->set("right", OtFunctionCreate(&OtStringClass::right));
			type->set("mid", OtFunctionCreate(&OtStringClass::mid));

			type->set("find", OtFunctionCreate(&OtStringClass::find));

			type->set("trim", OtFunctionCreate(&OtStringClass::trim));
			type->set("ltrim", OtFunctionCreate(&OtStringClass::ltrim));
			type->set("rtrim", OtFunctionCreate(&OtStringClass::rtrim));
			type->set("compress", OtFunctionCreate(&OtStringClass::compress));
			type->set("lower", OtFunctionCreate(&OtStringClass::lower));
			type->set("upper", OtFunctionCreate(&OtStringClass::upper));
		}

		return type;
	}

	// create a new object
	static OtString create(const std::string& value)
	{
		OtString string = std::make_shared<OtStringClass>(value);
		string->setType(getMeta());
		return string;
	}

private:
	std::string value;
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate(const std::string& value) { return OtStringClass::create(value); }
