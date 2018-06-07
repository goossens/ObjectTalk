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


#pragma once


//
//  OtReal
//

class OtRealClass;
typedef std::shared_ptr<OtRealClass> OtReal;


//
//  OtRealClass
//

class OtRealClass : public OtObjectClass
{
public:
	OtRealClass() {}
	OtRealClass(double real) { value = real; }

	operator bool() { return value != 0.0; }
	operator long() { return long(value); }
	operator size_t() { return (size_t) value; }
	operator double() { return value; }
	operator std::string() {return std::to_string(value); }

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
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtRealClass>("Real", OtObjectClass::getMeta());

			type->set("__add__", OtFunctionCreate(&OtRealClass::add));
			type->set("__sub__", OtFunctionCreate(&OtRealClass::subtract));
			type->set("__mul__", OtFunctionCreate(&OtRealClass::multiply));
			type->set("__div__", OtFunctionCreate(&OtRealClass::divide));
			type->set("__pow__", OtFunctionCreate(&OtRealClass::power));

			type->set("__eq__", OtFunctionCreate(&OtRealClass::equal));
			type->set("__ne__", OtFunctionCreate(&OtRealClass::notEqual));
			type->set("__gt__", OtFunctionCreate(&OtRealClass::greaterThan));
			type->set("__lt__", OtFunctionCreate(&OtRealClass::lessThan));
			type->set("__ge__", OtFunctionCreate(&OtRealClass::greaterEqual));
			type->set("__le__", OtFunctionCreate(&OtRealClass::lessEqual));

			type->set("__neg__", OtFunctionCreate(&OtRealClass::negate));

			type->set("abs", OtFunctionCreate(&OtRealClass::abs));
			type->set("sign", OtFunctionCreate(&OtRealClass::sign));
			type->set("round", OtFunctionCreate(&OtRealClass::round));
			type->set("ceil", OtFunctionCreate(&OtRealClass::ceil));
			type->set("floor", OtFunctionCreate(&OtRealClass::floor));
			type->set("sin", OtFunctionCreate(&OtRealClass::sin));
			type->set("cos", OtFunctionCreate(&OtRealClass::cos));
			type->set("tan", OtFunctionCreate(&OtRealClass::tan));
			type->set("radians", OtFunctionCreate(&OtRealClass::radians));
			type->set("degrees", OtFunctionCreate(&OtRealClass::degrees));
		}

		return type;
	}

	// create a new object
	static OtReal create(double value)
	{
		OtReal real = std::make_shared<OtRealClass>(value);
		real->setType(getMeta());
		return real;
	}

private:
	double value;
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate(float value) { return OtRealClass::create(value); }
inline OtObject OtObjectCreate(double value) { return OtRealClass::create(value); }

