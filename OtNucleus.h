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
//  Include files
//

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "OtException.h"


//
//  OtType
//

class OtMetaClass;
typedef std::shared_ptr<OtMetaClass> OtType;


//
//  OtValue
//

class OtObject;
typedef std::shared_ptr<OtObject> OtValue;


//
//  OtConstructor
//

typedef std::function<OtValue ()> OtConstructor;


//
//  OtExecutable
//

typedef std::function<OtValue (OtValue context, size_t count, OtValue* parameters)> OtExecutable;


//
//  OtMetaClass 
//

class OtMetaClass : public std::enable_shared_from_this<OtMetaClass>
{
public:
	// constructor
	OtMetaClass(const std::string& n, OtType p, OtConstructor c=nullptr)
	{
		name = n;
		parent = p;
		constructor = c ? c : parent->constructor;
	}

	// create new instance
	OtValue instantiate();

	// create a sub-metaclass
	OtType subClass(const std::string& n, OtConstructor c=nullptr)
	{
		return std::make_shared<OtMetaClass>(n, getSharedPtr(), c);
	}

	// get shared pointer
	OtType getSharedPtr() { return shared_from_this(); }

	// see if meta class is kind of
	bool isKindOf(const std::string& className)
	{
		for (auto p = getSharedPtr(); p; p = p->parent)
			if (p->name == className)
				return true;

		return false;
	}

	bool isKindOf(OtType metaclass)  { return isKindOf(metaclass->name); }

	// get information
	std::string getName() { return name; }
	OtType getParent() { return parent; }

	// member access
	bool has(const std::string& name) { return get(name) != nullptr; }
	OtValue set(const std::string& name, OtValue value) { members[name] = value; return value; }
	OtValue get(const std::string& name) { return members.count(name) ? members[name] : nullptr; }

private:
	// meta class attributes
	std::string name;
	OtType parent;
	std::map<std::string, OtValue> members;
	OtConstructor constructor;
};


//
//  Create a new type
//

template <class T>
OtType OtTypeCreate(const std::string& name, OtType parent=nullptr)
{
	return std::make_shared<OtMetaClass>(name, parent, []() { return std::make_shared<T>(); });
}


//
//  Type cast
//

template <class T>
std::shared_ptr<T> OtTypeCast(OtValue value)
{
	return std::dynamic_pointer_cast<T>(value);
}


//
//  OtObject
//

class OtObject : public std::enable_shared_from_this<OtObject>
{
public:
	// constructor/destructor
	OtObject() {}
	virtual ~OtObject() {}

	// type access
	void setType(OtType t) { type = t; }
	OtType getType() { return type; }
	std::string getTypeName() { return type->getName(); }

	// see if object "is kind of"
	bool isKindOf(const std::string& className) { return type->isKindOf(className); }
	bool isKindOf(OtType t)  { return type->isKindOf(t); }

	// default conversion operators
	virtual operator bool() { return false; }
	virtual operator long() { return 0; }
	virtual operator size_t() { return 0; }
	virtual operator double() { return 0.0; }
	virtual operator std::string() { return ""; }
	virtual operator OtValue() { return shared_from_this(); }

	virtual std::string repr() { return operator std::string(); }

	// get shared pointer
	OtValue getSharedPtr() { return shared_from_this(); }

	// parent access
	void setParent(OtValue v) { parent = v; }
	OtValue getParent() { return parent; }

	// member acccess
	bool has(const std::string& name)
	{
		if (members && members->count(name))
			return true;

		if (parent && parent->has(name))
			return true;

		for (auto t = type; t; t = t->getParent())
			if (t->has(name))
				return true;

		return false;
	}

	virtual OtValue set(const std::string& name, OtValue value)
	{
		if (!members)
			members = std::make_shared<std::map<std::string, OtValue>>();

		members->operator [] (name) = value;
		return value;
	}

	virtual OtValue get(const std::string& name)
	{
		if (members && members->count(name))
			return members->operator [] (name);

		if (parent && parent->has(name))
			return parent->get(name);

		for (auto t = type; t; t = t->getParent())
			if (t->has(name))
				return t->get(name);

		OT_EXCEPT("Unknown member [%s] in class [%s]", name.c_str(), type->getName().c_str());
		return nullptr;
	}

	// "execute" object (context, count, parameters)
	virtual OtValue execute(OtValue, size_t, OtValue*) { return nullptr; }

	// cast shared pointer to specified type
	template <typename T>
	std::shared_ptr<T> cast()
	{
		return std::static_pointer_cast<T>(shared_from_this());
	}

protected:
	// object type
	OtType type;

	// members
	std::shared_ptr<std::map<std::string, OtValue>> members;

	// parent in chain
	OtValue parent;
};


//
//  Create a new value
//

inline OtValue OtValueCreate(OtType type, OtValue object)
{
	if (object)
		object->setType(type);

	else
		object = type->instantiate();

	return object;
}


//
//  OtMetaClass::instantiate (defined here due to circular references)
//

inline OtValue OtMetaClass::instantiate()
{
	OtValue object = constructor();
	object->setType(shared_from_this());
	return object;
}


//
//  OtInstruction
//

class OtInstruction
{
public:
	typedef enum
	{
		OT_OPCODE_PUSH,
		OT_OPCODE_POP,
		OT_OPCODE_POP_COUNT,
		OT_OPCODE_DUP,
		OT_OPCODE_SWAP,
		OT_OPCODE_JUMP,
		OT_OPCODE_JUMP_TRUE,
		OT_OPCODE_JUMP_FALSE,
		OT_OPCODE_EXIT,
		OT_OPCODE_PUSH_CONTEXT,
		OT_OPCODE_POP_CONTEXT,
		OT_OPCODE_SET,
		OT_OPCODE_GET,
		OT_OPCODE_METHOD
	} OtOpcode;

