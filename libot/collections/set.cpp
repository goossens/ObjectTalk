//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <sstream>
#include <utility>

#include "ot/set.h"
#include "ot/setiterator.h"
#include "ot/function.h"


//
//	OtSetClass::operator std::string
//

OtSetClass::operator std::string() {
	std::ostringstream o;
	bool first = true;

	o << "[";

	for (auto const& entry : set) {
		if (first) {
			first = false;

		} else {
			o << ",";
		}

		o << entry->json();
	}

	o << "]";
	return o.str();
}


//
//	OtSetClass::init
//

OtObject OtSetClass::init(size_t count, OtObject* parameters) {
	set.clear();

	for (auto c = 0; c < count; c++) {
		set.insert(parameters[c]);
	}

	return shared();
}


//
//	OtSetClass::operator ==
//

bool OtSetClass::operator == (OtObject object) {
	OtSet op = object->cast<OtSetClass>();

	// ensure object is a set
	if (!op) {
		return false;

	// ensure they have the same size
	} else if (set.size() != op->set.size()) {
		return false;
	}

	// compare all elements
	for (auto const& entry : set) {
		if (!op->contains(entry)) {
			return false;
		}
	}

	return true;
}


//
//	OtSetClass::iterate
//

OtObject OtSetClass::iterate() {
	return OtSetIteratorClass::create(cast<OtSetClass>());
}


//
//	OtSetClass::add
//

OtObject OtSetClass::add(OtObject object) {
	OtSet result = create();

	for (auto& entry : set) {
		result->set.insert(entry);
	}

	if (!result->contains(object)) {
		result->set.insert(std::move(object));
	}

	return result;
}


//
//	OtSetClass::subtract
//

OtObject OtSetClass::subtract(OtObject object) {
	OtSet result = create();

	for (auto& entry : set) {
		if (!entry->equal(object)) {
			result->set.insert(entry);
		}
	}

	return result;
}


//
//	OtSetClass::contains
//

bool OtSetClass::contains(OtObject object) {
	for (auto const& entry : set) {
		if (entry->equal(object)) {
			return true;
		}
	}

	return false;
}


//
//	OtSetClass::clone
//

OtObject OtSetClass::clone() {
	OtSet result = create();

	for (auto& entry : set) {
		result->set.insert(entry);
	}

	return result;
}


//
//	OtSetClass::merge
//

OtObject OtSetClass::merge(OtObject object) {
	if (!object->isKindOf("Set")) {
		OtExcept("Set merge expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = create();

	for (auto& item : set) {
		result->set.insert(item);
	}

	for (auto& item : object->cast<OtSetClass>()->set) {
		if (!result->contains(item)) {
			result->set.insert(item);
		}
	}

	return result;
}


//
//	OtSetClass::insert
//

void OtSetClass::insert(OtObject object) {
	if (!contains(object)) {
		set.insert(std::move(object));
	}
}


//
//	OtSetClass::erase
//

void OtSetClass::erase(OtObject object) {
	bool found = false;

	// use remove_if when we move the C++20
	for (auto it = set.begin(); !found && it != set.end();) {
		if ((*it)->equal(object)) {
			it = set.erase(it);
			found = true;

		} else {
			it++;
		}
	}
}


//
//	OtSetClass::intersection
//

OtObject OtSetClass::intersectWith(OtObject object) {
	OtSet op = object->cast<OtSetClass>();

	if (!op) {
		OtExcept("Set intersect method expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = create();

	for (auto& item : set) {
		if (op->contains(item)) {
			result->set.insert(item);
		}
	}

	return result;
}


//
//	OtSetClass::difference
//

OtObject OtSetClass::diffFrom(OtObject object) {
	OtSet op = object->cast<OtSetClass>();

	if (!op) {
		OtExcept("Set difference expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = create();

	for (auto& item : set) {
		if (!op->contains(item)) {
			result->set.insert(item);
		}
	}

	for (auto& item : op->set) {
		if (!contains(item)) {
			result->set.insert(item);
		}
	}

	return result;
}


//
//	OtSetClass::unionWith
//

OtObject OtSetClass::unionWith(OtObject object) {
	if (!object->isKindOf("Set")) {
		OtExcept("Set union expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = create();

	for (auto& item : set) {
		result->set.insert(item);
	}

	for (auto& item : object->cast<OtSetClass>()->set) {
		if (!result->contains(item)) {
			result->set.insert(item);
		}
	}

	return result;
}


//
//	OtSetClass::subtractFrom
//

OtObject OtSetClass::subtractFrom(OtObject object) {
	OtSet op = object->cast<OtSetClass>();

	if (!op) {
		OtExcept("Set subtract expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = create();

	for (auto& item : set) {
		if (!op->contains(item)) {
			result->set.insert(item);
		}
	}

	return result;
}


//
//	OtSetClass::getMeta
//

OtType OtSetClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtSetClass>("Set", OtCollectionClass::getMeta());

		type->set("string", OtFunctionClass::create(&OtSetClass::operator std::string));

		type->set("__init__", OtFunctionClass::create(&OtSetClass::init));
		type->set("__iter__", OtFunctionClass::create(&OtSetClass::iterate));
		type->set("__add__", OtFunctionClass::create(&OtSetClass::add));
		type->set("__sub__", OtFunctionClass::create(&OtSetClass::subtract));
		type->set("__contains__", OtFunctionClass::create(&OtSetClass::contains));

		type->set("size", OtFunctionClass::create(&OtSetClass::size));
		type->set("contains", OtFunctionClass::create(&OtSetClass::contains));

		type->set("clone", OtFunctionClass::create(&OtSetClass::clone));
		type->set("merge", OtFunctionClass::create(&OtSetClass::merge));
		type->set("clear", OtFunctionClass::create(&OtSetClass::clear));

		type->set("insert", OtFunctionClass::create(&OtSetClass::insert));
		type->set("erase", OtFunctionClass::create(&OtSetClass::erase));

		type->set("intersection", OtFunctionClass::create(&OtSetClass::intersectWith));
		type->set("difference", OtFunctionClass::create(&OtSetClass::diffFrom));
		type->set("union", OtFunctionClass::create(&OtSetClass::unionWith));
		type->set("subtract", OtFunctionClass::create(&OtSetClass::subtractFrom));
	}

	return type;
}


//
//	OtSetClass::create
//

OtSet OtSetClass::create() {
	OtSet set = std::make_shared<OtSetClass>();
	set->setType(getMeta());
	return set;
}


//
//	OtSetClass::create
//

OtSet OtSetClass::create(size_t count, OtObject* objects) {
	OtSet set = create();

	for (auto c = 0; c < count; c++) {
		set->add(objects[c]);
	}

	return set;
}
