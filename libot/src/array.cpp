//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <sstream>

#include "ot/array.h"
#include "ot/function.h"
#include "ot/context.h"
#include "ot/reference.h"
#include "ot/iterator.h"


//
//	OtArrayClass::operator std::string
//

OtArrayClass::operator std::string() {
	std::ostringstream o;
	bool first = true;

	o << "[";

	for (auto const& entry : *this) {
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
//	OtArrayClass::init
//

OtObject OtArrayClass::init(OtContext, size_t count, OtObject* parameters) {
	clear();

	for (auto c = 0; c < count; c++) {
		push_back(parameters[c]);
	}

	return getSharedPtr();
}


//
//	OtArrayReferenceClass
//

class OtArrayReferenceClass;
typedef std::shared_ptr<OtArrayReferenceClass> OtArrayReference;

class OtArrayReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtArrayReferenceClass() = default;
	OtArrayReferenceClass(OtArray a, size_t i) : array(a), index(i) {}

	// index operations
	OtObject deref() { return array->operator[] (index); }
	OtObject assign(OtObject object) { array->operator[] (index) = object; return object; }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtArrayReferenceClass>("ArrayReference", OtReferenceClass::getMeta());
			type->set("__deref__", OtFunctionClass::create(&OtArrayReferenceClass::deref));
			type->set("__assign__", OtFunctionClass::create(&OtArrayReferenceClass::assign));
		}

		return type;
	}

	// create a new object
	static OtArrayReference create(OtArray a, size_t i) {
		OtArrayReference reference = std::make_shared<OtArrayReferenceClass>(a, i);
		reference->setType(getMeta());
		return reference;
	}

private:
	OtArray array;
	size_t index;
};


//
//	OtArrayClass::index
//

OtObject OtArrayClass::index(size_t index) {
	if (index < 0) {
		OT_EXCEPT("Negative index [%ld] is not allowed in array", index);

	} else if (index >= size()) {
		OT_EXCEPT("Index [%ld] is greater than array length [%ld]", index, size());

	}

	return OtArrayReferenceClass::create(cast<OtArrayClass>(), index);
}


//
//	OtArrayIteratorClass
//

class OtArrayIteratorClass;
typedef std::shared_ptr<OtArrayIteratorClass> OtArrayIterator;

class OtArrayIteratorClass : public OtIteratorClass {
public:
	// constructors
	OtArrayIteratorClass() = default;
	OtArrayIteratorClass(OtArray a) : array(a) {}

	// iteration operations
	bool end() { return index == array->size(); }
	OtObject next() { return array->operator[](index++); }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtArrayIteratorClass>("ArrayIterator", OtIteratorClass::getMeta());
			type->set("__end__", OtFunctionClass::create(&OtArrayIteratorClass::end));
			type->set("__next__", OtFunctionClass::create(&OtArrayIteratorClass::next));
		}

		return type;
	}

	// create a new object
	static OtArrayIterator create(OtArray a) {
		OtArrayIterator iterator = std::make_shared<OtArrayIteratorClass>(a);
		iterator->setType(getMeta());
		return iterator;
	}

private:
	OtArray array;
	size_t index {0};
};


//
//	OtArrayClass::iterate
//

OtObject OtArrayClass::iterate() {
	return OtArrayIteratorClass::create(cast<OtArrayClass>());
}


//
//	OtArrayClass::add
//

OtObject OtArrayClass::add(OtObject object) {
	OtArray result = create();

	for (auto& it : *this) {
		result->push_back(it);
	}

	result->append(object);
	return result;
}


//
//	OtArrayClass::contains
//

OtObject OtArrayClass::contains(OtObject object) {
	bool result = false;
	OtContext context = OtContextClass::create();

	for (auto it = begin(); it != end() && !result; it++) {
		result = object->method("__eq__", context, 1, &(*it))->operator bool();
	}

	return OtBooleanClass::create(result);
}


//
//	OtArrayClass::mySize
//

size_t OtArrayClass::mySize() {
	return size();
}


//
//	OtArrayClass::find
//

long OtArrayClass::find(OtObject object) {
	long result = -1;

	for (auto i = 0; i < size() && result == -1; i++) {
		if (object->method("__eq__", nullptr, 1, &(operator[] (i)))->operator bool()) {
			result = i;
		}
	}

	return result;
}


//
//	OtArrayClass::clone
//

OtObject OtArrayClass::clone() {
	OtArray result = create();

	for (auto& it : *this) {
		result->push_back(it);
	}

	return result;
}


//
//	OtArrayClass::join
//

