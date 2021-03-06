//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtReal
//

class OtRealClass;
typedef std::shared_ptr<OtRealClass> OtReal;

class OtRealClass : public OtPrimitiveClass {
public:
	// constructors
	OtRealClass() = default;
	OtRealClass(double real) : value(real) {}

	// covertors
	operator bool() { return value != 0.0; }
	operator long() { return long(value); }
	operator size_t() { return (size_t) value; }
	operator double() { return value; }
	operator std::string() {return std::to_string(value); }

	// arithmetic
	double add(double operand) { return value + operand; }
	double subtract(double operand) { return value - operand; }
	double multiply(double operand) { return value * operand; }
	double divide(double operand) { return value / operand; }
	double power(double operand) { return std::pow(value, operand); }

	// comparison
	bool equal(double operand) { return value == operand; }
	bool notEqual(double operand) { return value != operand; }
	bool greaterThan(double operand) { return value > operand; }
	bool lessThan(double operand) { return value < operand; }
	bool greaterEqual(double operand) { return value >= operand; }
	bool lessEqual(double operand) { return value <= operand; }

	// functions
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
			type = OtTypeClass::create<OtRealClass>("Real", OtPrimitiveClass::getMeta());

			type->set("__add__", OtFunctionClass::create(&OtRealClass::add));
			type->set("__sub__", OtFunctionClass::create(&OtRealClass::subtract));
			type->set("__mul__", OtFunctionClass::create(&OtRealClass::multiply));
			type->set("__div__", OtFunctionClass::create(&OtRealClass::divide));
			type->set("__pow__", OtFunctionClass::create(&OtRealClass::power));

			type->set("__eq__", OtFunctionClass::create(&OtRealClass::equal));
			type->set("__ne__", OtFunctionClass::create(&OtRealClass::notEqual));
			type->set("__gt__", OtFunctionClass::create(&OtRealClass::greaterThan));
			type->set("__lt__", OtFunctionClass::create(&OtRealClass::lessThan));
			type->set("__ge__", OtFunctionClass::create(&OtRealClass::greaterEqual));
			type->set("__le__", OtFunctionClass::create(&OtRealClass::lessEqual));

			type->set("__neg__", OtFunctionClass::create(&OtRealClass::negate));

			type->set("abs", OtFunctionClass::create(&OtRealClass::abs));
			type->set("sign", OtFunctionClass::create(&OtRealClass::sign));
			type->set("round", OtFunctionClass::create(&OtRealClass::round));
			type->set("ceil", OtFunctionClass::create(&OtRealClass::ceil));
			type->set("floor", OtFunctionClass::create(&OtRealClass::floor));
			type->set("sin", OtFunctionClass::create(&OtRealClass::sin));
			type->set("cos", OtFunctionClass::create(&OtRealClass::cos));
			type->set("tan", OtFunctionClass::create(&OtRealClass::tan));
			type->set("radians", OtFunctionClass::create(&OtRealClass::radians));
			type->set("degrees", OtFunctionClass::create(&OtRealClass::degrees));
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
