//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/context.h"
#include "ot/exception.h"


//
//	OtContext
//

bool OtContextClass::has(const std::string& name) {
	if (hasMember(name)) {
		return true;
	}

	for (auto p = parent.lock(); p; p = p->getParent()) {
		if (p->hasMember(name)) {
			return true;
		}
	}

	for (auto t = type; t; t = t->getParent()) {
		if (t->has(name)) {
			return true;
		}
	}

	return false;
}


//
//	OtContextClass::get
//

OtObject OtContextClass::get(const std::string& name) {
	if (hasMember(name)) {
		return getMember(name);
	}

	for (auto p = parent.lock(); p; p = p->getParent()) {
		if (p->hasMember(name)) {
			return p->getMember(name);
		}
	}

	for (auto t = type; t; t = t->getParent()) {
		if (t->has(name)) {
			return t->get(name);
		}
	}

	OT_EXCEPT("Unknown member [%s] in instance of class [%s]", name.c_str(), type->getName().c_str());
	return nullptr;
}


//
//	OtContextClass::getMeta
//

OtType OtContextClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtContextClass>("Context", OtInternalClass::getMeta());
	}

	return type;
}


//
//	OtContextClass::create
//

OtContext OtContextClass::create() {
	OtContext context = std::make_shared<OtContextClass>();
	context->setType(getMeta());
	return context;
}
