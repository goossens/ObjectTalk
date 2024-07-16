//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtException.h"
#include "OtClass.h"
#include "OtFunction.h"
#include "OtObject.h"


//
//	OtObjectClass::~OtObjectClass
//

OtObjectClass::~OtObjectClass() {
	unsetAll();
}


//
//	OtObjectClass::describe
//

std::string OtObjectClass::describe() {
	return type->getName();
}


//
//	OtObjectClass::has
//

bool OtObjectClass::has(size_t selector) {
	if (members && members->has(selector)) {
		return true;
	}

	for (auto t = type; t; t = t->getParent()) {
		if (t->has(selector)) {
			return true;
		}
	}

	return false;
}


//
//	OtObjectClass::set
//

OtObject OtObjectClass::set(size_t selector, OtObject value) {
	if (!members) {
		members = new OtMembers;
	}

	members->set(selector, value);
	return value;
}


//
//	OtObjectClass::get
//

OtObject& OtObjectClass::get(size_t selector) {
	if (members && members->has(selector)) {
		return members->get(selector);
	}

	for (auto t = type; t; t = t->getParent()) {
		if (t->has(selector)) {
			return t->get(selector);
		}
	}

	auto name = OtSelector::name(selector);
	OtError("Unknown member [{}] in instance of class [{}]", name, type->getName());

	// we will never get here because of the exception but a return statement keeps the compiler happy
	return members->get(selector);
}


//
//	OtObjectClass::unset
//

void OtObjectClass::unset(size_t selector) {
	if (members && members->has(selector)) {
		members->unset(selector);

	} else {
		auto name = OtSelector::name(selector);
		OtError("Unknown member [%.*s] in instance of class [{}]", name.size(), name.data(), type->getName());
	}
}


//
//	OtObjectClass::unsetAll
//

void OtObjectClass::unsetAll() {
	if (members) {
		delete members;
		members = nullptr;
	}
}


//
//	OtObjectClass::getMemberNames
//

void OtObjectClass::getMemberNames(std::vector<std::string_view>& names) {
	if (members) {
		members->getMemberNames(names);

	} else {
		names.clear();
	}
}


//
//	OtObjectClass::operator==
//

bool OtObjectClass::operator==(OtObject operand) {
	if (this == operand.raw()) {
		return true;

	} else {
		return this->type == getMeta() && operand.raw()->type == getMeta();
	}
}


//
//	OtObjectClass::operator<
//

bool OtObjectClass::operator<(OtObject operand) {
	return this < operand.raw();
}


//
//	OtObjectClass::expectKindOf
//

static bool isvowel(char ch) {
	ch = tolower(ch);
	return ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u';
}

void OtObjectClass::expectKindOf(const std::string& className) {
	// ensure object is of the right kind
	if (!isKindOf(className)) {
		OtError("Expected {} [{}] instance, not a [{}]",
			isvowel(className[0]) ? "an" : "a",
			className.c_str(),
			getType()->getName().c_str());
	}
}


//
//	OtObjectClass::getClass
//

OtObject OtObjectClass::getClass() {
	return OtClass::create(getType());
}


//
//	OtObjectClass::getMeta
//

OtType OtObjectClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtObjectClass>("Object", nullptr);

		type->set("boolean", OtFunction::create(&OtObjectClass::operator bool));
		type->set("integer", OtFunction::create(&OtObjectClass::operator int64_t));
		type->set("real", OtFunction::create(&OtObjectClass::operator double));
		type->set("string", OtFunction::create(&OtObjectClass::operator std::string));
		type->set("json", OtFunction::create(&OtObjectClass::json));

		type->set("has", OtFunction::create(&OtObjectClass::hasByName));
		type->set("set", OtFunction::create(&OtObjectClass::setByName));
		type->set("get", OtFunction::create(&OtObjectClass::getByName));
		type->set("unset", OtFunction::create(&OtObjectClass::unsetByName));
		type->set("unsetAll", OtFunction::create(&OtObjectClass::unsetAll));

		type->set("__eq__", OtFunction::create(&OtObjectClass::equal));
		type->set("__ne__", OtFunction::create(&OtObjectClass::notEqual));

		type->set("getClass", OtFunction::create(&OtObjectClass::getClass));
		type->set("isKindOf", OtFunction::create(&OtObjectClass::isKindOf));
	}

	return type;
}
