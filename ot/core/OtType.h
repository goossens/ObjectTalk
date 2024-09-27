//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <list>
#include <string>

#include "OtMembers.h"
#include "OtObjectPointer.h"


//
//	OtType
//

class OtTypeClass;
using OtTypeAllocator = OtObject(*)();

class OtType {
public:
	// constructors
	OtType() = default;
	OtType(OtTypeClass* t) : type(t) {}

	// pointer access
	OtTypeClass* operator->() { return type; }
	OtTypeClass& operator*() { return *type; }
	OtTypeClass* raw() { return type; }

	// see if type is valid
	operator bool() { return type != nullptr; }

	// see if types are equal
	bool operator==(OtType other) const { return type == other.type; }

	// create a new type
	template <class T>
	static OtType create(const std::string& name, OtType parent=nullptr, OtTypeAllocator allocator=nullptr);

	static OtType create(const std::string& name);

private:
	OtTypeClass* type;
	static std::list<OtTypeClass> types;
};


//
//	OtTypeClass
//

class OtObjectClass;
using OtObject = OtObjectPointer<OtObjectClass>;

class OtTypeClass {
public:
	// constructor
	OtTypeClass(const std::string& n, OtType p={}, OtTypeAllocator a=nullptr);

	// allocate a new instance
	OtObject allocate();

	// set the type's parent type
	void setParent(OtType p);

	// see if type is kind of
	bool isKindOf(const std::string& className);

	// get information
	std::string& getName() { return name; }
	OtType getParent() { return parent; }

	// member access
	bool has(size_t id) { return members.has(id) != 0; }
	OtObject set(size_t id, OtObject value);
	OtObject set(const char* name, OtObject value);
	OtObject& get(size_t id) { return members.get(id); }
	void unset(size_t id) { members.unset(id); }
	void getMemberNames(std::vector<std::string_view>& names) { members.getMemberNames(names); }

private:
	// attributes
	std::string name;
	OtType parent;
	OtMembers members;
	OtTypeAllocator allocator;
};


//
//	OtType::create
//

template <class T>
OtType OtType::create(const std::string& name, OtType parent, OtTypeAllocator allocator) {
	if (!allocator) {
		allocator = []() {
			return (OtObject) OtObjectPointer<T>::create();
		};
	}

	return OtType(&types.emplace_back(name, parent, allocator));
}

inline OtType OtType::create(const std::string& name) {
	return OtType(&types.emplace_back(name));
}
