//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <list>
#include <string>

#include "OtIdentifier.h"
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

	template <class T>
	static OtType create(size_t id, OtType parent=nullptr, OtTypeAllocator allocator=nullptr);

	// create an incomplete type (parent must be set later)
	static OtType create(size_t id);

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
	OtTypeClass(size_t id, OtType p={}, OtTypeAllocator a=nullptr);

	// allocate a new instance
	OtObject allocate();

	// set the type's parent type
	void setParent(OtType p);

	// see if type is kind of
	bool isKindOf(size_t id);
	bool isKindOf(const std::string& name);

	// get information
	inline size_t getID() { return id; }
	inline OtType getParent() { return parent; }
	inline std::string getName() { return std::string(OtIdentifier::name(id)); }

	// member access
	inline bool has(size_t id) { return members.has(id) != 0; }
	OtObject set(size_t id, OtObject value);
	OtObject set(const char* name, OtObject value);
	inline OtObject& get(size_t id) { return members.get(id); }
	inline void unset(size_t id) { members.unset(id); }

	// iterate through the members
	inline void eachMember(std::function<void(size_t, OtObject object)> callback) { members.each(callback); }
	inline void eachMemberID(std::function<void(size_t)> callback) { members.eachID(callback); }

private:
	// attributes
	size_t id;
	OtType parent;
	OtMembers members;
	OtTypeAllocator allocator;
};


//
//	OtType::create
//

template <class T>
OtType OtType::create(const std::string& name, OtType parent, OtTypeAllocator allocator) {
	return OtType::create<T>(OtIdentifier::create(name), parent, allocator);
}

template <class T>
OtType OtType::create(size_t id, OtType parent, OtTypeAllocator allocator) {
	if (!allocator) {
		allocator = []() {
			return (OtObject) OtObjectPointer<T>::create();
		};
	}

	return OtType(&types.emplace_back(id, parent, allocator));
}

inline OtType OtType::create(size_t id) {
	return OtType(&types.emplace_back(id));
}
