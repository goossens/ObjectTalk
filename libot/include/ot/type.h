//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "members.h"


//
//	Forward references
//

class OtObjectClass;
typedef std::shared_ptr<OtObjectClass> OtObject;


//
//	OtType
//

class OtTypeClass;
typedef OtTypeClass* OtType;
typedef std::shared_ptr<OtTypeClass> OtSharedType;

class OtTypeClass {
private:
	typedef OtObject(*OtAllocator)();

public:
	// constructor
	OtTypeClass(const std::string& n, OtType p, OtAllocator a=nullptr);

	// allocate a new instance
	OtObject allocate();

	// create a sub-type
	OtType subType(const std::string& n);

	// see if type is kind of
	bool isKindOf(const std::string& className);

	// get information
	std::string getName() { return name; }
	OtType getParent() { return parent; }

	// member access
	bool has(const std::string& name) { return members->has(name) != 0; }
	OtObject set(const std::string& name, OtObject value) { members->set(name, value); return value; }
	OtObject get(const std::string& name) { return members->has(name) ? members->get(name) : nullptr; }
	void unset(const std::string& name);
	OtMembers getMembers() { return members; }

	// register a type
	static OtType registerType(OtSharedType type);

	// retrieve a registered type
	static OtType getRegistered(const std::string& name);

	// create a new type
	template <class CLASS>
	static OtType create(const std::string& name, OtType parent, OtAllocator allocator=nullptr) {
		if (!allocator) {
			allocator = []() {
				return (OtObject) std::make_shared<CLASS>();
			};
		}

		return registerType(std::make_shared<OtTypeClass>(name, parent, allocator));
	}

private:
	// attributes
	std::string name;
	OtType parent;
	OtMembers members;
	OtAllocator allocator;
};
