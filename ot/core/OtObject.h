//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "OtMembers.h"
#include "OtType.h"


//
//	Forward references
//

class OtClassClass;
typedef std::shared_ptr<OtClassClass> OtClass;


//
//	OtObject
//

class OtObjectClass;
typedef std::shared_ptr<OtObjectClass> OtObject;

class OtObjectClass : public std::enable_shared_from_this<OtObjectClass> {
public:
	// destructor
	virtual ~OtObjectClass();

	// type access
	void setType(OtType t) { type = t; }
	OtType getType() { return type; }

	// see if object "is kind of"
	bool isKindOf(const std::string& className) { return type->isKindOf(className); }

	// expect object "is kind of" (exception is raised in case it is not)
	void expectKindOf(const std::string& className);

	// default conversion operators
	virtual operator bool() { return false; }
	virtual operator int() { return 0; }
	virtual operator int64_t() { return 0; }
	virtual operator size_t() { return 0; }
	virtual operator float() { return 0.0; }
	virtual operator double() { return 0.0; }
	virtual operator std::string() { return ""; }
	virtual operator OtObject() { return shared_from_this(); }

	// get object's JSON representation
	virtual std::string json() { return operator std::string(); }

	// describe object for debugging purposes
	virtual std::string describe();

	// get shared pointer
	OtObject shared() { return shared_from_this(); }

	// get casted shared pointer
	template <typename CLASS>
	std::shared_ptr<CLASS> cast() { return std::dynamic_pointer_cast<CLASS>(shared_from_this()); }

	// member acccess
	virtual bool has(size_t selector);
	virtual OtObject set(size_t selector, OtObject value);
	virtual OtObject set(const char* name, OtObject value) { return set(OtSelector::create(name), value); }
	virtual OtObject set(const std::string& name, OtObject value) { return set(OtSelector::create(name), value); }
	virtual OtObject get(size_t selector);
	virtual void unset(size_t selector);
	virtual void unsetAll() { members = nullptr; }

	// member acccess by name
	virtual bool hasByName(const std::string& name) { return has(OtSelector::create(name)); }
	virtual OtObject setByName(const std::string& name, OtObject value) { return set(OtSelector::create(name), value); }
	virtual OtObject getByName(const std::string& name) { return get(OtSelector::create(name)); }
	virtual void unsetByName(const std::string& name)  { return unset(OtSelector::create(name)); }

	bool hasMembers() { return members != nullptr; }
	std::vector<std::string> getMemberNames();

	// comparison
	virtual bool operator == (OtObject operand);
	virtual bool operator < (OtObject operand);

	bool equal(OtObject operand) { return operator == (operand); }
	bool notEqual(OtObject operand) { return !(operator == (operand)); }

	// "call" object (count, parameters)
	virtual OtObject operator () (size_t, OtObject*) { return nullptr; }

	// get an object's class
	OtClass getClass();

	// support observer pattern through callbacks
	size_t attach(std::function<void(void)> callback);
	void detach(size_t id);
	void notify();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtObject create();

protected:
	// object type
	OtType type;

	// members
	OtMembers* members = nullptr;

	// observers
	struct OtObserver {
		OtObserver(size_t i, std::function<void(void)> c) : id(i), callback(c) {}
		size_t id;
		std::function<void(void)> callback;
	};

	std::vector<OtObserver>* observers = nullptr;
};


//
//	OtObjectDescribe
//

inline std::string OtObjectDescribe(OtObject object) {
	if (object) {
		return object->getType()->getName() + " " + object->describe();

	} else {
		return "null";
	}
}