OtObject OtArrayClass::join(OtObject object) {
	if (!object->isKindOf("Array")) {
		OT_EXCEPT("The array join expects another array instance, not a [%s]", object->getType()->getName().c_str());
	}

	OtArray result = create();

	for (auto& it : *this) {
		result->push_back(it);
	}

	for (auto& it : *(object->cast<OtArrayClass>())) {
		result->push_back(it);
	}

	return result;
}


//
//	OtArrayClass::append
//

OtObject OtArrayClass::append(OtObject object) {
	push_back(object);
	return getSharedPtr();
}


//
//	OtArrayClass::insert
//

OtObject OtArrayClass::insert(size_t index, OtObject object) {
	if (index < 0) {
		OT_EXCEPT("Negative index [%ld] is not allowed in array", index);

	} else if (index >= size()) {
		OT_EXCEPT("Index [%ld] is greater than array length [%ld]", index, size());

	}

	std::vector<OtObject>::insert(begin() + index, object);
	return getSharedPtr();
}


//
//	OtArrayClass::erase
//

OtObject OtArrayClass::erase(size_t index) {
	if (index < 0) {
		OT_EXCEPT("Negative index [%ld] is not allowed in array", index);

	} else if (index >= size()) {
		OT_EXCEPT("Index [%ld] is greater than array length [%ld]", index, size());

	}

	std::vector<OtObject>::erase(begin() + index);
	return getSharedPtr();
}


//
//	OtArrayClass::eraseMultiple
//

OtObject OtArrayClass::eraseMultiple(size_t index1, size_t index2) {
	if (index1 < 0) {
		OT_EXCEPT("Negative index [%ld] is not allowed in array", index1);

	} else if (index1 >= size()) {
		OT_EXCEPT("Index [%ld] is greater than array length [%ld]", index1, size());

	}

	if (index2 < 0) {
		OT_EXCEPT("Negative index [%ld] is not allowed in array", index2);

	} else if (index2 >= size()) {
		OT_EXCEPT("Index [%ld] is greater than array length [%ld]", index2, size());

	}

	if (index1 > index2) {
		OT_EXCEPT("Indexes [%ld and %ld2] are in the wrong order", index1, index2);
	}

	std::vector<OtObject>::erase(begin() + index1, begin() + index2);
	return getSharedPtr();
}


//
//	OtArrayClass::push
//

OtObject OtArrayClass::push(OtObject object) {
	push_back(object); return object;
}


//
//	OtArrayClass::pop
//

OtObject OtArrayClass::pop() {
	OtObject object = back();
	pop_back();
	return object;
}


//
//	OtArrayClass::getMeta
//

OtType OtArrayClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtArrayClass>("Array", OtCollectionClass::getMeta());

		type->set("string", OtFunctionClass::create(&OtArrayClass::operator std::string));

		type->set("__init__", OtFunctionClass::create(&OtArrayClass::init));

		type->set("__index__", OtFunctionClass::create(&OtArrayClass::index));
		type->set("__iter__", OtFunctionClass::create(&OtArrayClass::iterate));
		type->set("__add__", OtFunctionClass::create(&OtArrayClass::add));
		type->set("__contains__", OtFunctionClass::create(&OtArrayClass::contains));

		type->set("size", OtFunctionClass::create(&OtArrayClass::mySize));
		type->set("find", OtFunctionClass::create(&OtArrayClass::find));
		type->set("contains", OtFunctionClass::create(&OtArrayClass::contains));

		type->set("clone", OtFunctionClass::create(&OtArrayClass::clone));
		type->set("clear", OtFunctionClass::create(&OtArrayClass::clear));

		type->set("append", OtFunctionClass::create(&OtArrayClass::append));
		type->set("insert", OtFunctionClass::create(&OtArrayClass::insert));

		type->set("erase", OtFunctionClass::create(&OtArrayClass::erase));
		type->set("eraseMultiple", OtFunctionClass::create(&OtArrayClass::eraseMultiple));

		type->set("push", OtFunctionClass::create(&OtArrayClass::push));
		type->set("pop", OtFunctionClass::create(&OtArrayClass::pop));
	}

	return type;
}


//
//	OtArrayClass::create
//

OtArray OtArrayClass::create() {
	OtArray array = std::make_shared<OtArrayClass>();
	array->setType(getMeta());
	return array;
}

OtArray OtArrayClass::create(size_t count, OtObject* objects) {
	OtArray array = create();

	for (auto c = 0; c < count; c++) {
		array->push_back(objects[c]);
	}

	return array;
}
