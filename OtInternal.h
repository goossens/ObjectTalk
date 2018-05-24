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
//  OtInternal
//

class OtInternal : public OtObject
{
public:
	// constructor
	OtInternal() {}
};


//
//  OtClass
//

class OtClass : public OtInternal
{
public:
	// constructors
	OtClass() {}
	OtClass(OtType t) { classType = t; }

	// access member information
	std::string getName() { return classType->getName(); }

	bool hasParent()
	{
		return classType->getParent() != nullptr;
	}
	OtValue getParent()
	{
		return OtValueCreate(classType->getParent());
	}

	// create a sub class
	OtValue subClass(const std::string& name)
	{
		return OtValueCreate(classType->subClass(name));
	}

	// see if class is kind of
	bool isKindOf(const std::string& className) { return classType->isKindOf(className); }

	// special member acccess (so we can add methods to metaclass via class
	OtValue set(const std::string& name, OtValue value) { return classType->set(name, value); }

	// call operator
	OtValue call(OtValue context, size_t count, OtValue* parameters)
	{
		// create new instance
		OtValue value = classType->instantiate();

		// run possible init function
		if (value->has("__init__"))
		{
			OtValue pars[count + 1];
			pars[0] = value;

			for (size_t c = 0; c < count; c++)
				pars[c + 1] = parameters[c];

			value->get("__init__")->execute(context, count + 1, pars);
		}

		return value;
	}

protected:
	OtType classType;
};


//
//
//  OtContextReference
//

class OtContextReference : public OtInternal
{
public:
	OtContextReference() {}
	OtContextReference(const std::string& m) { member = m; }

	OtValue deref(OtValue context, size_t, OtValue*)
	{
		return context->get(member);
	}

	OtValue assign(OtValue context, size_t, OtValue* parameters)
	{
		return context->set(member, *parameters);
	}

private:
	std::string member;
};


//
//
//  OtMemberReference
//

class OtMemberReference : public OtInternal
{
public:
	OtMemberReference() {}
	OtMemberReference(OtValue o, const std::string& m) { object = o; member = m; }

	OtValue deref()
	{
		OtValue result = object->get(member);

		if (result && result->isKindOf("Function"))
			return OtBoundFunctionCreate(object, result);

		else
			return result;
	}

	OtValue assign(OtValue value)
	{
		return object->set(member, value);
	}

private:
	OtValue object;
	std::string member;
};


//
//  OtArrayReference
//

class OtArrayReference : public OtInternal
{
public:
	OtArrayReference() {}
	OtArrayReference(std::shared_ptr<OtArray> a, size_t i) { array = a; index = i; }

	OtValue deref() { return (*array)[index]; }
	OtValue assign(OtValue value) { (*array)[index] = value; return value; }

private:
	std::shared_ptr<OtArray> array;
	size_t index;
};


//
//  OtDictReference
//

class OtDictReference : public OtInternal
{
public:
	OtDictReference() {}
	OtDictReference(std::shared_ptr<OtDict> d, const std::string& i) { dict = d; index = i; }

	OtValue deref() { return (*dict)[index]; }
	OtValue assign(OtValue value) { (*dict)[index] = value; return value; }

private:
	std::shared_ptr<OtDict> dict;
	std::string index;
};
