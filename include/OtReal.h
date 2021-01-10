//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
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
//	OtReal
//

class OtRealClass;
typedef std::shared_ptr<OtRealClass> OtReal;


//
//	OtRealClass
//

class OtRealClass : public OtPrimitiveClass {
public:
	OtRealClass() = default;
	OtRealClass(double real) : value(real) {}

	operator bool() { return value != 0.0; }
	operator long() { return long(value); }
	operator size_t() { return (size_t) value; }
	operator double() { return value; }
	operator std::wstring() {return std::to_wstring(value); }

	double add(double operand) { return value + operand; }
	double subtract(double operand) { return value - operand; }
	double multiply(double operand) { return value * operand; }
	double divide(double operand) { return value / operand; }
	double power(double operand) { return std::pow(value, operand); }

	bool equal(double operand) { return value == operand; }
	bool notEqual(double operand) { return value != operand; }
	bool greaterThan(double operand) { return value > operand; }
	bool lessThan(double operand) { return value < operand; }
	bool greaterEqual(double operand) { return value >= operand; }
	bool lessEqual(double operand) { return value <= operand; }

	double negate() { return -value; }
	double abs() { return std::abs(value); }
	double sign() { return value / std::abs(value); }
	double round() { return std::round(value); }
	double ceil() { return std::ceil(value); }
	double floor() { return std::floor(value); }
	double sin() { return std::sin(value); }
	double cos() { return std::cos(value); }
	double tan() { return std::tan(value); }
	double radians() { return value * M_PI / 180.0; }
	double degrees() { return value / M_PI * 190.0; }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtRealClass>(L"Real", OtPrimitiveClass::getMeta());

			type->set(L"__add__", OtFunctionCreate(&OtRealClass::add));
			type->set(L"__sub__", OtFunctionCreate(&OtRealClass::subtract));
			type->set(L"__mul__", OtFunctionCreate(&OtRealClass::multiply));
			type->set(L"__div__", OtFunctionCreate(&OtRealClass::divide));
			type->set(L"__pow__", OtFunctionCreate(&OtRealClass::power));

			type->set(L"__eq__", OtFunctionCreate(&OtRealClass::equal));
			type->set(L"__ne__", OtFunctionCreate(&OtRealClass::notEqual));
			type->set(L"__gt__", OtFunctionCreate(&OtRealClass::greaterThan));
			type->set(L"__lt__", OtFunctionCreate(&OtRealClass::lessThan));
			type->set(L"__ge__", OtFunctionCreate(&OtRealClass::greaterEqual));
			type->set(L"__le__", OtFunctionCreate(&OtRealClass::lessEqual));

			type->set(L"__neg__", OtFunctionCreate(&OtRealClass::negate));

			type->set(L"abs", OtFunctionCreate(&OtRealClass::abs));
			type->set(L"sign", OtFunctionCreate(&OtRealClass::sign));
			type->set(L"round", OtFunctionCreate(&OtRealClass::round));
			type->set(L"ceil", OtFunctionCreate(&OtRealClass::ceil));
			type->set(L"floor", OtFunctionCreate(&OtRealClass::floor));
			type->set(L"sin", OtFunctionCreate(&OtRealClass::sin));
			type->set(L"cos", OtFunctionCreate(&OtRealClass::cos));
			type->set(L"tan", OtFunctionCreate(&OtRealClass::tan));
			type->set(L"radians", OtFunctionCreate(&OtRealClass::radians));
			type->set(L"degrees", OtFunctionCreate(&OtRealClass::degrees));
		}

		return type;
	}

	// create a new object
	static OtReal create(double value) {
		OtReal real = std::make_shared<OtRealClass>(value);
		real->setType(getMeta());
		return real;
	}

private:
	double value {0.0};
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate(float value) { return OtRealClass::create(value); }
inline OtObject OtObjectCreate(double value) { return OtRealClass::create(value); }
