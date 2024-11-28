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
	inline OtTypeClass* operator->() { return type; }
	inline OtTypeClass& operator*() { return *type; }
	inline OtTypeClass* raw() { return type; }

	// see if type is valid
	inline operator bool() { return type != nullptr; }

	// see if types are equal
	inline bool operator==(OtType other) const { return type == other.type; }

	// create a new type
	template <class T>
	static inline OtType create(const std::string& name, OtType parent=nullptr, OtTypeAllocator allocator=nullptr);

	template <class T>
	static inline OtType create(OtID id, OtType parent=nullptr, OtTypeAllocator allocator=nullptr);

	// create an incomplete type (parent must be set later)
	static inline OtType create(OtID id);

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
	OtTypeClass(OtID id, OtType p={}, OtTypeAllocator a=nullptr);

	// allocate a new instance
	OtObject allocate();

	// set the type's parent type
	void setParent(OtType p);

	// see if type is kind of
	bool isKindOf(OtID id);
	bool isKindOf(const std::string& name);

	// get information
	inline OtID getID() { return id; }
	inline OtType getParent() { return parent; }
	inline std::string getName() { return std::string(OtIdentifier::name(id)); }

	// member access
	inline bool has(OtID id) { return members.has(id) != 0; }
	OtObject set(OtID id, OtObject value);
	OtObject set(const char* name, OtObject value);
	inline OtObject get(OtID id) { return members.get(id); }
	inline void unset(OtID id) { members.unset(id); }

	// iterate through the members
	inline void eachMember(std::function<void(OtID, OtObject object)> callback) { members.each(callback); }
	inline void eachMemberID(std::function<void(OtID)> callback) { members.eachID(callback); }

private:
	// attributes
	OtID id;
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
OtType OtType::create(OtID id, OtType parent, OtTypeAllocator allocator) {
	if (!allocator) {
		allocator = []() {
			return (OtObject) OtObjectPointer<T>::create();
		};
	}

	return OtType(&types.emplace_back(id, parent, allocator));
}

inline OtType OtType::create(OtID id) {
	return OtType(&types.emplace_back(id));
}
