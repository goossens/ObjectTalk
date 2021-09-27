//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/function.h"
#include "ot/array.h"
#include "ot/dict.h"
#include "ot/dictreference.h"


//
//	OtDictClass::operator std::string
//

OtDictClass::operator std::string() {
	std::ostringstream o;
	bool first = true;

	o << "{";

	for (auto const& entry : dict) {
		if (first) {
			first = false;
		} else {
			o << ",";
		}

		o << OtText::toJSON(entry.first) << ":" << entry.second->json();
	}

	o <<  "}";
	return o.str();
}


//
//	OtDictClass::init
//

OtObject OtDictClass::init(size_t count, OtObject* parameters) {
	if (count %2 != 0) {
		OtExcept("Dict constructor expects an even number of parameters not [%ld]", count);
	}

	// clear dictionary and add all calling parameters
	dict.clear();

	for (size_t c = 0; c < count; c += 2) {
		dict.insert(std::make_pair((std::string) *parameters[c], parameters[c + 1]));
	}

	return shared();
}


//
//	OtDictClass::operator ==
//

bool OtDictClass::operator == (OtObject operand) {
	OtDict op = operand->cast<OtDictClass>();

	// ensure object is an dictionary
	if (!op) {
		return false;

	// ensure they have the same size
	} else if (dict.size() != op->dict.size()) {
		return false;
	}

	// compare all elements
	for (auto& it : dict) {
		if (op->dict.find(it.first) == op->dict.end()) {
			return false;

		} else if (!it.second->equal(op->dict[it.first])) {
			return false;
		}
	}

	return true;
}


//
//	OtDictClass::getEntry
//

OtObject OtDictClass::getEntry(const std::string& index) {
	// sanity check
	if (dict.find(index) == dict.end()) {
		OtExcept("Unkown dictionary member [%s]", index.c_str());
	}

	// return entry
	return dict[index];
}


//
//	OtDictClass::setEntry
//

OtObject OtDictClass::setEntry(const std::string& index, OtObject object) {
	// set entry
	dict[index] = object;
	return object;
}


//
//	OtDictClass::index
//

OtObject OtDictClass::index(const std::string& index) {
	return OtDictReferenceClass::create(cast<OtDictClass>(), index);
}


//
//	OtDictClass::add
//

OtObject OtDictClass::add(OtObject value) {
	if (!value->isKindOf("Dict")) {
		OtExcept("The dictionary add operator expects another [dictionary] instance, not [%s]", value->getType()->getName().c_str());
	}

	OtDict result = create();

	for (auto& it : dict) {
		result->setEntry(it.first, it.second);
	}


	for (auto& it : value->cast<OtDictClass>()->dict) {
		result->setEntry(it.first, it.second);
	}

	return result;
}


//
//	OtDictClass::clone
//

OtObject OtDictClass::clone() {
	OtDict result = create();

	for (auto& it : dict) {
		result->setEntry(it.first, it.second);
	}

	return result;
}


//
//	OtDictClass::merge
//

OtObject OtDictClass::merge(OtObject value) {
	if (!value->isKindOf("Dict")) {
		OtExcept("Dictionary merge expects another [dictionary] instance, not [%s]", value->getType()->getName().c_str());
	}

	OtDict result = create();

	for (auto& it : dict) {
		result->setEntry(it.first, it.second);
	}


	for (auto& it : value->cast<OtDictClass>()->dict) {
		result->setEntry(it.first, it.second);
	}

	return result;
}


//
//	OtDictClass::eraseEntry
//

void OtDictClass::eraseEntry(const std::string& index) {
	if (dict.find(index) == dict.end()) {
		OtExcept("Unkown dictionary member [%s]", index.c_str());
	}

	dict.erase(index);
}


//
//	OtDictClass::keys
//

OtObject OtDictClass::keys() {
	// create array of all keys in dictionary
	OtArray array = OtArrayClass::create();

	for (auto const& entry : dict) {
		array->append(OtStringClass::create(entry.first));
	}

	return array;
}


//
//	OtDictClass::values
//

OtObject OtDictClass::values() {
	// create array of all values in dictionary
	OtArray array = OtArrayClass::create();

	for (auto const& entry : dict) {
		array->append(entry.second);
	}

	return array;
}


//
//	OtDictClass::getMeta
//

OtType OtDictClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtDictClass>("Dict", OtCollectionClass::getMeta());

		type->set("string", OtFunctionClass::create(&OtDictClass::operator std::string));

		type->set("__init__", OtFunctionClass::create(&OtDictClass::init));
		type->set("__index__", OtFunctionClass::create(&OtDictClass::index));
		type->set("__add__", OtFunctionClass::create(&OtDictClass::add));
		type->set("__contains__", OtFunctionClass::create(&OtDictClass::contains));

		type->set("size", OtFunctionClass::create(&OtDictClass::size));
		type->set("contains", OtFunctionClass::create(&OtDictClass::contains));

		type->set("clone", OtFunctionClass::create(&OtDictClass::clone));
		type->set("merge", OtFunctionClass::create(&OtDictClass::merge));
		type->set("clear", OtFunctionClass::create(&OtDictClass::clear));
		type->set("erase", OtFunctionClass::create(&OtDictClass::eraseEntry));

		type->set("keys", OtFunctionClass::create(&OtDictClass::keys));
		type->set("values", OtFunctionClass::create(&OtDictClass::values));
	}

	return type;
}


//
//	OtDictClass::create
//

OtDict OtDictClass::create() {
	OtDict dict = std::make_shared<OtDictClass>();
	dict->setType(getMeta());
	return dict;
}

OtDict OtDictClass::create(size_t count, OtObject* objects) {
	OtDict dict = create();

	for (size_t c = 0; c < count; c += 2) {
		dict->setEntry((std::string) *objects[c], objects[c + 1]);
	}

	return dict;
}
