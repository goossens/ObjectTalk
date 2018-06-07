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
//  OtBoolean
//

class OtBooleanClass;
typedef std::shared_ptr<OtBooleanClass> OtBoolean;


//
//  OtBooleanClass
//

class OtBooleanClass : public OtPrimitiveClass
{
public:
	OtBooleanClass() {}
	OtBooleanClass(bool boolean) { value = boolean; }

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

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtBooleanClass>("Boolean", OtPrimitiveClass::getMeta());

			type->set("__and__", OtFunctionCreate(&OtBooleanClass::logicalAnd));
			type->set("__or__", OtFunctionCreate(&OtBooleanClass::logicalOr));
			type->set("__not__", OtFunctionCreate(&OtBooleanClass::logicalNot));

			type->set("__eq__", OtFunctionCreate(&OtBooleanClass::equal));
			type->set("__ne__", OtFunctionCreate(&OtBooleanClass::notEqual));
		}

		return type;
	}

	// create a new object
	static OtBoolean create(bool value)
	{
		OtBoolean boolean = std::make_shared<OtBooleanClass>(value);
		boolean->setType(getMeta());
		return boolean;
	}

private:
	bool value;
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate(bool value) { return OtBooleanClass::create(value); }
