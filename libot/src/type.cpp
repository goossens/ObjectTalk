//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/type.h"


//
//	OtTypeRegistry
//

class OtTypeRegistry {
public:
	void store(OtSharedType type) {
		types.push_back(type);
	}

	// implemented as singleton
	static OtTypeRegistry& instance() {
		static OtTypeRegistry registry;
		return registry;
	}

private:
	// constructor (nobody should be instantiating this singleton)
	OtTypeRegistry() = default;

	// registry of registered types
	std::vector<OtSharedType> types;
};


//
//	OtTypeClass::OtTypeClass
//

OtTypeClass::OtTypeClass(const std::string& n, OtType p, OtConstructor c) {
	name = n;
	parent = p;
	constructor = c ? c : parent->constructor;
}


//
//	OtTypeClass::construct
//

OtObject OtTypeClass::construct() {
	return constructor();
}


//
//	OtTypeClass::subType
//

OtType OtTypeClass::subType(const std::string& n) {
	OtSharedType type = std::make_shared<OtTypeClass>(n, this, constructor);
	OtTypeRegistry::instance().store(type);
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
		members.erase(name);

	} else {
		OT_EXCEPT("Unknown member [%s] in itype [%s]", name.c_str(), getName().c_str());
	}
}


//
//	OtTypeClass::registerType
//

OtType OtTypeClass::registerType(OtSharedType type) {
	OtTypeRegistry::instance().store(type);
	return type.get();
}