	OtInstruction(OtOpcode o) { opcode = o; integer = 0; value = nullptr; }
	OtInstruction(OtOpcode o, long i)  { opcode = o; integer = i; value = nullptr; }
	OtInstruction(OtOpcode o, const std::string& s)  { opcode = o; string = s; integer = 0; value = nullptr; }
	OtInstruction(OtOpcode o, const std::string& s, long i)  { opcode = o; string = s; integer = i; value = nullptr; }
	OtInstruction(OtOpcode o, OtValue v)  { opcode = o; integer = 0; value = v; }

	OtOpcode opcode;
	long integer;
	std::string string;
	OtValue value;
};


//
//  OtInstructions
//

class OtInstructions : public std::vector<OtInstruction>
{
public:
	// add instructions to code
	void push(OtValue value) { push_back(OtInstruction(OtInstruction::OT_OPCODE_PUSH, value)); }
	void pop() { push_back(OtInstruction(OtInstruction::OT_OPCODE_POP)); }
	void pop(size_t count) { push_back(OtInstruction(OtInstruction::OT_OPCODE_POP_COUNT, count)); }
	void dup() { push_back(OtInstruction(OtInstruction::OT_OPCODE_DUP)); }
	void swap() { push_back(OtInstruction(OtInstruction::OT_OPCODE_SWAP)); }
	void jump(size_t offset) { push_back(OtInstruction(OtInstruction::OT_OPCODE_JUMP, offset)); }
	void jumpTrue(size_t offset) { push_back(OtInstruction(OtInstruction::OT_OPCODE_JUMP_TRUE, offset)); }
	void jumpFalse(size_t offset) { push_back(OtInstruction(OtInstruction::OT_OPCODE_JUMP_FALSE, offset)); }
	void method(const std::string& name, size_t count) { push_back(OtInstruction(OtInstruction::OT_OPCODE_METHOD, name, count)); }
	void exit() { push_back(OtInstruction(OtInstruction::OT_OPCODE_EXIT)); }
	void set(const std::string& name) { push_back(OtInstruction(OtInstruction::OT_OPCODE_SET, name)); }
	void get(const std::string& name) { push_back(OtInstruction(OtInstruction::OT_OPCODE_GET, name)); }
	void pushContext() { push_back(OtInstruction(OtInstruction::OT_OPCODE_PUSH_CONTEXT)); }
	void popContext() { push_back(OtInstruction(OtInstruction::OT_OPCODE_POP_CONTEXT)); }

	// patch jump instruction
	void patch(size_t location) { at(location).integer = size(); }

	OtValue run(OtValue context)
	{
		// program counter
		size_t pc = 0;

		// stack and pointer
		std::vector<OtValue> stack;
		OtValue* sp;

		// local variables
		OtValue value;
		size_t count;

		// execute all code
		while (pc < size())
		{
			switch (at(pc).opcode)
			{
				case OtInstruction::OT_OPCODE_PUSH:
					stack.push_back(at(pc).value);
					break;

				case OtInstruction::OT_OPCODE_POP:
					stack.pop_back();
					break;

				case OtInstruction::OT_OPCODE_POP_COUNT:
					stack.resize(stack.size() - at(pc).integer);
					break;

				case OtInstruction::OT_OPCODE_DUP:
					stack.push_back(stack.back());
					break;

				case OtInstruction::OT_OPCODE_SWAP:
					std::swap(stack[stack.size() - 1], stack[stack.size() - 2]);
					break;

				case OtInstruction::OT_OPCODE_JUMP:
					pc = at(pc).integer - 1;
					break;

				case OtInstruction::OT_OPCODE_JUMP_TRUE:
					value = stack.back();
					stack.pop_back();

					if (value->operator bool())
						pc = at(pc).integer - 1;

					break;

				case OtInstruction::OT_OPCODE_JUMP_FALSE:
					value = stack.back();
					stack.pop_back();

					if (!value->operator bool())
						pc = at(pc).integer - 1;

					break;

				case OtInstruction::OT_OPCODE_EXIT:
					pc = size();
					break;

				case OtInstruction::OT_OPCODE_SET:
					value = stack.back();
					stack.pop_back();
					context->set(at(pc).string, value);
					break;

				case OtInstruction::OT_OPCODE_GET:
					stack.push_back(context->get(at(pc).string));
					break;

				case OtInstruction::OT_OPCODE_PUSH_CONTEXT:
					value = stack.back();
					stack.pop_back();
					value->setParent(context);

					stack.push_back(context);
					context = value;
					break;

				case OtInstruction::OT_OPCODE_POP_CONTEXT:
					context = stack.back();
					stack.pop_back();
					break;

				case OtInstruction::OT_OPCODE_METHOD:
					// get target object and call method
					count = at(pc).integer + 1;
					sp = &stack[stack.size() - count];
					value = sp[0]->get(at(pc).string);

					if (!value)
						OT_EXCEPT("Can't call method [%s] on NULL object", at(pc).string.c_str());

					value = value->execute(context, count, sp);

					// clean up stack
					stack.resize(stack.size() - count);
					stack.push_back(value);
					break;
			}

			pc++;
		}

		return stack.back();
	}
};


//
//  OtCode
//

typedef std::shared_ptr<OtInstructions> OtCode;
inline OtCode OtCodeCreate() { return std::make_shared<OtInstructions>(); }
