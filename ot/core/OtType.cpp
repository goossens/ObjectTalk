//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtObject.h"
#include "OtType.h"


//
//	List of all declared types
//

std::list<OtTypeClass> OtType::types;


//
//	OtTypeClass::OtTypeClass
//

OtTypeClass::OtTypeClass(const std::string& n, OtType p, OtTypeAllocator a) {
	name = n;
	parent = p;
	allocator = a ? a : p ? p->allocator : nullptr;
}


//
//	OtTypeClass::allocate
//

OtObject OtTypeClass::allocate() {
	if (!allocator) {
		OtError("Can't allocation incomplete type [{}]", name);
	}

	return allocator();
}


void OtTypeClass::setParent(OtType p) {
	if (!allocator) {
		allocator = p->allocator;
	}

	parent = p;
}


//
//	OtTypeClass::isKindOf
//

bool OtTypeClass::isKindOf(const std::string& className) {
	for (auto p = this; p; p = p->parent.raw()) {
		if (p->name == className) {
			return true;
		}
	}

	return false;
}


//
//	OtTypeClass::set
//

OtObject OtTypeClass::set(size_t selector, OtObject value) {
	members.set(selector, value); return value;
}

OtObject OtTypeClass::set(const char* name, OtObject value) {
	return set(OtSelector::create(name), value);
}
