//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	if (members) {
		delete members;
		members = nullptr;
	}

	if (observers) {
		delete observers;
		observers = nullptr;
	}
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

OtObject OtObjectClass::get(size_t selector) {
	if (members && members->has(selector)) {
		return members->get(selector);
	}

	for (auto t = type; t; t = t->getParent()) {
		if (t->has(selector)) {
			return t->get(selector);
		}
	}

	auto text = OtSelector::name(selector);
	OtExcept("Unknown member [%s] in instance of class [%s]", OtSelector::name(selector).c_str(), type->getName().c_str());
	return nullptr;
}


//
//	OtObjectClass::unset
//

void OtObjectClass::unset(size_t selector) {
	if (members && members->has(selector)) {
		members->unset(selector);

	} else {
		OtExcept("Unknown member [%s] in instance of class [%s]", OtSelector::name(selector).c_str(), type->getName().c_str());
	}
}


//
//	OtObjectClass::getMemberNames
//

std::vector<std::string> OtObjectClass::getMemberNames() {
	if (members) {
		return members->getMemberNames();
	}

	return std::vector<std::string>{};
 }


//
//	OtObjectClass::operator ==
//

bool OtObjectClass::operator == (OtObject operand) {
	if (shared().get() == operand.get()) {
		return true;

	} else {
		return type == getMeta() && operand->getType() == getMeta();
	}
}


//
//	OtObjectClass::operator <
//

bool OtObjectClass::operator < (OtObject operand) {
	return shared().get() < operand.get();
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
		OtExcept("Expected %s [%s] instance, not a [%s]",
			isvowel(className[0]) ? "an" : "a",
			className.c_str(),
			getType()->getName().c_str());
	}
}


//
//	OtObjectClass::getClass
//

OtClass OtObjectClass::getClass() {
	return OtClassClass::create(getType());
}


//
//	OtObjectClass::attach
//

size_t OtObjectClass::attach(std::function<void(void)> callback) {
	if (!observers) {
		observers = new std::vector<OtObserver>;
	}

	static size_t nextID = 1;
	size_t id = nextID++;

	observers->push_back(OtObserver(id, callback));
	return id;
}


//
//	OtObjectClass::detach
//

void OtObjectClass::detach(size_t id) {
	if (observers) {
		observers->erase(std::remove_if(observers->begin(), observers->end(), [id] (OtObserver& observer) {
			return observer.id == id;
		}), observers->end());
	}
}


//
//	OtObjectClass::notify
//

void OtObjectClass::notify() {
	if (observers) {
		for (auto& observer : *observers) {
			observer.callback();
		}
	}
}


//
//	OtObjectClass::getMeta
//

OtType OtObjectClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtObjectClass>("Object", nullptr);

		type->set("boolean", OtFunctionClass::create(&OtObjectClass::operator bool));
		type->set("integer", OtFunctionClass::create(&OtObjectClass::operator int64_t));
		type->set("real", OtFunctionClass::create(&OtObjectClass::operator double));
		type->set("string", OtFunctionClass::create(&OtObjectClass::operator std::string));
		type->set("json", OtFunctionClass::create(&OtObjectClass::json));

		type->set("has", OtFunctionClass::create(&OtObjectClass::hasByName));
		type->set("set", OtFunctionClass::create(&OtObjectClass::setByName));
		type->set("get", OtFunctionClass::create(&OtObjectClass::getByName));
		type->set("unset", OtFunctionClass::create(&OtObjectClass::unsetByName));
		type->set("unsetAll", OtFunctionClass::create(&OtObjectClass::unsetAll));

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
