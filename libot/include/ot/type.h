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
	typedef OtObject(*OtConstructor)();

public:
	// constructor
	OtTypeClass(const std::string& n, OtType p, OtConstructor c=nullptr);

	// construct a new instance
	OtObject construct();

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

	// register a type
	static OtType registerType(OtSharedType type);

	// retrieve a registered type
	static OtType getRegistered(const std::string& name);

	// create a new type
	template <class CLASS>
	static OtType create(const std::string& name, OtType parent) {
		OtSharedType type = std::make_shared<OtTypeClass>(name, parent, []() {
			return (OtObject) std::make_shared<CLASS>();
		});

		return registerType(type);
	}

private:
	// attributes
	std::string name;
	OtType parent;
	OtMembers members;
	OtConstructor constructor;
};
