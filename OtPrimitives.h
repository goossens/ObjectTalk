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
//  OtPrimitive
//

class OtPrimitive : public OtObject
{
public:
	OtPrimitive() {}
};


//
//  OtBoolean
//

class OtBoolean : public OtPrimitive
{
public:
	OtBoolean() {}
	OtBoolean(bool boolean) { value = boolean; }

	operator bool() { return value; }
	operator long() { return value ? 1 : 0; }
	operator size_t() { return value ? 1 : 0; }
	operator double() { return value ? 1.0 : 0.0; }
	operator std::string() {return value ? "true" : "false"; }

	bool logicalAnd(bool operand) { return value && operand; }
	bool logicalOr(bool operand) { return value || operand; }
	bool logicalNot() { return !value; }

	bool equal(bool operand) { return value == operand; }
	bool notEqual(bool operand) { return value != operand; }

private:
	bool value;
};


//
//  OtInteger
//

class OtInteger : public OtPrimitive
{
public:
	OtInteger() {}
	OtInteger(long integer) { value = integer; }

	operator bool() { return value != 0; }
	operator long() { return value; }
	operator size_t() { return (size_t) value; }
	operator double() { return double(value); }
	operator std::string() {return std::to_string(value); }

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

private:
	long value;
};


//
//  OtReal
//

class OtReal : public OtPrimitive
{
public:
	OtReal() {}
	OtReal(double real) { value = real; }

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

private:
	double value;
};


//
//  OtString
//

class OtString : public OtPrimitive
{
public:
	OtString() {}
	OtString(const std::string& string) { value = string; }

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

private:
	std::string value;
};


//
//  OtFunction
//

class OtFunction : public OtPrimitive
{
public:
	// constructors
	OtFunction() {}

	OtFunction(size_t p, OtExecutable e)
	{
		parameterCount = p;
		executable = e;
		code = nullptr;
	}

	OtFunction(size_t p, OtCode c, const std::vector<std::string>& n)
	{
		parameterCount = p;
		executable = nullptr;
		code = c;
		argumentNames = n;
	}

	// execute function
	OtValue execute(OtValue context, size_t count, OtValue* parameters)
	{
		// sanity check
		if (parameterCount != SIZE_MAX && count != parameterCount)
			OT_EXCEPT("Function expects %d parameters, %d given", parameterCount, count);

		if (code)
		{
			// setup calling parameters
			OtValue local = OtValueCreate();
			local->setParent(context);

			// handle variadic call
			if (parameterCount == SIZE_MAX)
				local->set("args", OtArrayCreate(count, parameters));

			else
			{
				size_t c = 0;

				for (auto i = argumentNames.begin(); i < argumentNames.end(); i++)
				{
					std::string name = *i;
					local->set(*i, parameters[c++]);
				}
			}

			// run code
			return code->run(local);
		}

		// handle C(++) call
		else
			return executable(context, count, parameters);
	}

protected:
	size_t parameterCount;
	OtExecutable executable;
	OtCode code;
	std::vector<std::string> argumentNames;
};


//
//  OtBoundFunction
//

class OtBoundFunction : public OtPrimitive
{
public:
	// constructor
	OtBoundFunction() {}
	OtBoundFunction(OtValue o, OtValue f) { object = o; function = f; }

	// execute bound function
	OtValue execute(OtValue context, size_t count, OtValue* parameters)
	{
		OtValue pars[count + 1];
		pars[0] = object;

		if (count)
			std::copy(parameters, parameters + count, pars + 1);

		return OtTypeCast<OtFunction>(function)->execute(context, count + 1, pars);
	}

private:
	OtValue object;
	OtValue function;
};
