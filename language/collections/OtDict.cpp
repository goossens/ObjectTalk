//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtArray.h"
#include "OtDict.h"
#include "OtDictReference.h"
#include "OtFunction.h"
#include "OtLog.h"


//
//	OtDictClass::OtDictClass
//

OtDictClass::OtDictClass(size_t count, OtObject* objects) {
	for (size_t c = 0; c < count; c += 2) {
		setEntry((std::string) *objects[c], objects[c + 1]);
	}
}


//
//	OtDictClass::operator std::string
//

OtDictClass::operator std::string() {
	std::ostringstream o;
	bool first = true;

	o << "{";

	for (auto& entry : dict) {
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

void OtDictClass::init(size_t count, OtObject* parameters) {
	if (count %2 != 0) {
		OtLogError("Dict constructor expects an even number of parameters not [{}]", count);
	}

	// clear dictionary and add all calling parameters
	dict.clear();

	for (size_t c = 0; c < count; c += 2) {
		dict.insert(std::make_pair((std::string) *parameters[c], parameters[c + 1]));
	}
}


//
//	OtDictClass::operator==
//

bool OtDictClass::operator==(OtObject operand) {
	OtDict op = operand;

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
//	OtDictClass::set
//

OtObject OtDictClass::set(OtID id, OtObject value) {
	auto index = std::string(OtIdentifier::name(id));
	setEntry(index, value);
	return value;
}


//
//	OtDictClass::get
//

OtObject OtDictClass::get(OtID id) {
	auto index = std::string(OtIdentifier::name(id));

	if (contains(index)) {
		return getEntry(index);

	} else {
		return OtCollectionClass::get(id);
	}
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
//	OtDictClass::getEntry
//

OtObject OtDictClass::getEntry(const std::string& index) {
	// sanity check
	if (dict.find(index) == dict.end()) {
		OtLogError("Unkown dictionary member [{}]", index);
	}

	// return entry
	return dict[index];
}


//
//	OtDictClass::index
//

OtObject OtDictClass::index(const std::string& index) {
	return OtDictReference::create(OtDict(this), index);
}


//
//	OtDictClass::add
//

OtObject OtDictClass::add(OtObject value) {
	if (!value.isKindOf<OtDictClass>()) {
		OtLogError("The dictionary add operator expects another [Dict] instance, not [{}]", value.getTypeName());
	}

	auto result = OtDict::create();

	for (auto& it : dict) {
		result->setEntry(it.first, it.second);
	}

	for (auto& it : OtDict(value)->dict) {
		result->setEntry(it.first, it.second);
	}

	return result;
}


//
//	OtDictClass::clone
//

OtObject OtDictClass::clone() {
	auto result = OtDict::create();

	for (auto& it : dict) {
		result->setEntry(it.first, it.second);
	}

	return result;
}


//
//	OtDictClass::merge
//

OtObject OtDictClass::merge(OtObject value) {
	if (!value.isKindOf<OtDictClass>()) {
		OtLogError("Dictionary merge expects another [Dict] instance, not [{}]", value.getTypeName());
	}

	auto result = OtDict::create();

	for (auto& it : dict) {
		result->setEntry(it.first, it.second);
	}


	for (auto& it : OtDict(value)->dict) {
		result->setEntry(it.first, it.second);
	}

	return result;
}


//
//	OtDictClass::eraseEntry
//

void OtDictClass::eraseEntry(const std::string& index) {
	if (dict.find(index) == dict.end()) {
		OtLogError("Unkown dictionary member [{}]", index);
	}

	dict.erase(index);
}


//
//	OtDictClass::keys
//

OtObject OtDictClass::keys() {
	// create array of all keys in dictionary
	auto array = OtArray::create();

	for (auto const& entry : dict) {
		array->append(OtString::create(entry.first));
	}

	return array;
}


//
//	OtDictClass::values
//

OtObject OtDictClass::values() {
	// create array of all values in dictionary
	auto array = OtArray::create();

	for (auto const& entry : dict) {
		array->append(entry.second);
	}

	return array;
}


//
//	OtDictClass::getMeta
//

OtType OtDictClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtDictClass>("Dict", OtCollectionClass::getMeta());

		type->set("string", OtFunction::create(&OtDictClass::operator std::string));

		type->set("__init__", OtFunction::create(&OtDictClass::init));
		type->set("__index__", OtFunction::create(&OtDictClass::index));
		type->set("__add__", OtFunction::create(&OtDictClass::add));
		type->set("__contains__", OtFunction::create(&OtDictClass::contains));

		type->set("size", OtFunction::create(&OtDictClass::size));
		type->set("contains", OtFunction::create(&OtDictClass::contains));

		type->set("clone", OtFunction::create(&OtDictClass::clone));
		type->set("merge", OtFunction::create(&OtDictClass::merge));
		type->set("clear", OtFunction::create(&OtDictClass::clear));
		type->set("erase", OtFunction::create(&OtDictClass::eraseEntry));

		type->set("keys", OtFunction::create(&OtDictClass::keys));
		type->set("values", OtFunction::create(&OtDictClass::values));
	}

	return type;
}
