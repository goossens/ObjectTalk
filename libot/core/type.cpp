//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/type.h"


//
//	OtTypeClass::OtTypeClass
//

OtTypeClass::OtTypeClass(const std::string& n, OtType p, OtAllocator a) {
	name = n;
	parent = p;
	allocator = a ? a : p ? p->allocator : nullptr;
	members = OtMembersClass::create();
}


//
//	OtTypeClass::allocate
//

OtObject OtTypeClass::allocate() {
	if (!allocator) {
		OtExcept("Can't allocation incomplete type [%s]", name.c_str());
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
	for (auto p = this; p; p = p->parent.lock().get()) {
		if (p->name == className) {
			return true;
		}
	}

	return false;
}


//
//	OtTypeClass::unset
//

void OtTypeClass::unset(const std::string& name) {
	if (has(name)) {
		members->unset(name);

	} else {
		OtExcept("Unknown member [%s] in type [%s]", name.c_str(), getName().c_str());
	}
}
