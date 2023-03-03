//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtMembers.h"
#include "OtObjectPointer.h"


//
//	Forward references
//

class OtObjectClass;
using OtObject = OtObjectPointer<OtObjectClass>;


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
	bool has(size_t selector) { return members.has(selector) != 0; }
	OtObject set(size_t selector, OtObject value);
	OtObject set(const char* name, OtObject value);
	OtObject& get(size_t selector) { return members.get(selector); }
	void unset(size_t selector) { members.unset(selector); }
	void getMemberNames(std::vector<std::string_view>& names) { members.getMemberNames(names); }

	// create a new type
	template <class T>
	static OtType create(const std::string& name, OtType parent, OtAllocator allocator=nullptr) {
		if (!allocator) {
			allocator = []() {
				return (OtObject) OtObjectPointer<T>::create();
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
