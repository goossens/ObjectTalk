//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	// type access
	inline void setType(OtType t) { type = t; }
	inline OtType getType() { return type; }
	inline std::string getTypeName() { return type->getName(); }

	// see if object "is kind of"
	inline bool isKindOf(const std::string& className) { return type->isKindOf(className); }

	// expect object "is kind of" (exception is raised in case it is not)
	void expectKindOf(const std::string& className);

	// default conversion operators
	virtual inline operator bool() { return false; }
	virtual inline operator int() { return 0; }
	virtual inline operator int64_t() { return 0; }
	virtual inline operator size_t() { return 0; }
	virtual inline operator float() { return 0.0f; }
	virtual inline operator double() { return 0.0; }
	virtual inline operator std::string() { return ""; }
	virtual inline operator OtObject() { return OtObject(this); }

	// get object's JSON representation
	virtual inline std::string json() { return operator std::string(); }

	// describe object for debugging purposes
	virtual inline std::string describe() { return ""; }

	// member access
	virtual bool has(OtID id);
	virtual OtObject set(OtID id, OtObject value);
	virtual OtObject get(OtID id);
	virtual void unset(OtID id);
	virtual void unsetAll();

	inline OtObject set(const char* name, OtObject value) { return set(OtIdentifier::create(name), value); }
	inline OtObject set(const std::string& name, OtObject value) { return set(OtIdentifier::create(name), value); }

	// member access by name
	inline bool hasByName(const std::string& name) { return has(OtIdentifier::create(name)); }
	inline OtObject setByName(const std::string& name, OtObject value) { return set(OtIdentifier::create(name), value); }
	inline OtObject getByName(const std::string& name) { return get(OtIdentifier::create(name)); }
	inline void unsetByName(const std::string& name) { return unset(OtIdentifier::create(name)); }

	bool hasMembers() { return members != nullptr; }

	// iterate through the members
	inline void eachMember(std::function<void(OtID, OtObject object)> callback) { if (members) { members->each(callback); } }
	inline void eachMemberID(std::function<void(OtID)> callback) { if (members) { members->eachID(callback); } }

	// comparison
	virtual bool operator==(OtObject operand);
	virtual bool operator<(OtObject operand);

	inline bool equal(OtObject operand) { return operator==(operand); }
	inline bool notEqual(OtObject operand) { return !(operator==(operand)); }

	// "call" object (count, parameters)
	virtual OtObject operator() (size_t, OtObject*) { return nullptr; }

	// get an object's class
	OtObject getClass();

	// get type definition
	static OtType getMeta();

protected:
	// constructors/destructor (rule of 6)
	friend class OtObjectPointer<OtObjectClass>;
	OtObjectClass() = default;
	OtObjectClass(const OtObjectClass&) = delete;
	OtObjectClass(OtObjectClass&&) = delete;
	virtual ~OtObjectClass();

	OtObjectClass& operator=(const OtObjectClass&) = delete;
	OtObjectClass& operator=(OtObjectClass&&) = delete;

private:
	friend class OtClassClass;

	// object type
	OtType type;

	// reference count
	size_t referenceCount = 0;
	template <typename T> friend class OtObjectPointer;

	// members
	OtMembers* members = nullptr;
};
