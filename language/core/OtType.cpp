//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

OtTypeClass::OtTypeClass(OtID i, OtType p, OtTypeAllocator a) {
	typeID = i;
	parent = p;
	allocator = a ? a : p ? p->allocator : nullptr;
}


//
//	OtTypeClass::allocate
//

OtObject OtTypeClass::allocate() {
	if (!allocator) {
		OtError("Can't allocate incomplete type [{}]", OtIdentifier::name(typeID));
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

bool OtTypeClass::isKindOf(OtID otherID) {
	for (auto p = this; p; p = p->parent.raw()) {
		if (p->typeID == otherID) {
			return true;
		}
	}

	return false;
}

bool OtTypeClass::isKindOf(const std::string& name) {
	return isKindOf(OtIdentifier::create(name));
}


//
//	OtTypeClass::set
//

OtObject OtTypeClass::set(OtID id, OtObject value) {
	members.set(id, value);
	return value;
}

OtObject OtTypeClass::set(const char* name, OtObject value) {
	return set(OtIdentifier::create(name), value);
}
