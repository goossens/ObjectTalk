//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/object.h"
#include "ot/function.h"
#include "ot/class.h"
#include "ot/memberreference.h"


//
//	OtObjectClass::has
//

bool OtObjectClass::has(const std::string& name) {
	if (members && members->has(name)) {
		return true;
	}

	for (auto t = type; t; t = t->getParent()) {
		if (t->has(name)) {
			return true;
		}
	}

	return false;
}


//
//	OtObjectClass::set
//

OtObject OtObjectClass::set(const std::string& name, OtObject value) {
	if (!members) {
		members = OtMembersClass::create();
	}

	members->set(name, value);
	return value;
}


//
//	OtObjectClass::get
//

OtObject OtObjectClass::get(const std::string& name) {
	if (members && members->has(name)) {
		return members->get(name);
	}

	for (auto t = type; t; t = t->getParent()) {
		if (t->has(name)) {
			return t->get(name);
		}
	}

	OtExcept("Unknown member [%s] in instance of class [%s]", name.c_str(), type->getName().c_str());
	return nullptr;
}


//
//	OtObjectClass::unset
//

void OtObjectClass::unset(const std::string& name) {
	if (members && members->has(name)) {
		members->unset(name);

	} else {
		OtExcept("Unknown member [%s] in instance of class [%s]", name.c_str(), type->getName().c_str());
	}
}


//
//	OtObjectClass::member
//

OtObject OtObjectClass::member(const std::string& name) {
	return OtMemberReferenceClass::create(getSharedPtr(), name);
}


//
//	OtObjectClass::operator ==
//

bool OtObjectClass::operator ==(OtObject operand) {
	if (getSharedPtr().get() == operand.get()) {
		return true;

	} else {
		return type == getMeta() && operand->getType() == getMeta();
	}
}


//
//	OtObjectClass::operator <
//

bool OtObjectClass::operator <(OtObject operand) {
	return getSharedPtr().get() < operand.get();
}


//
//	OtObjectClass::getClass
//

OtClass OtObjectClass::getClass() {
	return OtClassClass::create(getType());
}


//
//	OtObjectClass::getMeta
//

OtType OtObjectClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtObjectClass>("Object", nullptr);

		type->set("boolean", OtFunctionClass::create(&OtObjectClass::operator bool));
		type->set("integer", OtFunctionClass::create(&OtObjectClass::operator long));
		type->set("real", OtFunctionClass::create(&OtObjectClass::operator double));
		type->set("string", OtFunctionClass::create(&OtObjectClass::operator std::string));
		type->set("json", OtFunctionClass::create(&OtObjectClass::json));

		type->set("has", OtFunctionClass::create(&OtObjectClass::has));
		type->set("__member__", OtFunctionClass::create(&OtObjectClass::member));
		type->set("unset", OtFunctionClass::create(&OtObjectClass::unset));

		type->set("__eq__", OtFunctionClass::create(&OtObjectClass::equal));
		type->set("__ne__", OtFunctionClass::create(&OtObjectClass::notEqual));

		type->set("getClass", OtFunctionClass::create(&OtObjectClass::getClass));
		type->set("isKindOf", OtFunctionClass::create(&OtObjectClass::isKindOf));
	}

	return type;
}


//
//	OtObjectClass::create
//

OtObject OtObjectClass::create() {
	OtObject object = std::make_shared<OtObjectClass>();
	object->setType(getMeta());
	return object;
}
