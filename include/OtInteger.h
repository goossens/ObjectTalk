//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtInteger
//

class OtIntegerClass;
typedef std::shared_ptr<OtIntegerClass> OtInteger;

class OtIntegerClass : public OtPrimitiveClass {
public:
	// constructors
	OtIntegerClass() = default;
	OtIntegerClass(long integer) : value(integer) {}

	// convertors
	operator bool() { return value != 0; }
	operator long() { return value; }
	operator size_t() { return (size_t) value; }
	operator double() { return double(value); }
	operator std::string() {return std::to_string(value); }

	// arithmetic
	long add(long operand) { return value + operand; }
	long subtract(long operand) { return value - operand; }
	long multiply(long operand) { return value * operand; }
	long divide(long operand) { return value / operand; }
	long modulo(long operand) { return value % operand; }
	long power(long operand) { return std::pow(value, operand); }

	long increment() { return ++value; }
	long decrement() { return --value; }

	long shiftLeft(long operand) { return value << operand; }
	long shiftRight(long operand) { return value >> operand; }

	long bitwiseAnd(long operand) { return value & operand; }
	long bitwiseOr(long operand) { return value | operand; }
	long bitwiseXor(long operand) { return value ^ operand; }
	long bitwiseNot() { return ~value; }

	// comparison
	bool equal(long operand) { return value == operand; }
	bool notEqual(long operand) { return value != operand; }
	bool greaterThan(long operand) { return value > operand; }
	bool lessThan(long operand) { return value < operand; }
	bool greaterEqual(long operand) { return value >= operand; }
	bool lessEqual(long operand) { return value <= operand; }

	// funtions
	long negate() { return -value; }
	long abs() { return std::abs(value); }
	long sign() { return value / std::abs(value); }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtIntegerClass>("Integer", OtPrimitiveClass::getMeta());

			type->set("__add__", OtFunctionCreate(&OtIntegerClass::add));
			type->set("__sub__", OtFunctionCreate(&OtIntegerClass::subtract));
			type->set("__mul__", OtFunctionCreate(&OtIntegerClass::multiply));
			type->set("__div__", OtFunctionCreate(&OtIntegerClass::divide));
			type->set("__mod__", OtFunctionCreate(&OtIntegerClass::modulo));
			type->set("__pow__", OtFunctionCreate(&OtIntegerClass::power));

			type->set("__inc__", OtFunctionCreate(&OtIntegerClass::increment));
			type->set("__dec__", OtFunctionCreate(&OtIntegerClass::decrement));

			type->set("__lshift__", OtFunctionCreate(&OtIntegerClass::shiftLeft));
			type->set("__rshift__", OtFunctionCreate(&OtIntegerClass::shiftRight));

			type->set("__band__", OtFunctionCreate(&OtIntegerClass::bitwiseAnd));
			type->set("__bor__", OtFunctionCreate(&OtIntegerClass::bitwiseOr));
			type->set("__bxor__", OtFunctionCreate(&OtIntegerClass::bitwiseXor));
			type->set("__bnot__", OtFunctionCreate(&OtIntegerClass::bitwiseNot));

			type->set("__eq__", OtFunctionCreate(&OtIntegerClass::equal));
			type->set("__ne__", OtFunctionCreate(&OtIntegerClass::notEqual));
			type->set("__gt__", OtFunctionCreate(&OtIntegerClass::greaterThan));
			type->set("__lt__", OtFunctionCreate(&OtIntegerClass::lessThan));
			type->set("__ge__", OtFunctionCreate(&OtIntegerClass::greaterEqual));
			type->set("__le__", OtFunctionCreate(&OtIntegerClass::lessEqual));

			type->set("__neg__", OtFunctionCreate(&OtIntegerClass::negate));

			type->set("abs", OtFunctionCreate(&OtIntegerClass::abs));
			type->set("sign", OtFunctionCreate(&OtIntegerClass::sign));
		}

		return type;
	}

	// create a new object
	static OtInteger create(long value) {
		OtInteger integer = std::make_shared<OtIntegerClass>(value);
		integer->setType(getMeta());
		return integer;
	}

private:
	long value {0};
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate(long value) { return OtIntegerClass::create(value); }
inline OtObject OtObjectCreate(size_t value) { return OtIntegerClass::create(value); }
