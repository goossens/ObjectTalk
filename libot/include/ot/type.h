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
typedef std::shared_ptr<OtTypeClass> OtType;
typedef std::weak_ptr<OtTypeClass> OtTypeWeak;

class OtTypeClass {
private:
	typedef OtObject(*OtAllocator)();

public:
	// constructor
	OtTypeClass(const std::string& n, OtType p=nullptr, OtAllocator a=nullptr);

	// allocate a new instance
	OtObject allocate();

	// set the type's parent type
	void setParent(OtType p);

	// see if type is kind of
	bool isKindOf(const std::string& className);

	// get information
	std::string getName() { return name; }
	OtType getParent() { return parent.lock(); }

	// member access
	bool has(const std::string& name) { return members->has(name) != 0; }
	OtObject set(const std::string& name, OtObject value) { members->set(name, value); return value; }
	OtObject get(const std::string& name) { return members->has(name) ? members->get(name) : nullptr; }
	void unset(const std::string& name);
	OtMembers getMembers() { return members; }

	// create a new type
	template <class CLASS>
	static OtType create(const std::string& name, OtType parent, OtAllocator allocator=nullptr) {
		if (!allocator) {
			allocator = []() {
				return (OtObject) std::make_shared<CLASS>();
			};
		}

		return std::make_shared<OtTypeClass>(name, parent, allocator);
	}

	static OtType create(const std::string& name) {
		return std::make_shared<OtTypeClass>(name);
	}

private:
	// attributes
	std::string name;
	OtTypeWeak parent;
	OtMembers members;
	OtAllocator allocator;
};
