//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "OtMembers.h"
#include "OtObjectPointer.h"
#include "OtType.h"


//
//	Forward declarations
//

class OtClassClass;
using OtClass = OtObjectPointer<OtClassClass>;


//
//	OtObject
//

class OtObjectClass;
using OtObject = OtObjectPointer<OtObjectClass>;

class OtObjectClass {
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
	virtual operator std::filesystem::path() { return std::filesystem::path(operator std::string()); }
	virtual operator OtObject() { return OtObject(this); }

	// get object's JSON representation
	virtual std::string json() { return operator std::string(); }

	// describe object for debugging purposes
	virtual std::string describe();

	// member acccess
	virtual bool has(size_t selector);
	virtual OtObject set(size_t selector, OtObject value);
	virtual OtObject set(const char* name, OtObject value) { return set(OtSelector::create(name), value); }
	virtual OtObject set(const std::string& name, OtObject value) { return set(OtSelector::create(name), value); }
	virtual OtObject& get(size_t selector);
	virtual void unset(size_t selector);
	virtual void unsetAll() { members = nullptr; }

	// member acccess by name
	virtual bool hasByName(const std::string& name) { return has(OtSelector::create(name)); }
	virtual OtObject setByName(const std::string& name, OtObject value) { return set(OtSelector::create(name), value); }
	virtual OtObject getByName(const std::string& name) { return get(OtSelector::create(name)); }
	virtual void unsetByName(const std::string& name)  { return unset(OtSelector::create(name)); }

	bool hasMembers() { return members != nullptr; }
	void getMemberNames(std::vector<std::string_view>& names);

	// comparison
	virtual bool operator==(OtObject operand);
	virtual bool operator<(OtObject operand);

	bool equal(OtObject operand) { return operator==(operand); }
	bool notEqual(OtObject operand) { return !(operator==(operand)); }

	// "call" object (count, parameters)
	virtual OtObject operator () (size_t, OtObject*) { return nullptr; }

	// get an object's class
	OtObject getClass();

	// support observer pattern through callbacks
	size_t attach(std::function<void(void)> callback);
	void detach(size_t id);
	void notify();

	// get type definition
	static OtType getMeta();

protected:
	// object type
	OtType type;

	// reference count
	size_t referenceCount{0};
	template <typename T> friend class OtObjectPointer;

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
		auto type = object->getType()->getName();
		auto description = object->describe();

		if (type == description) {
			return type;

		} else {
			return type + " " + description;
		}

	} else {
		return "null";
	}
}
