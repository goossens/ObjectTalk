//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/dict.h"
#include "ot/function.h"
#include "ot/array.h"
#include "ot/internal.h"


//
//	OtDictClass::operator std::string
//

OtDictClass::operator std::string() {
	std::ostringstream o;
	bool first = true;

	o << "{";

	for (auto const& entry : *this) {
		if (first) {
			first = false;
		} else {
			o << ",";
		}

		o << "\"" << OtTextToJSON(entry.first) << "\":" << entry.second->json();
	}

	o <<  "}";
	return o.str();
}


//
//	OtDictClass::init
//

OtObject OtDictClass::init(OtContext, size_t count, OtObject* parameters) {
	// clear dictionary and add all calling parameters
	clear();

	for (size_t c = 0; c < count; c += 2) {
		insert(std::make_pair((std::string) *parameters[c], parameters[c + 1]));
	}

	return getSharedPtr();
}


//
//	OtDictReferenceClass
//

class OtDictReferenceClass;
typedef std::shared_ptr<OtDictReferenceClass> OtDictReference;

class OtDictReferenceClass : public OtInternalClass {
public:
	// constructors
	OtDictReferenceClass() = default;
	OtDictReferenceClass(OtDict d, const std::string& i) : dict(d), index(i) {}

	// index operations
	OtObject deref() { return dict->operator[] (index); }
	OtObject assign(OtObject value) { dict->operator[] (index) = value; return value; }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtDictReferenceClass>("DictReference", OtInternalClass::getMeta());
			type->set("__deref__", OtFunctionClass::create(&OtDictReferenceClass::deref));
			type->set("__assign__", OtFunctionClass::create(&OtDictReferenceClass::assign));
		}

		return type;
	}

	// create a new object
	static OtDictReference create(OtDict a, const std::string& i) {
		OtDictReference reference = std::make_shared<OtDictReferenceClass>(a, i);
		reference->setType(getMeta());
		return reference;
	}

private:
	OtDict dict;
	std::string index;
};


//
//	OtDictClass::index
//

OtObject OtDictClass::index(const std::string& index) {
	return OtDictReferenceClass::create(cast<OtDictClass>(), index);
}


//
//	OtDictClass::mySize
//

size_t OtDictClass::mySize() {
	return size();
}


//
//	OtDictClass::clone
//

OtObject OtDictClass::clone() {
	OtDict result = create();

	for (auto& it : *this) {
		result->insert(std::make_pair(it.first, it.second));
	}

	return result;
}


//
//	OtDictClass::eraseEntry
//

OtObject OtDictClass::eraseEntry(const std::string& name) {
	OtObject value = operator[] (name);
	erase(name);
	return value;
}


//
//	OtDictClass::keys
//

OtObject OtDictClass::keys() {
	// create array of all keys in context
	OtArray array = OtArrayClass::create();

	for (auto const& entry : *this) {
		array->push_back(OtStringClass::create(entry.first));
	}

	return array;
}


//
//	OtDictClass::values
//

OtObject OtDictClass::values() {
	// create array of all values in context
	OtArray array = OtArrayClass::create();

	for (auto const& entry : *this) {
		array->push_back(entry.second);
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

		type->set("__init__", OtFunctionClass::create(&OtDictClass::init));
		type->set("__index__", OtFunctionClass::create(&OtDictClass::index));

		type->set("size", OtFunctionClass::create(&OtDictClass::mySize));

		type->set("clone", OtFunctionClass::create(&OtDictClass::clone));
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

OtDict OtDictClass::create(size_t count, OtObject* values) {
	OtDict dict = create();

	for (size_t c = 0; c < count; c += 2) {
		dict->insert(std::make_pair((std::string) *values[c], values[c + 1]));
	}

	return dict;
}
