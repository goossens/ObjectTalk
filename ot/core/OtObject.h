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
#include <functional>
#include <string>
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
	virtual operator float() { return 0.0f; }
	virtual operator double() { return 0.0; }
	virtual operator std::string() { return ""; }
	virtual operator OtObject() { return OtObject(this); }

	// get object's JSON representation
	virtual std::string json() { return operator std::string(); }

	// describe object for debugging purposes
	virtual std::string describe();

	// member acccess
	virtual bool has(size_t id);
	virtual OtObject set(size_t id, OtObject value);
	virtual OtObject set(const char* name, OtObject value) { return set(OtIdentifier::create(name), value); }
	virtual OtObject set(const std::string& name, OtObject value) { return set(OtIdentifier::create(name), value); }
	virtual OtObject& get(size_t id);
	virtual void unset(size_t id);
	virtual void unsetAll();

	// member acccess by name
	virtual bool hasByName(const std::string& name) { return has(OtIdentifier::create(name)); }
	virtual OtObject setByName(const std::string& name, OtObject value) { return set(OtIdentifier::create(name), value); }
	virtual OtObject getByName(const std::string& name) { return get(OtIdentifier::create(name)); }
	virtual void unsetByName(const std::string& name) { return unset(OtIdentifier::create(name)); }

	bool hasMembers() { return members != nullptr; }

	// iterate through the members
	inline void eachMember(std::function<void(size_t, OtObject object)> callback) { members->each(callback); }
	inline void eachMemberID(std::function<void(size_t)> callback) { members->eachID(callback); }

	// comparison
	virtual bool operator==(OtObject operand);
	virtual bool operator<(OtObject operand);

	bool equal(OtObject operand) { return operator==(operand); }
	bool notEqual(OtObject operand) { return !(operator==(operand)); }

	// "call" object (count, parameters)
	virtual OtObject operator() (size_t, OtObject*) { return nullptr; }

	// get an object's class
	OtObject getClass();

	// get type definition
	static OtType getMeta();

protected:
	// object type
	OtType type;

	// reference count
	size_t referenceCount = 0;
	template <typename T> friend class OtObjectPointer;

	// members
	OtMembers* members = nullptr;
};


//
//	OtObjectDescribe
//

inline std::string OtObjectDescribe(OtObject object) {
	if (object) {
		auto type = std::string(OtIdentifier::name(object->getType()->getID()));
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
