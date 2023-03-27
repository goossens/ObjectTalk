//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <sstream>
#include <utility>

#include "OtFunction.h"
#include "OtSet.h"
#include "OtSetIterator.h"


//
//	OtSetClass::OtSetClass
//

OtSetClass::OtSetClass(OtObject* objects, size_t count) {
	for (auto c = 0; c < count; c++) {
		add(objects[c]);
	}
}


//
//	OtSetClass::operator std::string
//

OtSetClass::operator std::string() {
	std::ostringstream o;
	bool first = true;

	o << "[";

	for (auto entry : set) {
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

void OtSetClass::init(size_t count, OtObject* parameters) {
	for (auto c = 0; c < count; c++) {
		set.insert(parameters[c]);
	}
}


//
//	OtSetClass::operator ==
//

bool OtSetClass::operator == (OtObject object) {
	OtSet op = object;

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
	return OtSetIterator::create(OtSet(this));
}


//
//	OtSetClass::add
//

OtObject OtSetClass::add(OtObject object) {
	OtSet result = OtSet::create();

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
	OtSet result = OtSet::create();

	for (auto entry : set) {
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
	for (auto entry : set) {
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
	OtSet result = OtSet::create();

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
		OtError("Set merge expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = OtSet::create();

	for (auto& item : set) {
		result->set.insert(item);
	}

	for (auto& item : OtSet(object)->set) {
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
		if (((OtObject) *it)->equal(object)) {
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
	OtSet op = object;

	if (!op) {
		OtError("Set intersect method expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = OtSet::create();

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
	OtSet op = object;

	if (!op) {
		OtError("Set difference expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = OtSet::create();

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
		OtError("Set union expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = OtSet::create();

	for (auto& item : set) {
		result->set.insert(item);
	}

	for (auto& item : OtSet(object)->set) {
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
	OtSet op = object;

	if (!op) {
		OtError("Set subtract expects another [set] instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtSet result = OtSet::create();

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
	static OtType type;

	if (!type) {
		type = OtType::create<OtSetClass>("Set", OtCollectionClass::getMeta());

		type->set("string", OtFunction::create(&OtSetClass::operator std::string));

		type->set("__init__", OtFunction::create(&OtSetClass::init));
		type->set("__iter__", OtFunction::create(&OtSetClass::iterate));
		type->set("__add__", OtFunction::create(&OtSetClass::add));
		type->set("__sub__", OtFunction::create(&OtSetClass::subtract));
		type->set("__contains__", OtFunction::create(&OtSetClass::contains));

		type->set("size", OtFunction::create(&OtSetClass::size));
		type->set("contains", OtFunction::create(&OtSetClass::contains));

		type->set("clone", OtFunction::create(&OtSetClass::clone));
		type->set("merge", OtFunction::create(&OtSetClass::merge));
		type->set("clear", OtFunction::create(&OtSetClass::clear));

		type->set("insert", OtFunction::create(&OtSetClass::insert));
		type->set("erase", OtFunction::create(&OtSetClass::erase));

		type->set("intersection", OtFunction::create(&OtSetClass::intersectWith));
		type->set("difference", OtFunction::create(&OtSetClass::diffFrom));
		type->set("union", OtFunction::create(&OtSetClass::unionWith));
		type->set("subtract", OtFunction::create(&OtSetClass::subtractFrom));
	}

	return type;
}
