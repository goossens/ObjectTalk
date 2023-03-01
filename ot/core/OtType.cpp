//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtTypeClass::OtTypeClass
//

OtTypeClass::OtTypeClass(const std::string& n, OtType p, OtAllocator a) {
	name = n;
	parent = p;
	allocator = a ? a : p ? p->allocator : nullptr;
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
//	OtTypeClass::set
//

OtObject OtTypeClass::set(size_t selector, OtObject value) {
	members.set(selector, value); return value;
}

OtObject OtTypeClass::set(const char *name, OtObject value) {
	return set(OtSelector::create(name), value);
}


//
//	OtTypeClass::unset
//

void OtTypeClass::unset(size_t selector) {
	if (has(selector)) {
		members.unset(selector);

	} else {
		OtExcept("Unknown member [%s] in type [%s]", OtSelector::name(selector).c_str(), getName().c_str());
	}
}
