//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>
#include <utility>

#include "type.h"
#include "members.h"


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
	virtual ~OtObjectClass() {}

	// type access
	void setType(OtType t) { type = t; }
	OtType getType() { return type; }

	// see if object "is kind of"
	bool isKindOf(const std::string& className) { return type->isKindOf(className); }

	// default conversion operators
	virtual operator bool() { return false; }
	virtual operator int() { return 0; }
	virtual operator long() { return 0; }
	virtual operator size_t() { return 0; }
	virtual operator double() { return 0.0; }
	virtual operator std::string() { return ""; }
	virtual operator OtObject() { return shared_from_this(); }

	// get object's JSON representation
	virtual std::string json() { return operator std::string(); }

	// describe object for debugging purposes
	virtual std::string describe() { return ""; }

	// get shared pointer
	OtObject getSharedPtr() { return shared_from_this(); }

	// member acccess
	virtual bool has(const std::string& name);
	virtual OtObject set(const std::string& name, OtObject value);
	virtual OtObject get(const std::string& name);
	virtual void unset(const std::string& name);

	// support member operator
	OtObject member(const std::string& name);
	OtMembers getMembers() { return members; }

	// comparison
	virtual bool operator ==(OtObject operand);
	virtual bool operator <(OtObject operand);

	bool equal(OtObject operand) { return operator ==(operand); }
	bool notEqual(OtObject operand) { return !operator ==(operand); }

	// "call" object (count, parameters)
	virtual OtObject operator()(size_t, OtObject*) { return nullptr; }

	// get an object's class
	OtClass getClass();

	// cast shared pointer to specified type
	template <typename CLASS>
	std::shared_ptr<CLASS> cast() { return std::dynamic_pointer_cast<CLASS>(shared_from_this()); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtObject create();

protected:
	// object type
	OtType type;

	// members
	OtMembers members;
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
