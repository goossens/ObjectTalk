//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/registry.h"
#include "ot/singleton.h"
#include "ot/type.h"


//
//	OtTypeRegistry
//

class OtTypeRegistry : public OtSingleton<OtTypeRegistry>, public OtRegistry<OtSharedType> {
};


//
//	OtTypeClass::OtTypeClass
//

OtTypeClass::OtTypeClass(const std::string& n, OtType p, OtAllocator a) {
	name = n;
	parent = p;
	allocator = a ? a : parent->allocator;
	members = OtMembersClass::create();
}


//
//	OtTypeClass::allocate
//

OtObject OtTypeClass::allocate() {
	return allocator();
}


//
//	OtTypeClass::subType
//

OtType OtTypeClass::subType(const std::string& name) {
	OtSharedType type = std::make_shared<OtTypeClass>(name, this, allocator);
	OtTypeRegistry::instance()->set(name, type);
	return type.get();
}


//
//	OtTypeClass::isKindOf
//

bool OtTypeClass::isKindOf(const std::string& className) {
	for (auto p = this; p; p = p->parent) {
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


//
//	OtTypeClass::registerType
//

OtType OtTypeClass::registerType(OtSharedType type) {
	OtTypeRegistry::instance()->set(type->getName(), type);
	return type.get();
}


//
//	OtTypeClass::getRegistered
//

OtType OtTypeClass::getRegistered(const std::string& name) {
	return OtTypeRegistry::instance()->get(name).get();
}
