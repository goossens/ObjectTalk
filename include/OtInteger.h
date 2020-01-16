//	ObjectTalk Scripting Language
//	Copyright 1993-2019 Johan A. Goossens
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
//	OtInteger
//

class OtIntegerClass;
typedef std::shared_ptr<OtIntegerClass> OtInteger;


//
//	OtInteger
//

class OtIntegerClass : public OtPrimitiveClass
{
public:
	OtIntegerClass() {}
	OtIntegerClass(long integer) { value = integer; }

	operator bool() { return value != 0; }
	operator long() { return value; }
	operator size_t() { return (size_t) value; }
	operator double() { return double(value); }
	operator std::wstring() {return std::to_wstring(value); }

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

	bool equal(long operand) { return value == operand; }
	bool notEqual(long operand) { return value != operand; }
	bool greaterThan(long operand) { return value > operand; }
	bool lessThan(long operand) { return value < operand; }
	bool greaterEqual(long operand) { return value >= operand; }
	bool lessEqual(long operand) { return value <= operand; }

	long negate() { return -value; }
	long abs() { return std::abs(value); }
	long sign() { return value / std::abs(value); }

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtIntegerClass>(L"Integer", OtPrimitiveClass::getMeta());

			type->set(L"__add__", OtFunctionCreate(&OtIntegerClass::add));
			type->set(L"__sub__", OtFunctionCreate(&OtIntegerClass::subtract));
			type->set(L"__mul__", OtFunctionCreate(&OtIntegerClass::multiply));
			type->set(L"__div__", OtFunctionCreate(&OtIntegerClass::divide));
			type->set(L"__mod__", OtFunctionCreate(&OtIntegerClass::modulo));
			type->set(L"__pow__", OtFunctionCreate(&OtIntegerClass::power));

			type->set(L"__inc__", OtFunctionCreate(&OtIntegerClass::increment));
			type->set(L"__dec__", OtFunctionCreate(&OtIntegerClass::decrement));

			type->set(L"__lshift__", OtFunctionCreate(&OtIntegerClass::shiftLeft));
			type->set(L"__rshift__", OtFunctionCreate(&OtIntegerClass::shiftRight));

			type->set(L"__band__", OtFunctionCreate(&OtIntegerClass::bitwiseAnd));
			type->set(L"__bor__", OtFunctionCreate(&OtIntegerClass::bitwiseOr));
			type->set(L"__bxor__", OtFunctionCreate(&OtIntegerClass::bitwiseXor));
			type->set(L"__bnot__", OtFunctionCreate(&OtIntegerClass::bitwiseNot));

			type->set(L"__eq__", OtFunctionCreate(&OtIntegerClass::equal));
			type->set(L"__ne__", OtFunctionCreate(&OtIntegerClass::notEqual));
			type->set(L"__gt__", OtFunctionCreate(&OtIntegerClass::greaterThan));
			type->set(L"__lt__", OtFunctionCreate(&OtIntegerClass::lessThan));
			type->set(L"__ge__", OtFunctionCreate(&OtIntegerClass::greaterEqual));
			type->set(L"__le__", OtFunctionCreate(&OtIntegerClass::lessEqual));

			type->set(L"__neg__", OtFunctionCreate(&OtIntegerClass::negate));

			type->set(L"abs", OtFunctionCreate(&OtIntegerClass::abs));
			type->set(L"sign", OtFunctionCreate(&OtIntegerClass::sign));
		}

		return type;
	}

	// create a new object
	static OtInteger create(long value)
	{
		OtInteger integer = std::make_shared<OtIntegerClass>(value);
		integer->setType(getMeta());
		return integer;
	}

private:
	long value;
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate(long value) { return OtIntegerClass::create(value); }
inline OtObject OtObjectCreate(size_t value) { return OtIntegerClass::create(value); }
